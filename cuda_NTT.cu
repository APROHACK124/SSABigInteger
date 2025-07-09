#include "modular.cpp"
#include <cuda_runtime.h>
#include <stdio.h>
#include <math.h>
#include <vector>
#include <random>
#include <time.h>
#include <chrono>
#include <iomanip>
#include <iostream>

using namespace std;

// TODO: convert to kernel (idk how)
void bit_reverse_permutation_faster(std::vector<ll>& a) {
    int n = a.size();

    for(int i = 1 , j = 0 ; i < n ; ++ i){
        int bit = n >> 1;
        for(; j & bit ; bit >>= 1)
            j ^= bit;
            j ^= bit;

        if(i < j)
            swap(a[i], a[j]);
    }

}

__global__ void inverse_scale(ll *a, ll u, int N, int m){
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if(idx < N){
        a[idx] = (a[idx] * u) % m;
    }
}

__global__ void ntt_kernel(ll *a, ll *w_table, int n, int n_2, ll m, int len, int len_2){
    int thread_idx = blockIdx.x * blockDim.x + threadIdx.x;

    if(thread_idx < n_2){
        int j = thread_idx % len_2;
        int i_base = (thread_idx / len_2) * len;

        int idx1 = i_base + j;
        int idx2 = idx1 + len_2;

        int w_idx = j * (n / len);
        ll w = w_table[w_idx];

        ll u = a[idx1];
        ll v = (a[idx2] * w) % m;

        a[idx1] = u + v < m ? u + v : u + v - m;
        a[idx2] = u - v >= 0 ? u - v : u - v + m;
    }

}




/**
 * @brief Reorders the input vector 'a' according to a bit-reversal permutation.
 * This is the first step in an iterative FFT/NTT algorithm.
 * * CUDA Porting Note: This entire function can be converted into a single
 * CUDA kernel where each thread is responsible for one or a few swaps.
 * @param a The vector to be permuted (must be a power of two in size).
 */
void bit_reverse_permutation(std::vector<ll>& a) {
    int n = a.size();
    int log_n = log2(n);

    for (int i = 0; i < n; i++) {
        int j = 0;
        for (int k = 0; k < log_n; k++) {
            if ((i >> k) & 1) {
                j |= 1 << (log_n - 1 - k);
            }
        }
        if (i < j) {
            std::swap(a[i], a[j]);
        }
    }
    
}

/**
 * @brief Performs an iterative, in-place Number Theoretic Transform (NTT).
 * This version is structured to be more easily parallelizable.
 *
 * @param a The input vector of numbers. Its size must be a power of two.
 * @param m The modulus for the finite field arithmetic.
 * @param primitive_root A primitive root of unity modulo m.
 * @param inverse If true, computes the inverse NTT.
 */
void ntt_iterative(std::vector<ll>& a, ll m, ll primitive_root, bool inverse) {
    int n = a.size();

    // 1. Reorder the input vector based on bit-reversal.
    bit_reverse_permutation(a);

    // 2. Perform the butterfly operations, stage by stage.
    // CUDA Porting Note: This outer loop over 'len' remains on the host (CPU).
    // For each 'len', a new CUDA kernel is launched to perform the butterfly
    // operations for that stage in parallel.

    int root = pow_mod(primitive_root, (m - 1) / n, m);
    int root_1 = mod_inverse(root, m);

    for (int len = 2; len <= n; len <<= 1) {
        // Calculate the twiddle factor for the current stage.

        ll wlen = inverse ? root_1 : root;

        for(int i = len ; i < n ; i <<= 1){
            wlen = (int)(1LL * wlen * wlen % m);
        }

        // CUDA Porting Note: The two loops below (over 'i' and 'j') are
        // parallelized. They can be combined into one large grid of threads in a kernel.
        // Each thread would compute one butterfly: a[i+j] and a[i+j+len/2].
        for (int i = 0; i < n; i += len) {
            ll w = 1;
            for (int j = 0; j < len / 2; j++) {
                ll u = a[i + j];
                ll v = mul_mod(a[i + j + len / 2], w, m);

                a[i + j] = u + v < m ? u + v : u + v - m;
                a[i + j + len / 2] = u - v >= 0 ? u - v : u - v + m;
                
                w = mul_mod(w, wlen, m);
            }
        }
    }

    // 3. If performing an inverse transform, scale the results.
    if (inverse) {
        ll inv_n = mod_inverse(n, m);
        // CUDA Porting Note: This can be a final, simple parallel kernel
        // where each thread scales one element of the array.
        for(int i = 0 ; i < n ; ++ i){
            a[i] = (a[i] * 1LL * inv_n) % m;
        }
    }

}