#include "cuda_SSA.h"
#include <iostream>
using namespace std;

big_integer binpow(big_integer a, long long b){
    int base = 2;
    big_integer res({1}, false, base);
    while (b > 0) {
        if (b & 1)
            res = ssa_multiplication_cuda(res, a, base);
        a = ssa_multiplication_cuda(a, a, base);
        b >>= 1;
    }
    return res;
}

struct Matrix2x2 {
    big_integer m00, m01, m10, m11;
};

Matrix2x2 multiply(const Matrix2x2& A, const Matrix2x2& B) {
    Matrix2x2 C{big_integer(10), big_integer(10), big_integer(10), big_integer(10)};
    int target_base = 10;

    // Calculate C.m00 = A.m00 * B.m00 + A.m01 * B.m10
    big_integer term1 = ssa_multiplication_cuda(A.m00, B.m00, target_base);
    big_integer term2 = ssa_multiplication_cuda(A.m01, B.m10, target_base);
    C.m00 = term1 + term2;

    // Calculate C.m01 = A.m00 * B.m01 + A.m01 * B.m11
    term1 = ssa_multiplication_cuda(A.m00, B.m01, target_base);
    term2 = ssa_multiplication_cuda(A.m01, B.m11, target_base);
    C.m01 = term1 + term2;

    // Calculate C.m10 = A.m10 * B.m00 + A.m11 * B.m10
    term1 = ssa_multiplication_cuda(A.m10, B.m00, target_base);
    term2 = ssa_multiplication_cuda(A.m11, B.m10, target_base);
    C.m10 = term1 + term2;

    // Calculate C.m11 = A.m10 * B.m01 + A.m11 * B.m11
    term1 = ssa_multiplication_cuda(A.m10, B.m01, target_base);
    term2 = ssa_multiplication_cuda(A.m11, B.m11, target_base);
    C.m11 = term1 + term2;

    return C;
}

Matrix2x2 matrix_pow(Matrix2x2 base, long long exp) {
    if (exp < 0) {
        throw std::invalid_argument("Exponent must be non-negative.");
    }
    
    // Initialize result as the identity matrix
    Matrix2x2 result = {big_integer({1}, false, 10), big_integer({0}, false, 10), big_integer({0}, false, 10), big_integer({1}, false, 10)};

    while (exp > 0) {
        // If exponent is odd, multiply result by base
        if (exp % 2 == 1) {
            result = multiply(result, base);
        }
        // Square the base and halve the exponent
        base = multiply(base, base);
        exp /= 2;
    }
    return result;
}

big_integer fibonacci(long long n) {
    if (n <= 0) return big_integer(10);
    if (n == 1) return big_integer({1}, false, 10);

    // The base Fibonacci matrix
    Matrix2x2 F = {big_integer({1}, false, 10), big_integer({1}, false, 10), big_integer({1}, false, 10), big_integer({0}, false, 10)};

    // Raise the matrix to the power of (n-1)
    Matrix2x2 Fn_minus_1 = matrix_pow(F, n - 1);

    // The result F(n) is in the top-left corner of the matrix
    return Fn_minus_1.m00;
}

big_integer power_of_2(long long p){
    big_integer u({1}, false, 2);
    u = (u << p);
    return u;
}

bool is_prime(int p) {
    if (p <= 1) return false;
    if (p <= 3) return true;
    if (p % 2 == 0 || p % 3 == 0) return false;
    for (int i = 5; i * i <= p; i = i + 6) {
        if (p % i == 0 || p % (i + 2) == 0) {
            return false;
        }
    }
    return true;
}

// Tests if 2^p - 1 is prime
bool lucas_lehmer_test(int p) {
    // Step 1: The exponent 'p' must be an odd prime.
    if (p == 2) return true; // M_2 = 3 is prime
    if (!is_prime(p) || p % 2 == 0) {
        std::cerr << p << " Error: The exponent p must be an odd prime for the Lucas-Lehmer test." << std::endl;
        return false;
    }

    std::cout << "Starting Lucas-Lehmer test for M_" << p << " = 2^" << p << " - 1..." << std::endl;

    // Step 2: Define the Mersenne number M_p = 2^p - 1.
    // This requires big_integer arithmetic.
    big_integer M_p = power_of_2(p) - big_integer({1}, false, 2);
    std::cout << "Successfully calculated M_p." << std::endl;


    // Step 3: Initialize the Lucas-Lehmer sequence.
    // The sequence starts with s = 4.
    big_integer s = change_basis(big_integer({4}, false, 10), 2);
    big_integer two = change_basis(big_integer({2}, false, 10), 2);
    int target_base = 2; // Or whatever your function requires.

    // Step 4: Iterate p - 2 times.
    // The core of the test happens here.
    std::cout << "Starting " << p - 2 << " iterations..." << std::endl;
    for (int i = 0; i < p - 2; ++i) {
        // The critical calculation: s = s^2 - 2.
        // First, square s using your ultra-fast CUDA multiplication.
        s = ssa_multiplication_cuda(s, s, target_base);

        // Then, subtract 2.
        s = s - two;

        // Finally, take the result modulo M_p.
        // Your big_integer library's modulo operator must also be efficient.
        // The modulo operation can be optimized (e.g., using properties of 2^p - 1),
        // but a standard '%' is shown here for clarity.
        // while(s.digits.size() > p)s.digits.pop_back();
        s = s % M_p;
        
        // Optional: Print progress for very long tests
        if ((i + 1) % 100 == 0) {
            std::cout << "  Completed iteration " << i + 1 << "/" << p - 2 << std::endl;
        }
    }
    std::cout << "Iterations complete. Final value of s is " << s.print() << std::endl;

    // Step 5: Check the final result.
    // M_p is prime if and only if the final s is 0.
    return s == big_integer({0}, false, 2);
}

int main(){
    freopen("2-1e6.txt", "w", stdout);
    int exp = 1e6 * 1.5;
    big_integer u({2}, false, 10);
    u = change_basis(u, 2);
    big_integer result = binpow(u, exp);
    result = change_basis(result, 2);
    cout << "2^" << exp << " = " << endl;
    result.print();
    cout << endl;

    freopen("matrix.txt", "w", stdout);
    long long n = 1e6*1.5;
    result = fibonacci(n);
    result = change_basis(result, 10);
    cout << "fibonacci[" << n << "] = " << endl;
    result.print();
    cout << endl;   

    freopen("prime.txt", "w", stdout);
    
    // Perform the test
    int p = 127;
    bool is_mersenne_prime = lucas_lehmer_test(p);

    // Write the result to the file and the console
    if (is_mersenne_prime) {
        std::cout << "Result: 4095 = 2^" << p << "-1 is PRIME." << std::endl;
    } else {
        std::cout << "Result: 4095 = 2^" << p << "-1 is COMPOSITE." << std::endl;
    }




}