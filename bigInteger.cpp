// bigInteger.cpp
#include "bigInteger.h"
#include <iostream>
#include <vector>
#include <string.h>
#include <assert.h>
#include <algorithm>
#include <chrono>
#include <time.h>

#define ll long long
using namespace std;

string pr(pair<long long, pair<long long, long long >> a){
	cout << a.first << " " << a.second.first << " " << a.second.second << "\n";
	return "";
}

pair<long long, pair<long long, long long>>extended_euclid(long long n, long long m){
	pair<long long, pair<long long, long long>>u, v;
	
	
	u.first = abs(n), u.second.first = 1, u.second.second = 0;
	v.first = abs(m), v.second.first = 0, v.second.second = 1;


	// cout << "u = " << pr(u) << endl;
	// cout << "v = " << pr(v) << endl;

	while(v.first != 0){
		cout << "- Pasamos a calcular $\\gcd(u_{1} = " << u.first << ", " << "v_{1} = " << v.first << ")$\n";
		long long q = u.first / v.first;
		// cout << " hallando gcd " << u.first << " " << v.first << endl;

		cout << "- $u = (" << u.first << ", " << u.second.first << ", " << u.second.second << ")$."  << endl;
		cout << "- $v = (" << v.first << ", " << v.second.first << ", " << v.second.second << ")$."  << endl;

		cout << "- Hallamos $q$ y $0 \\leq r_{1} < v_{1}$ tales que $u_{1} = q \\cdot v_{1} + r_{1}$. ";
		auto r = u;
		
		r.first = u.first - q * v.first;
		r.second.first = u.second.first - q * v.second.first;
		r.second.second = u.second.second - q * v.second.second;
		cout << "En este caso, $q = " << q << ", r_{1} = " << r.first << "$.\n";
		cout << "- $r_{2} = u_{2} - q \\cdot v_{2} = " << u.second.first << " - " << q << " \\cdot " << v.second.first << " = " << r.second.first << "$.\n";
		cout << "- $r_{3} = u_{3} - q \\cdot v_{3} = " << u.second.second << " - " << q << " \\cdot " << v.second.second << " = " << r.second.second << "$.\n";
		cout << "- Por tanto, $r_{1} = " << r.first << " = n \\cdot " << r.second.first << " + " << "m \\cdot "<< r.second.second  << "$."<< endl << endl;
		u = v;
		v = r;
	}
	return u;
}

vector<long long>add_digits(const vector<long long>&a, const vector<long long>&b, long long moduli){
	vector<long long>result(max(a.size(), b.size()));

	int carry = 0;

	for(int i = 0 ; i < max(a.size(), b.size()) ; ++ i){
		result[i] = ((i < a.size() ? a[i] : 0) + (i < b.size() ? b[i] : 0) + carry) % moduli;
		if(((i < a.size() ? a[i] : 0) + (i < b.size() ? b[i] : 0) + carry) >= moduli) carry = 1;
		else carry = 0;
	}

	if(carry)result.push_back(carry);
	

	return result;
}

vector<long long>subtract_digits(vector<long long>a, vector<long long>b, long long moduli){

	while(!a.empty() && a.back() == 0)a.pop_back();
	while(!b.empty() && b.back() == 0)b.pop_back();

	if(b.size() > a.size())cout << a.size() << " " << b.size() << endl;
	assert(a.size() >= b.size());
	vector<long long>result(a.size());

	int carry = 0;

	for(int i = 0 ; i < a.size() ; ++ i){
		if(carry){
			a[i] -= 1;
		}
		result[i] = ((i < a.size() ? a[i] : 0) - (i < b.size() ? b[i] : 0));
		if(result[i] < 0){
			result[i] += moduli;
			carry = 1;
		}else carry = 0;
	}
	if(!result.empty() && result.back() == 0)result.pop_back();

	return result;
}

int closest_power2_up(long long v){
	for(int i = 0 ; i < 60 ; ++ i){
		if((1ll << i) >= v)return i;
	}
}


	big_integer::big_integer(vector<long long>d, bool is_negative, int b) : digits(d) {
		base = b;
		negative = is_negative;
	}

	big_integer::big_integer(bool is_neg, vector<long long>d, int b) : negative(is_neg), digits(d), base(b) {
		leading_zeroes();
	}

	big_integer::big_integer(long long base){
		digits = {0};
		base = base;
		negative = false;
	}



	ll big_integer::to_longlong(){

		leading_zeroes();

		long long B = 1;
		long long result = 0;
		for(int i = 0 ; i < digits.size() ; ++ i){
			result += B * digits[i];
			if(i + 1 < digits.size() )B *= base;
		}

		if(negative)result *= -1;
		return result;
	}

	int big_integer::first_non_zero() const {
		for(int i = (int)digits.size() - 1 ; i >= 0 ; -- i){
			if(digits[i] != 0)return i;
		}
		return -1;
	}

	string big_integer::print() const {
		if(negative)cout << "-" ;
		auto aux = digits;
///		while(!aux.empty() && aux.back() == 0)aux.pop_back();
		for(int i = (int)aux.size() - 1 ; i >= 0 ; -- i){
			cout << aux[i];
			if(base > 10)cout << " ";
		}
		if(aux.empty())cout << 0;
		return "";
	}

	big_integer big_integer::operator<<(int amount) const {
		vector<long long>aux(amount, 0);
		big_integer result(aux, false, base);

		for(int i = 0 ; i < digits.size() ; ++ i){
			result.digits.push_back(digits[i]);
		}
		return result;
	}

	void big_integer::leading_zeroes(){
		while(digits.size() > 1 && digits.back() == 0)digits.pop_back();
		if(digits.empty()){
			digits.push_back(0);
			negative = false;
		}
	}

	bool big_integer::operator== (const big_integer &other) const {

		
		int f = first_non_zero(), o = other.first_non_zero();

		if(f != o)return false;
		for(int i = f ; i >= 0 ; -- i){
			if(digits[i] != other.digits[i])return false;
		}
		if(negative != other.negative)return false;
		return true;
	}

	bool big_integer::operator<(const big_integer &other) const {
		if(negative != other.negative){
			return negative;
		}

		int s = first_non_zero(), t = other.first_non_zero();
		if(s == -1 && t == -1)return false;

		if(s == -1)return true != negative;
		if(t == -1)return false != negative;

		if(s > t)return false != negative;
		if(s < t)return true != negative;
		for(int i = s ; i >= 0 ; -- i){
			if(other.digits[i] > digits[i])return true != negative;
			if(other.digits[i] < digits[i])return false != negative;
		}
		return false;
	}


	bool big_integer::operator<=(const big_integer &other) const{
		return (*this < other) || (*this == other);
	}

	big_integer big_integer::operator+(const big_integer &other) const {

		if(negative == other.negative){
			big_integer result(add_digits(digits, other.digits, base), negative, base);
			return result;
		}else{
			if(other.negative){
				big_integer aux(other.digits, false, base);
				return *this - aux;
			}else{
				big_integer aux(digits, false, base);
				return other - aux;
			}
		}

	}

	bool big_integer::nonzero() const {
		for(auto &u : digits){
			if(u != 0)return true;
		}
		return false;
	}

	bool big_integer::operator!=(const big_integer &other) const {
		return not (*this == other);
	}

	bool big_integer::operator>(const big_integer &other) const {
		return other < *this;
	}

	big_integer big_integer::operator-(const big_integer &other) const {

		if(other.negative){
			big_integer aux(other);
			aux.negative = false;
			aux.base = base;
			return *this + aux;
		}

		if(negative){
			big_integer aux(add_digits(digits, other.digits, base), true, base);
			return aux;
		}

		if(*this < other){
			big_integer result(subtract_digits(other.digits, digits, base), true, base);
			return result;
		}else{
			big_integer result(subtract_digits(digits, other.digits, base), false, base);
			return result;
		}
	}
	

	big_integer big_integer::operator/(const big_integer &other) const{
		return division(*this, other);
	}

	big_integer big_integer::operator%(const big_integer &other) const {
		return remainder(*this, other);	
	}

	// big_integer operator*(const big_integer &other) const {
	// 	big_integer result(vector<long long>(digits.size() + other.digits.size() + 2, 0));
	// 	for(int i = 0 ; i < digits.size() ; ++ i){
	// 		for(int j = 0 ; j < other.digits.size() ; ++ j){
	// 			result.digits[i + j] += digits[i] * other.digits[j];
	// 			if(result.digits[i + j] >= base){
	// 				result.digits[i + j + 1] += result.digits[i + j] / base;
	// 				result.digits[i + j] %= base;
	// 			}
	// 		}
	// 	}
	// 	result.leading_zeroes();

	// 	if(negative != other.negative)result.negative = true;

	// 	return result;
	// }

	big_integer big_integer::operator*(const big_integer &other) const {

		big_integer aux(*this);
		big_integer o(other);
		bool isneg = (aux.negative != o.negative);
		aux.negative = false;
		o.negative = false;

		big_integer result = karatsuba(aux, o);
		result.negative = isneg;
		result.base = other.base;
		return result;
	}

	



big_integer ll_to_big_integer(long long u){
	big_integer result({0}, false, 10);
	if(u < 0)result.negative = true;
	u = abs(u);
	result.digits.clear();
	while(u != 0){
		result.digits.push_back(u % 10);
		u /= 10;
	}
	result.leading_zeroes();
}

big_integer convert_base_10_to_x(long long u, long long b){
	vector<long long>result;
	long long cur = b;
	bool isneg = u < 0;
	u = abs(u);
	while(cur * b <= u)cur *= b;
	while(cur > 0){
		result.push_back(u / cur);
		u %= cur;
		cur /= b;
	}
	reverse(result.begin(), result.end());
	return big_integer(isneg, result, b);
}

big_integer division(big_integer a, big_integer b){
	// a / b
	big_integer u(a), v(b);

	// cout << "dividing " << a.print() << " " << b.print() << endl;

	u.leading_zeroes();
	v.leading_zeroes();

	if(v.digits.size() == 1 && v.digits[0] == 0){
		throw runtime_error("Division by 0");
	}

	big_integer result({0}, false, u.base);
	result.negative = u.negative != v.negative;

	u.negative = false;
	v.negative = false;

	if(u < v){
		// Result is 0
		result.digits = {0};
		result.negative = false;
		return result;
	}

	if(v.digits.size() == 1 && v.digits[0] == 1){
		bool neg = result.negative;
		result =  big_integer(u);
		result.negative = neg;
		return result;
	}
	

	// Normalization
	long long d = (long long) (u.base) / (v.digits.back() + 1);
	big_integer u_norm(u);
	big_integer v_norm(v);

	if(d > 1){
		big_integer multiplier({0}, false, u.base);
		multiplier.digits = {d};
		u_norm = u_norm * multiplier;
		v_norm = v_norm * multiplier;
	}

	// cout << "after normalization: " << u_norm.print() << " " << v_norm.print() << endl;


	int n = v_norm.digits.size();
	int m = u_norm.digits.size() - n;
	result.digits.assign(m + 1, 0);

	u_norm.digits.resize(m + n + 1, 0); // Asegura que tenga suficientes digitos

	for(int j = m ; j >= 0 ; -- j){

		// cout << "j = " << j << endl;
		// Estimar el cociente
		long long estimation = (j + n < u_norm.digits.size() ? static_cast<__int128_t>(u_norm.digits[j + n]) * u.base : 0) +
                               (j + n - 1 < u_norm.digits.size() ? u_norm.digits[j + n - 1] : 0);
		long long v1 = v_norm.digits[n - 1];
		long long v2 = (n >= 2) ? v_norm.digits[n - 2] : 0;
		long long u2 = (j + n - 2 >= 0 && j + n - 2 < u_norm.digits.size()) ? u_norm.digits[j + n - 2] : 0;

		long long qhat = estimation / v1;
		__int128_t rhat = estimation % v1;

		// cout << "1. qhat = " << qhat << endl;

		while(qhat >= u.base || (qhat > 0 && qhat * v2 > rhat * u.base + u2)){
			// cout << "qhat --!" << endl;
			qhat -- ;
			rhat += v1;
			if (qhat <= 0 || rhat >= u.base) break;
		}


		big_integer temp(v_norm);
		temp = temp * big_integer(false, {qhat}, u.base);
		big_integer shifted_temp = temp;
		shifted_temp.digits.insert(shifted_temp.digits.begin(), j, 0);


		big_integer trial = u_norm - shifted_temp;
		// cout << "trial: " << trial.print() << endl;
		if(trial.negative && trial.nonzero()){
			// cout << "qhat --" << endl;
			qhat -- ;
			temp = v_norm;
			temp = temp * big_integer(false, {qhat}, u.base);
			shifted_temp = temp;
			shifted_temp.digits.insert(shifted_temp.digits.begin(), j, 0);
			trial = u_norm - shifted_temp;
		}

		// cout << "resulting qhat " << qhat << endl;


		u_norm = trial;

		// cout << "new u_norm " << u_norm.print() << endl;
		result.digits[j] = qhat;
	}

	result.leading_zeroes();
	// cout << "result " << result.print() << endl;
	return result;
}

big_integer remainder(big_integer a, big_integer b) {
    // a / b, returns remainder instead of quotient
    big_integer u(a), v(b);

    u.leading_zeroes();
    v.leading_zeroes();

    if (v.digits.size() == 1 && v.digits[0] == 0) {
        throw runtime_error("Division by 0");
    }

    // Initialize result to store the remainder (not quotient)
    big_integer remainder(u); // Start with u, will be updated to remainder
    remainder.negative = u.negative; // Remainder has same sign as dividend

    u.negative = false;
    v.negative = false;

    if (u < v) {
        // If u < v, the remainder is u itself
        return remainder;
    }

    if (v.digits.size() == 1 && v.digits[0] == 1) {
        // If dividing by 1, remainder is 0
        remainder.digits = {0};
        remainder.negative = false;
        return remainder;
    }

    // Normalization
    long long d = (long long)(u.base) / (v.digits.back() + 1);
    big_integer u_norm(u);
    big_integer v_norm(v);

    if (d > 1) {
        big_integer multiplier({0}, false, u.base);
        multiplier.digits = {d};
        u_norm = u_norm * multiplier;
        v_norm = v_norm * multiplier;
    }

    int n = v_norm.digits.size();
    int m = u_norm.digits.size() - n;
    big_integer result({0}, false, u.base); // Still needed for quotient calculation
    result.digits.assign(m + 1, 0);

    u_norm.digits.resize(m + n + 1, 0); // Ensure enough digits

    for (int j = m; j >= 0; --j) {
        // Estimate quotient
        long long estimation = (j + n < u_norm.digits.size() ? static_cast<__int128_t>(u_norm.digits[j + n]) * u.base : 0) +
                               (j + n - 1 < u_norm.digits.size() ? u_norm.digits[j + n - 1] : 0);
        long long v1 = v_norm.digits[n - 1];
        long long v2 = (n >= 2) ? v_norm.digits[n - 2] : 0;
        long long u2 = (j + n - 2 >= 0 && j + n - 2 < u_norm.digits.size()) ? u_norm.digits[j + n - 2] : 0;

        long long qhat = estimation / v1;
        __int128_t rhat = estimation % v1;

        while (qhat >= u.base || (qhat > 0 && qhat * v2 > rhat * u.base + u2)) {
            qhat--;
            rhat += v1;
            if (qhat <= 0 || rhat >= u.base) break;
        }

        big_integer temp(v_norm);
        temp = temp * big_integer(false, {qhat}, u.base);
        big_integer shifted_temp = temp;
        shifted_temp.digits.insert(shifted_temp.digits.begin(), j, 0);

        big_integer trial = u_norm - shifted_temp;
        if (trial.negative && trial.nonzero()) {
            qhat--;
            temp = v_norm;
            temp = temp * big_integer(false, {qhat}, u.base);
            shifted_temp = temp;
            shifted_temp.digits.insert(shifted_temp.digits.begin(), j, 0);
            trial = u_norm - shifted_temp;
        }

        u_norm = trial;
        result.digits[j] = qhat;
    }

    // Denormalize the remainder
    if (d > 1) {
        big_integer divisor({0}, false, u.base);
        divisor.digits = {d};
        u_norm = u_norm / divisor; // Divide u_norm by d to reverse normalization
    }

    // Set the remainder
    remainder = u_norm;
    remainder.negative = u.negative; // Remainder has same sign as dividend
    remainder.leading_zeroes();

    return remainder;
}

big_integer slow_multiplication(const big_integer &a, const big_integer &other) {

	big_integer result(vector<long long>(a.digits.size() + other.digits.size() + 2, 0), false, a.base);
	for(int i = 0 ; i < a.digits.size() ; ++ i){
		for(int j = 0 ; j < other.digits.size() ; ++ j){
			result.digits[i + j] += a.digits[i] * other.digits[j];
			if(result.digits[i + j] >= a.base){
				result.digits[i + j + 1] += result.digits[i + j] / a.base;
				result.digits[i + j] %= a.base;
			}
		}
	}
	result.leading_zeroes();

	if(a.negative != other.negative)result.negative = true;

	return result;
}

big_integer operator-(const big_integer a){
	big_integer ret(a);
	ret.negative = true;
	return ret;
}

big_integer change_basis(const big_integer& bi, int new_base) {
    // Check if the number is zero
	big_integer ret({0}, false, new_base);
    if (bi.digits.size() == 1 && bi.digits[0] == 0) {
		return ret;
    }

	if(bi.base == new_base)return bi;

    // Store the sign and work with absolute value
    bool is_neg = bi.negative;
    big_integer abs_bi = bi;
    abs_bi.negative = false;

    // Represent new_base in the current base (bi.base)
    vector<long long> b2_digits;
    long long temp = new_base;
    while (temp > 0) {
        b2_digits.push_back(temp % bi.base);
        temp /= bi.base;
    }
    big_integer b2_in_b1(false, b2_digits, bi.base);

    // Collect digits in the new base via repeated division
    vector<long long> new_digits;
    while (abs_bi > big_integer(false, {0}, bi.base)) {
        big_integer remainder = abs_bi % b2_in_b1;
        long long digit = remainder.to_longlong();
        new_digits.push_back(digit);
        abs_bi = abs_bi / b2_in_b1;
    }

	ret.digits = new_digits;
	ret.base = new_base;
	ret.negative = is_neg;
	ret.leading_zeroes();
	return ret;
}

big_integer karatsuba(big_integer a, big_integer b){

		a.leading_zeroes();
		b.leading_zeroes();
		// cout << "Multiplicando: $a = " << a.print() << ", b = " << b.print() << "$" << endl;
		int closest_power = closest_power2_up(max(a.digits.size(), b.digits.size()));
		int sz = (1ll << closest_power);
		// cout << "sz: " << sz << endl;
		bool normalized = true;
		if(a.digits.size() < sz || b.digits.size() < sz)normalized = false;

		
		if(a.digits.size() < 2 || b.digits.size() < 2 || (sz <= 2 && a.base >= 100)){
			return slow_multiplication(a, b);
		}
		if(!normalized){
			
			while(a.digits.size() < sz)a.digits.push_back(0);
			while(b.digits.size() < sz)b.digits.push_back(0);

			// cout << "- Se igualan los tamaños de ambos numeros a la potencia de 2 mas cercana." << endl;
			// cout << "- $a = " << a.print() << ", b = " << b.print() << "$."<< endl;
			normalized = true;
		}


		if(sz == 2){
			// cout << "- Caso base: multiplicacion de dos enteros de dos cifras.";
			long long m1 = (a.digits[1] + a.digits[0]) * (b.digits[1] + b.digits[0]);
			long long m2 = a.digits[1] * b.digits[1];
			long long m3 = a.digits[0] * b.digits[0];

			// cout << "- $a = " << a.print() << ", b = " << b.print() << "$" << endl;

			
			big_integer result = convert_base_10_to_x(m3 + (m1 - m2 - m3) * a.base + m2 * a.base * a.base, a.base);
			
			// cout << "resultado: $" << result.print() << "$." << endl;

			return result;
		}

		// cout << "- Se dividen los numeros en mitades: " << endl;
		big_integer u0({0}, false, a.base), u1({0}, false, a.base), v0({0}, false, a.base), v1({0}, false, a.base);
		u0.digits.clear();
		u1.digits.clear();
		v0.digits.clear();
		v1.digits.clear();

		for(int i = 0 ; i < (1 << (closest_power - 1)) ; ++ i){
			u0.digits.push_back(a.digits[i]);
			u1.digits.push_back(a.digits[i + (1 << (closest_power - 1))]);
			v0.digits.push_back(b.digits[i]);
			v1.digits.push_back(b.digits[i + (1 << (closest_power - 1))]);
		}

		// cout << "- $u_0 = " << u0.print() << "$" <<endl;
		// cout << "- $u_1 = " << u1.print() << "$" <<endl;
		// cout << "- $v_0 = " << v0.print() << "$" <<endl;
		// cout << "- $v_1 = " << v1.print() << "$" <<endl;

		// cout << "Calculamos $m_1 = (u_0 + u_1)\\cdot(v_0 + v_1)$ recursivamente usando karatsuba" << endl;
		big_integer m1 = karatsuba(u0 + u1, v0 + v1);

		// cout << "Volviendo al caso: $a = " << a.print() << ", b = " << b.print() << "$" << endl;
		// cout << "Calculamos $m_2 = u_1 \\cdot v_1$ recursivamente usando karatsuba" << endl;
		big_integer m2 = karatsuba(u1, v1);

		// cout << "Volviendo al caso: $a = " << a.print() << ", b = " << b.print() << "$" << endl;
		// cout << "Calculamos $m_3 = u_0 \\cdot v_0$ recursivamente usando karatsuba" << endl;
		big_integer m3 = karatsuba(u0, v0);

		// cout << "Volviendo al caso: $a = " << a.print() << ", b = " << b.print() << "$" << endl;
		// cout << "- $m_1 = " << m1.print() << ", m_2 = " << m2.print() << ", m_3 = " << m3.print() << "$" << endl;

		big_integer result = m3 + ((m1 - m2 - m3) << (sz / 2)) + (m2 << sz);

		// cout << "- Se suma $" << m3.print() << " + " << ((m1 - m2 - m3) << (sz / 2)).print() << " + " << (m2 << sz).print() << "$\n";

		// cout << "- $a = " << a.print() << "$" << endl;
		// cout << "- $b = " << b.print() << "$" << endl;
		// cout << "- $resultado = " << result.print() << "$" << endl;

		return result;
	}

big_integer read_big_integer(int base){
	if(base <= 10){
		string s;
		cout << "Write down the number: ";
		cin >> s;
		vector<long long>digits;
		bool isneg = false;
		if(s[0] == '-')isneg = true;
		if(s[0] == '-')s.erase(s.begin());
		for(int i = s.size() - 1 ; i >= 0 ; -- i){
			digits.push_back(s[i] - '0');
		}
		return big_integer(isneg, digits, base);
	}else{
		cout << "Write the number of digits: ";
		int d ;
		cin >> d;
		cout << endl;

		cout << "Is the number negative? (Y/N): ";
		bool isneg = false;
		char ans;
		cin >> ans;
		if(ans != 'Y')isneg = false;
		else isneg = true;
		cout << endl;
		cout << "Write down the number, digit by digit, separated by spaces: ";
		vector<long long>digits;
		for(int i = 0 ; i < d ; ++ i){
			long long r;
			cin >> r;
			digits.push_back(r);
		}

		reverse(digits.begin(), digits.end());
		return big_integer(isneg, digits, base);
	}
}