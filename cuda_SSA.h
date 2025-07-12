#ifndef CUDA_SSA_H
#define CUDA_SSA_H

#include <vector>
#include <cstdint>
#include "bigInteger.h"

using ll = long long;

#ifdef __CUDACC__
// Error checking macro for CUDA calls
#define CHECK_CUDA_ERROR(val) check((val), #val, __FILE__, __LINE__)
// CUDA kernel for element-wise multiplication of two arrays
__global__ void multiply_AB(ll* a, ll* b, ll* c, int N, ll m);
#endif

// Template function to check CUDA errors
template <typename T>
void check(T err, const char* const func, const char* const file, const int line);

// Function to propagate carry in coefficient arithmetic
std::vector<ll> carry_propagation(const std::vector<ll>& coeffs, int base);

// Function to find the next power of two
int next_power_of_two(int x);


// Fast multiplication using NTT on CUDA
std::vector<ll> fast_multiply_ntt(std::vector<ll> A, std::vector<ll> B, ll m, ll primitive_root, int desired_base);

// SSA multiplication using CUDA
big_integer ssa_multiplication_cuda(const big_integer& a, const big_integer& b, int target_base);

// SSA multiplication on CPU
big_integer ssa_multiplication(const big_integer& a, const big_integer& b, int target_base);

#endif // CUDA_SSA_H