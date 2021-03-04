#include <cmath>
#include <iostream>
#include <algorithm>
#include <vector>

using namespace std;

const int P_MIN = 1;
const int P_MAX = 29;
const int R_MIN = 1;
/*
 * klasa generujaca liczby pseudolosowe
 */
class RandomNumberGenerator
{
private:

	long seed;

public:
	RandomNumberGenerator(long seedValue) :
		seed(seedValue)
	{}
	int nextInt(int low, int high) {
		long k;
		double value_0_1;
		long m = 2147483647l, a = 16807l, b = 127773l, c = 2836l;

		k = seed / b;
		seed = a * (seed % b) - k * c;
		if (seed < 0)
			seed = seed + m;
		value_0_1 = seed;
		value_0_1 /= m;
		return low + (int)floor(value_0_1 * (high - low + 1));
	}
	float nextFloat(int low, int high) {
		low *= 100000;
		high *= 100000;
		float val = nextInt(low, high) / 100000.0;
		return val;
	}
};

/*
 * klasa zawierajaca wartosci problemu jednomaszynowego
 */
class RPQ {
private:

    vector<int> p;  // czas realizacji
    vector<int> r;  // czas przygotowania

public:
    RPQ(int amount, RandomNumberGenerator rng) {
        for (int i = 0; i < amount; i++) {
            p.push_back(rng.nextInt(P_MIN, P_MAX));
        }
        int A = 0;  // wartosc max losowania czasu przygotowania
        for (auto i : p) {
            A += i;
        }
        for (int i = 0; i < amount; i++) {
            r.push_back(rng.nextInt(R_MIN, A));
        }
    }
    
    void print() {
        cout << "[" << endl << "r:\t";
        for (auto i : r) {
            cout << i << " ";
        }
        cout << "\np:\t";
        for (auto i : p) {
            cout << i << " ";
        }
        cout << "\n]\n";
    }
};
int main () {
    RandomNumberGenerator rng(1);

    RPQ a(10, rng);
    a.print();
    return 0;
}
