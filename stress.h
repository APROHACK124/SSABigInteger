#ifndef STRESS_H
#define STRESS_H

#include "bigInteger.h"
#include <random>

// Interactive test for big_integer operations
void test();

// Random number generator initialization
extern std::mt19937 rng;

// Generate a random integer in range [l, r]
int random(int l, int r);

// Stress test for basic big_integer operations
int stress_test();

// Stress test for base conversion in big_integer
int stress_test_basis();

// Stress test comparing SSA and Karatsuba multiplication
int stress_test_SSA_karatsuba();

// Stress test for SSA multiplication
int stress_test_SSA();

// Stress test for SSA multiplication (CUDA version, placeholder)
int stress_test_SSA_SSACUDA();

int stress_test_SSACUDA();

#endif // STRESS_H