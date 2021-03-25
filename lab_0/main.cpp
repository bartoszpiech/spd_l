#include <iostream>
#include <cmath>
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
	int id; // ideks instancji
	int p;  // czas realizacji
	int r;  // czas przygotowania
	
public:
	RPQ(int a, int i) :
		id(i),
		p(a)
	{}
	int S = 0;	// czas oczekiwania w symulacji
	int C = 0;	// czas realizacji w symulacji

	void make_r(int b) { r = b; }
	int _p() { return p; }
	int _r() { return r; }
	int _id() { return id; }
};

/*
 * struktura pomocnicza (komparator)
 */
struct less_than_key{
	inline bool operator() (RPQ& class1, RPQ& class2){
		return (class1._r() < class2._r());
	}
};

/*
 * funkcja zwracajaca maksimum dwoch liczb
 */
int max (int a, int b) {
	return a > b ? a : b;
}

/*
 * symulacja RPQ
 */
void simulate(vector<RPQ>& container){
	container[0].S = container[0]._r();
	container[0].C = container[0]._p() + container[0].S;
	for (unsigned int i = 1; i < container.size(); i++) {
		container[i].S = max(container[i]._r(), container[i - 1].C);
		container[i].C = container[i]._p() + container[i].S;
	}
}

void show_results(vector<RPQ> container){
	cout << "nr: [ ";
	for (auto i : container) {
		cout << i._id() << " ";
	}
	cout << "]\nS: [ ";
	for (auto i : container) {
		cout << i.S << " ";
	}
	cout << "]\nC: [ ";
	for (auto i : container) {
		cout << i.C << " ";
	}
	cout << "]\n\n";
}

void show_generated(vector<RPQ> container) {
	cout << "nr: [ ";
	for (auto i : container) {
		cout << i._id() << " ";
	}
	cout << "]\nr: [ ";
	for (auto i : container) {
		cout << i._r() << " ";
	}
	cout << "]\np: [ ";
	for (auto i : container) {
		cout << i._p() << " ";
	}
	cout << "]\n\n";
}

int main() {
	int _size;
	int _seed;
	cout << "Wprowadz ziarno: ";
	cin >> _seed;
	cout << "Wprowadz rozmiar problemu: ";
	cin >> _size;

	RandomNumberGenerator rng(_seed);
	std::vector<RPQ> container;
	int A = 0;

	// generowanie instancji
	for (int i = 0; i < _size; i++) {
		container.emplace_back(rng.nextInt(P_MIN, P_MAX), i+1);
		A += container[i]._p();
	}
	for (auto &i : container) {
		i.make_r(rng.nextInt(R_MIN, A));
	}
	show_generated(container);

	// symulowanie rezulatu
	simulate(container);
	show_results(container);

	// optymalizacja i ponowne symulowanie rezultatu
	sort(container.begin(), container.end(), less_than_key());
	simulate(container);
	show_results(container);

	return 0;
}
