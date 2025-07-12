#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H

#include <vector>
#include <string>
#include <cstdint>

using ll = long long;

using namespace std;

// Extended Euclidean algorithm for GCD and coefficients
std::pair<ll, std::pair<ll, ll>> extended_euclid(ll n, ll m);

// Add digits of two vectors with a given modulus
std::vector<ll> add_digits(const std::vector<ll>& a, const std::vector<ll>& b, ll moduli);

// Subtract digits of two vectors with a given modulus
std::vector<ll> subtract_digits(std::vector<ll> a, std::vector<ll> b, ll moduli);

// Find the closest power of 2 greater than or equal to a value
int closest_power2_up(ll v);

// Big integer class for large number arithmetic
struct big_integer {
    bool negative;
    std::vector<ll> digits;
    int base;

    // Constructors
    big_integer(std::vector<ll> d, bool is_negative, int b);
    big_integer(bool is_neg, std::vector<ll> d, int b);
    big_integer(long long base);

    // Convert to long long
    ll to_longlong();

    // Find index of first non-zero digit
    int first_non_zero() const;

    // Print the number as a string
    string print() const;

    // Left shift operator
    big_integer operator<<(int amount) const;

    // Remove leading zeroes
    void leading_zeroes();

    // Comparison operators
    bool operator==(const big_integer& other) const;
    bool operator<(const big_integer& other) const;
    bool operator<=(const big_integer& other) const;
    bool operator!=(const big_integer& other) const;
    bool operator>(const big_integer& other) const;

    // Arithmetic operators
    big_integer operator+(const big_integer& other) const;
    big_integer operator-(const big_integer& other) const;
    big_integer operator/(const big_integer& other) const;
    big_integer operator%(const big_integer& other) const;
    big_integer operator*(const big_integer& other) const;

    // Check if non-zero
    bool nonzero() const;
};

// Unary negation operator
big_integer operator-(const big_integer a);

// Division function
big_integer division(big_integer a, big_integer b);

// Remainder function
big_integer remainder(big_integer a, big_integer b);

// Slow multiplication
big_integer slow_multiplication(const big_integer& a, const big_integer& other);

// Karatsuba multiplication
big_integer karatsuba(big_integer a, big_integer b);

// Convert long long to big_integer
big_integer ll_to_big_integer(ll u);

// Convert a number to a specified base
big_integer convert_base_10_to_x(ll u, ll b);

// Change the base of a big_integer
big_integer change_basis(const big_integer& bi, int new_base);

// Read big_integer from input
big_integer read_big_integer(int base);

#endif // BIG_INTEGER_H