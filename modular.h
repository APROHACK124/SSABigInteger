#ifndef MODULAR_H
#define MODULAR_H

#include <cstdint>

using ll = long long;

// Modular addition
ll add_mod(ll a, ll b, ll m);

// Modular subtraction
ll sub_mod(ll a, ll b, ll m);

// Modular multiplication
ll mul_mod(ll a, ll b, ll m);

// Modular exponentiation
ll pow_mod(ll a, ll b, ll m);

// Modular inverse using Fermat's little theorem
ll mod_inverse(ll a, ll m);

#endif // MODULAR_H