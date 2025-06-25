#include "SSA.cpp"
#include <random>
#include <time.h>
#include <chrono>
#include <iomanip>
void test(){
	int base;
	cout << "Base: ";
	cin >> base ;
	cout << endl;

	cout << "Reading first number (a): " << endl;
	big_integer a = read_big_integer(base);
	cout << "Reading second number (b): " << endl;
	big_integer b = read_big_integer(base);

	char op;
	cout << "Operation: " ;
	cin >> op;
	cout << endl;

	if(op == '+'){
		cout << "a + b = " << (a + b).print() << endl;
	}else if(op == '-'){
		cout << "a - b = " << (a - b).print() << endl;
	}else if(op == '*'){
		cout << "a * b = " << (a * b).print() << endl;
	}else{
		cout << "a / b = " << (a / b).print() << endl; 
	}
}

mt19937 rng(chrono::steady_clock::now().time_since_epoch().count()); //high quality random number generator using time as seed
int random(int l, int r){return uniform_int_distribution<int>(l,r)(rng);} //returns a randomb number between [l, r]

int stress_test(){

    for(long long a = -50 ; a <= 50 ; ++ a){
        for(long long b = -50 ; b <= 50 ; ++ b){
            for(long long base = 2 ; base < 100 ; ++ base){

                big_integer A = convert_base_10_to_x(a, base), B = convert_base_10_to_x(b, base);
                if(a + b != (A + B).to_longlong()){
                    cout << a << " " << b << endl;
                    cout << "+ " << endl;
                    cout << base << endl;
                    return 0;
                }
                if(a - b != (A - B).to_longlong()){
                    cout << a << " " << b << endl;
                    cout << "- " << endl;
                    cout << base << endl;
                    return 0;
                }
                if(a * b != (A * B).to_longlong()){
                    cout << a << " " << b << endl;
                    cout << "* " << endl;
                    cout << base << endl;
                    return 0;
                }
                if(b > 0 && a>= 0&& a % b != (A % B).to_longlong()){
                    cout << a << " " << b << endl;
                    cout << "% " << endl;
                    cout << base << endl;
                    return 0;
                }

                if(bool(a < b) != bool(A < B)){
                    cout << a << " " << b << endl;
                    cout << "<";
                    cout << base << endl;
                    return 0;
                }

                if(bool(a != b) != bool(A != B)){
                    cout << a << " " << b << endl;
                    cout << "!=" << endl;
                    cout << base << endl;
                    return 0;
                }

                if(bool(a == b) != bool (A == B)){
                    cout << a << " " << b << endl;
                    cout << "== " << endl;
                    cout << base << endl;
                    return 0;
                }
            }
        }
    }

	for(long long i = 0 ; i < 1e3 ; ++ i){
		long long a = random(-1e9, 1e9), b = random(-1e9, 1e9);
		long long base = random(2, 1e2);
		big_integer A = convert_base_10_to_x(a, base), B = convert_base_10_to_x(b, base);
		if(a + b != (A + B).to_longlong()){
			cout << a << " " << b << endl;
			return 0;
		}
		if(a - b != (A - B).to_longlong()){
			cout << a << " " << b << endl;
			return 0;
		}
		if(a * b != (A * B).to_longlong()){
			cout << a << " " << b << endl;
			return 0;
		}
		if(b > 0 && a >= 0 &&  a % b != (A % B).to_longlong()){
			cout << a << " " << b << endl;
			return 0;
		}

		if(i % (int)1e5 == 0){
			cout << i / 1e5 << endl;
		}

        if(bool(a < b) != bool(A < B)){
            cout << a << " " << b << endl;
            cout << "<" << endl;
            cout << base << endl;
            return 0;
        }

        if(bool(a != b) != bool(A != B)){
            cout << a << " " << b << endl;
            cout << "!=" << endl;
             cout << base << endl;
             return 0;
        }

        if(bool(a == b) != bool (A == B)){
            cout << a << " " << b << endl;
            cout << "== " << endl;
             cout << base << endl;
             return 0;
        }

	}
	return 1;
}

int stress_test_basis(){
    for(long long i = 0 ; i < 1e3 ; ++ i){
		long long x = random(-1e8, 1e8), y = random(-1e8, 1e8);
		long long base = random(2, 1e2);
        long long base2 = random(2, 1e2);
		big_integer A = convert_base_10_to_x(x, base), B = convert_base_10_to_x(y, base);
        big_integer a = change_basis(A, base2), b = change_basis(B, base2);
		if((a + b).to_longlong() != (A + B).to_longlong()){
			cout << a.print() << " " << b.print() << endl;
            cout << base << " " << base2 << endl;
			return 0;
		}

        if((a - b).to_longlong() != (A - B).to_longlong()){
			cout << a.print() << " " << b.print() << endl;
            cout << base << " " << base2 << endl;
			return 0;
		}

        if((a * b).to_longlong() != (A * B).to_longlong()){
			cout << a.print() << " " << b.print() << endl;
            cout << base << " " << base2 << endl;
			return 0;
		}

        if(y > 0  && (a / b).to_longlong() != (A / B).to_longlong()){
			cout << a.print() << " " << b.print() << endl;
            cout << base << " " << base2 << endl;
			return 0;
		}

		if(y != 0 && x >= 0  && (a % b).to_longlong() != (A % B).to_longlong()){
			cout << a.print() << " " << b.print() << endl;
            cout << base << " " << base2 << endl;
			return 0;
		}

        if((a < b) != (A < B)){
			cout << a.print() << " " << b.print() << endl;
            cout << base << " " << base2 << endl;
			return 0;
		}

        if((a == b) != (A == B)){
			cout << a.print() << " " << b.print() << endl;
            cout << base << " " << base2 << endl;
			return 0;
        }

	}
    return 1;
}

int stress_test_SSA_karatsuba(){
    int N = 10;
    long long base = 256;
    for(int x = 0 ; x < 20 ; ++ x, N *= 2){
        vector<long long>dA, dB;
        
        for(int i = 0 ; i < N ; ++ i){
            dA.push_back(random(1, base - 1));
            dB.push_back(random(1, base - 1));
        }
        big_integer A(dA, false, base), B(dB, false, base);
        cout << N << endl;

        auto start = std::chrono::high_resolution_clock::now();
        big_integer result_karatsuba = A * B;
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start);
        cout << " ! " << fixed << setprecision(7) << duration.count() / 1000.0 << endl;

        start = std::chrono::high_resolution_clock::now();
        big_integer result_SSA = ssa_multiplication(A, B);
        end_time = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start);

        cout << " ! " << fixed << setprecision(7) << duration.count() / 1000.0 << endl;
        

       
        if(result_karatsuba != result_SSA){
            cout << "wrong " << endl;
            cout << A.print() << endl;
            cout << B.print() << endl;

            cout << result_karatsuba.to_longlong() << endl;
            cout << result_SSA.to_longlong() << endl;
            cout << result_karatsuba.print() << endl;
            cout << result_SSA.print() << endl;
            return 0;
        }
    }
    return 1;
}

int stress_test_SSA(){
    int N = 10;
    long long base = 256;
    for(int x = 0 ; x < 20 ; ++ x, N *= 2){
        vector<long long>dA, dB;
        
        for(int i = 0 ; i < N ; ++ i){
            dA.push_back(random(1, base - 1));
            dB.push_back(random(1, base - 1));
        }
        big_integer A(dA, false, base), B(dB, false, base);
        cout << N << endl;

    
        auto start = std::chrono::high_resolution_clock::now();
        big_integer result_SSA = ssa_multiplication(A, B);
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start);

        cout << " ! " << fixed << setprecision(7) << duration.count() / 1000.0 << endl;
  
    }
    return 1;
}