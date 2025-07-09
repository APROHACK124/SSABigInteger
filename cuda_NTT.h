#ifndef CUDA_NTT_H
#define CUDA_NTT_H

#include <vector>
#include <cstdint>

using ll = long long;

// Function to perform faster bit-reverse permutation on a vector
void bit_reverse_permutation_faster(std::vector<ll>& a);

// CUDA kernel for scaling elements in inverse NTT
#ifdef __CUDACC__
__global__ void inverse_scale(ll* a, ll u, int N, int m);
__global__ void ntt_kernel(ll* a, ll* w_table, int n, int n_2, ll m, int len, int len_2);
#endif

// Function to perform bit-reverse permutation (CPU version)
void bit_reverse_permutation(std::vector<ll>& a);

// Iterative Number Theoretic Transform (NTT) on CPU
void ntt_iterative(std::vector<ll>& a, ll m, ll primitive_root, bool inverse);

#endif // CUDA_NTT_H