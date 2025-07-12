#include "modular.cpp" // Assuming this contains pow_mod
#include <vector>
#include <iostream>
using namespace std;

// Returns the prime factors of a number n
std::vector<ll> prime_factorize(ll n) {
    std::vector<ll> factors;
    for (ll i = 2; i * i <= n; ++i) {
        if (n % i == 0) {
            factors.push_back(i);
            while (n % i == 0) {
                n /= i;
            }
        }
    }
    if (n > 1) {
        factors.push_back(n);
    }
    return factors;
}

// Checks if g is a primitive root modulo p
bool is_primitive_root(ll g, ll p) {
    if (g <= 0 || g >= p) return false;
    
    ll p_minus_1 = p - 1;
    std::vector<ll> factors = prime_factorize(p_minus_1);

    for (ll factor : factors) {
        if (pow_mod(g, p_minus_1 / factor, p) == 1) {
            return false;
        }
    }
    return true;
}

int main(){
    int mod = 897581057;
    for(int i = 2 ; i < 10 ; ++ i){
        if(is_primitive_root(i, mod)){
            cout << i << "! " << endl;
        }
    }
}