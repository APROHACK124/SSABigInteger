#include "modular.cpp"
#include <math.h>
#include <vector>
#include <random>
#include <time.h>
#include <chrono>
#include <iomanip>
#include <iostream>

using namespace std;

// This works but it's hard to add parallelism.
// void recursive_ntt(std::vector<ll>& a, ll m, ll omega, bool inverse) {
//     int n = a.size();
//     if (n <= 1) return;

//     std::vector<ll> a0(n / 2), a1(n / 2);
//     for (int i = 0; i < n / 2; i++) {
//         a0[i] = a[i * 2];
//         a1[i] = a[i * 2 + 1];
//     }

//     recursive_ntt(a0, m, pow_mod(omega, 2, m), inverse);
//     recursive_ntt(a1, m, pow_mod(omega, 2, m), inverse);

//     ll w = 1;
//     ll w_n = inverse ? mod_inverse(omega, m) : omega;
//     for (int i = 0; i < n / 2; i++) {
//         ll t = mul_mod(w, a1[i], m);
//         a[i] = add_mod(a0[i], t, m);
//         a[i + n / 2] = sub_mod(a0[i], t, m);
//         w = mul_mod(w, w_n, m);
//     }
// }


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

// void ntt(std::vector<ll>& a, ll m, ll primitive_root, bool inverse) {
//     int n = a.size();
//     ll omega = pow_mod(primitive_root, (m - 1) / n, m);
//     recursive_ntt(a, m, omega, inverse);

//     if (inverse) {
//         ll inv_n = mod_inverse(n, m);
//         for (ll& x : a) {
//             x = mul_mod(x, inv_n, m);
//         }
//     }
// }

