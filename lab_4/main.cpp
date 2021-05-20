#include <iostream>
#include <cmath>
#include <algorithm>
#include <vector>
#include <queue>

using namespace std;

const int P_MIN = 1;
const int P_MAX = 29;
const int W_MIN = 1;
const int W_MAX = 9;
const int D_MIN = 1;
const int D_MAX = 29;
/*
 * klasa generujaca liczby pseudolosowe
 */
class RandomNumberGenerator {
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
 * klasa zawierajaca wartosci problemu WiTi
 */
class witi {
public:
    int id; // ideks instancji
	int p;  // czas realizacji
	int w;  // waga
	int d;	// opoznienie

    int S = 0;	// czas oczekiwania w symulacji
	int C = 0;	// czas realizacji w symulacji
    int T = 0;  // kara
    int wT = 0; // kara * waga
	witi(int i, int a) {
		id = i;
		p = a;

	}
	witi() {
		id = 0;
		p = 0;
		w = 0;
		d = 0;
	}
};

/*
 * funkcja zwracajaca maksimum dwoch liczb
 */
int max(int a, int b) {
	return a > b ? a : b;
}

/*
* funkcja kosztu
*/
int solve_cost(vector<witi>& container) {
	container[0].C = container[0].p;
    if(container[0].C - container[0].w > 0)
	    container[0].T = container[0].C - container[0].d;
    else container[0].T = 0;
    container[0].wT = container[0].w * container[0].T;
	int sum = 0;
	for (unsigned int i = 1; i < container.size(); i++) {
        container[i].C = container[i-1].C + container[i].p;
        container[i].T = container[i].C - container[i].d; 
        if (container[i].T < 0)
            container[i].T = 0;
        container[i].wT = container[i].w * container[i].T;
        sum += container[i].wT;
	}
	return sum;
}

/*
 * funkcja pokazujaca wyniki rpq
 */
 
void show_results(vector<witi> container){
	cout << "nr: [ ";
	for (auto i : container) {
		cout << i.id << " ";
	}
	cout << "]\nC: [ ";
	for (auto i : container) {
		cout << i.C << " ";
	}
	cout << "]\nT: [ ";
	for (auto i : container) {
		cout << i.T << " ";
	}
	cout << "]\nwT: [ ";
	for (auto i : container) {
		cout << i.wT << " ";
	}
	cout << "]\n";
}


/*
 * funkcja pokazujaca wygenerowana instancje rpq
 */
void show_generated(vector<witi> container) {
	cout << "nr: [ ";
	for (auto i : container) {
		cout << i.id << " ";
	}
	cout << "]\np: [ ";
	for (auto i : container) {
		cout << i.p << " ";
	}
	cout << "]\nw: [ ";
	for (auto i : container) {
		cout << i.w << " ";
	}
	cout << "]\nd: [ ";
	for (auto i : container) {
		cout << i.d << " ";
	}
	cout << "]\n\n";
}

vector<witi> generate(RandomNumberGenerator rng, int size) {
    vector<witi> result;
    int A = 0;
    for (int i = 0; i < size; i++) {
		result.emplace_back(i+1, rng.nextInt(P_MIN, P_MAX));
		A += result[i].p;
	}
	for (auto &i : result) {
		i.w = rng.nextInt(W_MIN, W_MAX);
	}
	for (auto &i : result) {
		i.d = rng.nextInt(D_MIN, D_MAX);
	}
    return result;
}

vector<witi> brute_force(vector<witi> container) {
    vector<witi> pi = container;
    sort(container.begin(), container.end(), [](witi& x, witi& y) { return (x.p > y.p); });
    do {
		if (solve_cost(pi) > solve_cost(container)) {
			pi = container;
		}
	} while (next_permutation(container.begin(), container.end(), [](witi& x, witi& y) { return (x.p > y.p); }));
	return pi;
}


int main() {
	int seed, size;
	cout << "Wprowadz ziarno: ";
	cin >> seed;
	cout << "Wprowadz rozmiar problemu: ";
	cin >> size;

	RandomNumberGenerator rng(seed);
	vector<witi> container = generate(rng, size);

	show_generated(container);

    int witi_sum;// = solve_cost(container);

    show_results(container);
    cout << "WiTi sum: " << witi_sum << endl;

    vector<witi> pi = brute_force(container);
    show_results(pi);
    witi_sum = solve_cost(pi);
    cout << "bruteforce sum: " << witi_sum << endl;
/*
	// symulowanie rezulatu
	int Cq_max = simulate(container);
	show_results(container);
	cout << "Cq_max = " << Cq_max << "\n\n";

	// optymalizacja algorytmem Schrage i wyswietlenie rezultatu
	container = schrage_interrupts(container);
	Cq_max = simulate(container);
	show_results(container);
	cout << "Cq_max = " << Cq_max << "\n\n";
    */
	return 0;
}
