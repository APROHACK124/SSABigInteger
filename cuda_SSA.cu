//cuda_SSA.cu
#include "cuda_NTT.h"
#include "modular.h"
#include "bigInteger.h"
#include <cuda_runtime.h>
#include <iostream>
#include <vector>
#define CHECK_CUDA_ERROR(val) check((val), #val, __FILE__, __LINE__)
using namespace std;

template <typename T>
void check(T err, const char* const func, const char* const file, const int line) {
    if (err != cudaSuccess) {
        fprintf(stderr, "CUDA error at %s:%d code=%d(%s) \"%s\" \n", file, line, static_cast<unsigned int>(err), cudaGetErrorString(err), func);
        exit(EXIT_FAILURE);
    }
}

vector<ll> carry_propagation(const vector<ll>& coeffs, int base) {
    vector<ll> result;
    ll carry = 0;
    for (auto c : coeffs) {
        ll temp = c + carry;
        ll digit = temp % base;
        carry = temp / base;
        result.push_back(digit);
    }
    while (carry > 0) {
        ll digit = carry % base;
        carry = carry / base;
        result.push_back(digit);
    }
    return result;
}

int next_power_of_two(int x) {
    int n = 1;
    while (n < x) n <<= 1;
    return n;
}

// pair<vector<ll>, vector<ll>> ntt_big_integer(const big_integer& a, const big_integer& b, ll m, ll primitive_root, int N) {
//     int target_base = 256;
//     big_integer a_base256 = change_basis(a.negative ? -a : a, target_base);
//     big_integer b_base256 = change_basis(b.negative ? -b : b, target_base);

//     vector<ll> a_digits = a_base256.digits;
//     vector<ll> b_digits = b_base256.digits;

//     a_digits.resize(N, 0);
//     b_digits.resize(N, 0);

//     ll omega = pow_mod(primitive_root, (m - 1) / N, m);

//     vector<ll> a_ntt = ntt(a_digits, m, omega, false);
//     vector<ll> b_ntt = ntt(b_digits, m, omega, false);

//     return {a_ntt, b_ntt};
// }

__global__ void multiply_AB(ll *a, ll *b, ll *c, int N, ll m){
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if(idx < N)
        c[idx] = (a[idx] * b[idx]) % m;
}

vector<ll>fast_multiply_ntt(vector<ll>A, vector<ll>B, ll m, ll primitive_root, int desired_base){

    // This didn't use gemini at all.

    // A, B are already permutated.

    const int THREADS_BLOCK = 256;

    ll *h_a, *h_b, *d_a, *d_b, *h_c, *d_c, *h_w, *d_w, *h_w_inv, *d_w_inv;
    cudaStream_t NTT_A, NTT_B;
    CHECK_CUDA_ERROR(cudaStreamCreate(&NTT_A));
    CHECK_CUDA_ERROR(cudaStreamCreate(&NTT_B));
    int N = A.size();
    size_t size = N * (sizeof (ll));

    int BLOCKS = (N + THREADS_BLOCK - 1) / THREADS_BLOCK;

    // Store a in an array
    CHECK_CUDA_ERROR(cudaMallocHost(&h_a, size));
    CHECK_CUDA_ERROR(cudaMallocHost(&h_b, size));
    CHECK_CUDA_ERROR(cudaMallocHost(&h_c, size));
    CHECK_CUDA_ERROR(cudaMallocHost(&h_w, size));
    CHECK_CUDA_ERROR(cudaMallocHost(&h_w_inv, size));

    CHECK_CUDA_ERROR(cudaMalloc(&d_a, size));
    CHECK_CUDA_ERROR(cudaMalloc(&d_b, size));
    CHECK_CUDA_ERROR(cudaMalloc(&d_c, size));
    CHECK_CUDA_ERROR(cudaMalloc(&d_w, size));
    CHECK_CUDA_ERROR(cudaMalloc(&d_w_inv, size));


    for(int i = 0 ; i < N ; ++ i){
        h_a[i] = A[i];
        h_b[i] = B[i];
        h_c[i] = 0;
    }
    
    int root = pow_mod(primitive_root, (m - 1) / N, m);
    int root_1 = mod_inverse(root, m);

    h_w[0] = 1;
    h_w_inv[0] = 1;
    for(int i = 1 ; i < N ; ++ i){
        h_w[i] = (h_w[i - 1] * root) % m;
        h_w_inv[i] = (h_w_inv[i - 1] * root_1) % m;
    }

    // Copy to Device
    CHECK_CUDA_ERROR(cudaMemcpyAsync(d_w, h_w, size, cudaMemcpyHostToDevice));
    CHECK_CUDA_ERROR(cudaMemcpyAsync(d_w_inv, h_w_inv, size, cudaMemcpyHostToDevice));
    CHECK_CUDA_ERROR(cudaDeviceSynchronize());
    CHECK_CUDA_ERROR(cudaMemcpyAsync(d_a, h_a, size, cudaMemcpyHostToDevice, NTT_A));
    CHECK_CUDA_ERROR(cudaMemcpyAsync(d_b, h_b, size, cudaMemcpyHostToDevice, NTT_B));
    CHECK_CUDA_ERROR(cudaMemcpyAsync(d_c, h_c, size, cudaMemcpyHostToDevice, NTT_A));


    // Apply NTT


    for(int len = 2 ; len <= N ; len <<= 1){
        ntt_kernel<<<BLOCKS, THREADS_BLOCK, 0, NTT_A>>>(d_a, d_w, N, N/2, m, len, len / 2);
    }
    for(int len = 2 ; len <= N ; len <<= 1){
        ntt_kernel<<<BLOCKS, THREADS_BLOCK, 0, NTT_B>>>(d_b, d_w, N, N/2, m, len, len / 2);
    }

    CHECK_CUDA_ERROR(cudaStreamSynchronize(NTT_A));
    CHECK_CUDA_ERROR(cudaStreamSynchronize(NTT_B));

    // Multiply
    multiply_AB<<<BLOCKS, THREADS_BLOCK, 0, NTT_A>>>(d_a, d_b, d_c, N, m);

    // Copy back to RAM

    vector<ll>result_c(N);
    CHECK_CUDA_ERROR(cudaMemcpyAsync(result_c.data(), d_c, size, cudaMemcpyDeviceToHost));
    CHECK_CUDA_ERROR(cudaDeviceSynchronize());

    // Bit reverse for c
    bit_reverse_permutation_faster(result_c);

    // Copy back to device

    CHECK_CUDA_ERROR(cudaMemcpyAsync(d_c, result_c.data(), size, cudaMemcpyHostToDevice));
    CHECK_CUDA_ERROR(cudaDeviceSynchronize());


    // Perform inverse NTT
    for(int len = 2 ; len <= N ; len <<= 1){
        ntt_kernel<<<BLOCKS, THREADS_BLOCK, 0, NTT_A>>>(d_c, d_w_inv, N, N/2, m, len, len / 2);
    }

    
    // Scale results
    ll inv_n = mod_inverse(N, m);
    inverse_scale<<<BLOCKS, THREADS_BLOCK, 0, NTT_A>>>(d_c, inv_n, N, m);

    CHECK_CUDA_ERROR(cudaDeviceSynchronize());
    

    // Copy back to device

    CHECK_CUDA_ERROR(cudaMemcpyAsync(result_c.data(), d_c, size, cudaMemcpyDeviceToHost));
    CHECK_CUDA_ERROR(cudaDeviceSynchronize());

    cudaFree(d_a);
    cudaFree(d_b);
    cudaFree(d_c);
    cudaFree(d_w);
    cudaFree(d_w_inv);
    cudaStreamDestroy(NTT_A);
    cudaStreamDestroy(NTT_B);
    cudaFreeHost(h_a);
    cudaFreeHost(h_b);
    cudaFreeHost(h_c);
    cudaFreeHost(h_w);
    cudaFreeHost(h_w_inv);

    return result_c;
}

big_integer ssa_multiplication_cuda(const big_integer& a, const big_integer& b) {
    bool negative = a.negative != b.negative;
    big_integer abs_a = a.negative ? -a : a;
    big_integer abs_b = b.negative ? -b : b;

    int target_base = 256;
    big_integer a_base256 = change_basis(abs_a, target_base);
    big_integer b_base256 = change_basis(abs_b, target_base);


    int n_a = a_base256.digits.size();
    int n_b = b_base256.digits.size();
    int N = next_power_of_two(n_a + n_b - 1);

    vector<ll> a_digits = a_base256.digits;
    vector<ll> b_digits = b_base256.digits;
    a_digits.resize(N, 0);
    b_digits.resize(N, 0);

    

    bit_reverse_permutation_faster(a_digits);
    bit_reverse_permutation_faster(b_digits);

    // Update depending on the number of moduli to use

    ll m = 998244353; 
    ll primitive_root = 3;
    vector<ll>result = fast_multiply_ntt(a_digits, b_digits, m, primitive_root, target_base);

    // TODO: Join everything with CRT
    
    vector<ll> result_digits = carry_propagation(result, target_base);

    big_integer biResult(negative, result_digits, target_base);
    biResult = change_basis(biResult, a.base);
    return biResult;
}

big_integer ssa_multiplication(const big_integer& a, const big_integer& b) {
    bool negative = a.negative != b.negative;
    big_integer abs_a = a.negative ? -a : a;
    big_integer abs_b = b.negative ? -b : b;

    int target_base = 256;
    big_integer a_base256 = change_basis(abs_a, target_base);
    big_integer b_base256 = change_basis(abs_b, target_base);

    // cout << "Digits in a_base: " << a_base256.digits.size() << ", b_base: " << b_base256.digits.size() << "\n";
    // cout << a_base256.print() << endl << b_base256.print() << endl;

    int n_a = a_base256.digits.size();
    int n_b = b_base256.digits.size();
    int N = next_power_of_two(n_a + n_b - 1);

    vector<ll> a_digits = a_base256.digits;
    vector<ll> b_digits = b_base256.digits;
    a_digits.resize(N, 0);
    b_digits.resize(N, 0);

    ll m = 998244353; 
    ll primitive_root = 3;

    ntt_iterative(a_digits, m, primitive_root, false);
    ntt_iterative(b_digits, m, primitive_root, false);


    vector<ll> c_ntt(N);

    
    for (int i = 0; i < N; ++i) {
        c_ntt[i] = mul_mod(a_digits[i], b_digits[i], m);
    }

    // cout << "c_ntt: " << endl;
    // for(auto &i : c_ntt) cout << i << " ";
    // cout << endl;

    ntt_iterative(c_ntt, m, primitive_root, true);
    
    vector<ll> result_digits = carry_propagation(c_ntt, target_base);

    big_integer result(negative, result_digits, target_base);
    result = change_basis(result, a.base);
    return result;
}