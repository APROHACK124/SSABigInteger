#include "NTT.cpp"
#include "bigInteger.cpp"

vector<ll> carry_propagation(const vector<ll>& coeffs, int base) {
    vector<ll> result;
    ll carry = 0;
    for (auto c : coeffs) {
        ll temp = c + carry;
        ll digit = temp % base;
        carry = temp / base;
        result.push_back(digit);
    }
    while (carry > 0) {
        ll digit = carry % base;
        carry = carry / base;
        result.push_back(digit);
    }
    return result;
}

int next_power_of_two(int x) {
    int n = 1;
    while (n < x) n <<= 1;
    return n;
}

// pair<vector<ll>, vector<ll>> ntt_big_integer(const big_integer& a, const big_integer& b, ll m, ll primitive_root, int N) {
//     int target_base = 256;
//     big_integer a_base256 = change_basis(a.negative ? -a : a, target_base);
//     big_integer b_base256 = change_basis(b.negative ? -b : b, target_base);

//     vector<ll> a_digits = a_base256.digits;
//     vector<ll> b_digits = b_base256.digits;

//     a_digits.resize(N, 0);
//     b_digits.resize(N, 0);

//     ll omega = pow_mod(primitive_root, (m - 1) / N, m);

//     vector<ll> a_ntt = ntt(a_digits, m, omega, false);
//     vector<ll> b_ntt = ntt(b_digits, m, omega, false);

//     return {a_ntt, b_ntt};
// }

big_integer ssa_multiplication(const big_integer& a, const big_integer& b) {
    bool negative = a.negative != b.negative;
    big_integer abs_a = a.negative ? -a : a;
    big_integer abs_b = b.negative ? -b : b;

    int target_base = 256;
    big_integer a_base256 = change_basis(abs_a, target_base);
    big_integer b_base256 = change_basis(abs_b, target_base);

    // cout << "Digits in a_base: " << a_base256.digits.size() << ", b_base: " << b_base256.digits.size() << "\n";
    // cout << a_base256.print() << endl << b_base256.print() << endl;

    int n_a = a_base256.digits.size();
    int n_b = b_base256.digits.size();
    int N = next_power_of_two(n_a + n_b - 1);

    vector<ll> a_digits = a_base256.digits;
    vector<ll> b_digits = b_base256.digits;
    a_digits.resize(N, 0);
    b_digits.resize(N, 0);

    ll m = 998244353; 
    ll primitive_root = 3;

    ntt_iterative(a_digits, m, primitive_root, false);
    ntt_iterative(b_digits, m, primitive_root, false);


    vector<ll> c_ntt(N);

    
    for (int i = 0; i < N; ++i) {
        c_ntt[i] = mul_mod(a_digits[i], b_digits[i], m);
    }

    // cout << "c_ntt: " << endl;
    // for(auto &i : c_ntt) cout << i << " ";
    // cout << endl;

    ntt_iterative(c_ntt, m, primitive_root, true);
    
    vector<ll> result_digits = carry_propagation(c_ntt, target_base);

    big_integer result(negative, result_digits, target_base);
    result = change_basis(result, a.base);
    return result;
}