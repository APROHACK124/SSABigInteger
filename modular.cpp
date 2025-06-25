#define ll long long

ll add_mod(ll a, ll b, ll m) {
    ll res = a + b;
    if (res >= m) res -= m;
    return res;
}

ll sub_mod(ll a, ll b, ll m) {
    ll res = a - b;
    if (res < 0) res += m;
    return res;
}

ll mul_mod(ll a, ll b, ll m) {
    unsigned long long res = a * b % m;
    return static_cast<ll>(res);
}

ll pow_mod(ll a, ll b, ll m) {
    ll res = 1;
    a = a % m;
    while (b > 0) {
        if (b & 1) res = mul_mod(res, a, m);
        a = mul_mod(a, a, m);
        b >>= 1;
    }
    return res;
}

ll mod_inverse(ll a, ll m) {
    return pow_mod(a, m - 2, m);
}