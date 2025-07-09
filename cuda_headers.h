#ifndef CUDA_SSA_H
#define CUDA_SSA_H

#include "bigInteger.cpp" // Assuming big_integer is defined in big_integer.h
#define ll long long


// Declare host functions
void bit_reverse_permutation_faster(std::vector<ll>& a);
void bit_reverse_permutation(std::vector<ll>& a);
void ntt_iterative(std::vector<ll>& a, ll m, ll primitive_root, bool inverse);
std::vector<ll> carry_propagation(const std::vector<ll>& coeffs, int base);
int next_power_of_two(int x);
std::vector<ll> fast_multiply_ntt(std::vector<ll> A, std::vector<ll> B, ll m, ll primitive_root, int desired_base);
big_integer ssa_multiplication_cuda(const big_integer& a, const big_integer& b);
big_integer ssa_multiplication(const big_integer& a, const big_integer& b);

// Declare CUDA kernels (only recognized by nvcc)
#ifdef __CUDACC__
__global__ void inverse_scale(ll* a, ll u, int N, int m);
__global__ void ntt_kernel(ll* a, ll* w_table, int n, int n_2, ll m, int len, int len_2);
__global__ void multiply_AB(ll* a, ll* b, ll* c, int N, ll m);
#endif

#endif // CUDA_SSA_H