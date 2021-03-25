#include <iostream>
#include <cmath>
#include <algorithm>
#include <vector>
#include <queue>

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
	int q;	// czas stygniecia
	
public:
	RPQ(int a, int i) {
		id = i;
		p = a;
	}
	RPQ() {
		id = 0;
		p = 0;
		r = 0;
		q = 0;
	}
	int S = 0;	// czas oczekiwania w symulacji
	int C = 0;	// czas realizacji w symulacji
	int Cq = 0; // czas rezlizacji + czas stygniecia

	void make_r(int b) { r = b; }
	void make_q(int c) { q = c; }
	void make_p(int d) { p = d; }
	int _p() const { return p; }
	int _r() const { return r; }
	int _q() const { return q; }
	int _id() const { return id; }
};

/*
 * struktury pomocnicze (komparatory)
 */

struct less_than_by_R {
	inline bool operator() (const RPQ& class1, const RPQ& class2) {
		return (class1._r() < class2._r());
	}
};

struct less_than_by_Q {
	inline bool operator() (const RPQ& class1, const RPQ& class2) {
		return (class1._q() < class2._q());
	}
};

struct more_than_by_R {
	inline bool operator() (const RPQ& class1, const RPQ& class2) {
		return (class1._r() > class2._r());
	}
};

/*
 * funkcja zwracajaca maksimum dwoch liczb
 */
int max(int a, int b) {
	return a > b ? a : b;
}

/*
* symulacja RPQ
*/
int simulate(vector<RPQ>& container) {
	container[0].S = container[0]._r();
	container[0].C = container[0]._p() + container[0].S;
	container[0].Cq = container[0].C + container[0]._q();
	int cmax = container[0]._q();
	for (unsigned int i = 1; i < container.size(); i++) {
		container[i].S = max(container[i]._r(), container[i - 1].C);
		container[i].C = container[i]._p() + container[i].S;
		container[i].Cq = container[i].C + container[i]._q();
		cmax = max(cmax, container[i].Cq);
	}
	return cmax;
}

/*
 * funkcja pokazujaca wyniki rpq
 */
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
	cout << "]\nCq: [ ";
	for (auto i : container) {
		cout << i.Cq << " ";
	}
	cout << "]\n";
}

/*
 * funkcja pokazujaca wygenerowana instancje rpq
 */
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
	cout << "]\nq: [ ";
	for (auto i : container) {
		cout << i._q() << " ";
	}
	cout << "]\n\n";
}

/*
 * algorytm Schrage z wykorzystaniem kolejki priorytetowej
 */
vector<RPQ> schrage_pq(vector<RPQ> N) {
	vector<RPQ> pi;
	priority_queue<RPQ, vector<RPQ>, less_than_by_Q> G; // kolejka priorytetowa dla Q
	sort(N.begin(), N.end(), [](RPQ& x, RPQ& y) { return (x._r() > y._r()); });
	int t = N.back()._r();
	while (!G.empty() || !N.empty()) {
		while (!N.empty() && N.back()._r() <= t) {
			G.push(N.back());
			N.pop_back();
		}
		if (!G.empty()) {
			pi.push_back(G.top());
			G.pop();
			t += pi.back()._p();
		}
		else t = N.back()._r();
	}
	return pi;
}

/*
 * algorytm Schrage bez wykorzystania kolejki priorytetowej
 */
vector<RPQ> schrage(vector<RPQ> N) {
	vector<RPQ> pi;
	vector<RPQ> G; // wektor dla Q
	sort(N.begin(), N.end(), [](RPQ& x, RPQ& y) { return (x._r() > y._r()); });
	int t = N.back()._r();
	while (!G.empty() || !N.empty()) {
		while (!N.empty() && N.back()._r() <= t) {
			G.emplace_back(N.back());
			N.pop_back();
		}
		if (!G.empty()) {
			sort(G.begin(), G.end(), [](RPQ& x, RPQ& y) { return (x._q() < y._q()); });
			pi.push_back(G.back());
			G.pop_back();
			t += pi.back()._p();
		}
		else t = N.back()._r();
	}
	return pi;
}

/*
 * funkcja pomocnicza zwracajaca minimalna wartosc r z wektora RPQ i usuwa go
 */
RPQ pop_min_r(vector<RPQ> &container) {
	RPQ tmp = container[0];
	int position = 0;
	for (unsigned int i = 0; i < container.size(); i++) {
		if (tmp._r() > container[i]._r()) {
			position = i;
			tmp = container[i];
		}
	}
	container.erase(container.begin() + position);
	return tmp;
}

/*
 * funkcja pomocnicza zwracajaca minimalna wartosc r z wektora RPQ
 */
int get_min_r(vector<RPQ> container) {
	RPQ tmp = container[0];
	for (unsigned int i = 0; i < container.size(); i++) {
		if (tmp._r() > container[i]._r()) {
			tmp = container[i];
		}
	}
	return tmp._r();
}

/*
 * funkcja pomocnicza zwracajaca maksymalna wartosc q z wektora RPQ i usuwa go
 */
RPQ pop_max_q(vector<RPQ> &container) {
	RPQ tmp = container[0];
	int position = 0;
	for (unsigned int i = 0; i < container.size(); i++) {
		if (tmp._q() < container[i]._q()) {
			position = i;
			tmp = container[i];
		}
	}
	container.erase(container.begin() + position);
	return tmp;
}

/*
 * funkcja pomocnicza zwracajaca minimalna wartosc r z wektora RPQ bez usuwania
 */
int get_max_q(vector<RPQ> &container) {
	RPQ tmp = container[0];
	for (unsigned int i = 0; i < container.size(); i++) {
		if (tmp._q() < container[i]._q()) {
			tmp = container[i];
		}
	}
	return tmp._q();
}

/*
 * algorytm Schrage z przerwaniami bez uzycia kolejek
 */
vector<RPQ> schrage_interrupts(vector<RPQ> N) {
	int k = 1;
	RPQ j, actual;
	vector<RPQ> pi;
	vector<RPQ> G;
	int t = get_min_r(N);
	int C_max = t;
	while (!G.empty() || !N.empty()) {
		while (!N.empty() && get_min_r(N) <= t) {
			j = pop_min_r(N);	// usuwa tez z N
			G.push_back(j);
			if (j._q() > actual._q()) {
				actual.make_p(t - j._r());
				t = j._r();
				if (actual._p() > 0) {
					G.push_back(actual);
				}
			}
		}
		if (!G.empty()) {
			j = pop_max_q(G);
			actual = j;
			pi.push_back(j);
			t += j._p();
			C_max = max(C_max, t+j._q());
			k++;
		} else {
			t = get_min_r(N);
		}
	}
	cout << "Cmax :" << C_max << endl;
	return pi;
}

int main() {
	int seed, size;
	cout << "Wprowadz ziarno: ";
	cin >> seed;
	cout << "Wprowadz rozmiar problemu: ";
	cin >> size;

	RandomNumberGenerator rng(seed);
	std::vector<RPQ> container;
	int A = 0;

	// generowanie instancji
	for (int i = 0; i < size; i++) {
		container.emplace_back(rng.nextInt(P_MIN, P_MAX), i+1);
		A += container[i]._p();
	}
	for (auto &i : container) {
		i.make_r(rng.nextInt(R_MIN, A));
	}
	for (auto &i : container) {
		i.make_q(rng.nextInt(R_MIN, A));
	}
	show_generated(container);

	// symulowanie rezulatu
	int Cq_max = simulate(container);
	show_results(container);
	cout << "Cq_max = " << Cq_max << "\n\n";

	// optymalizacja algorytmem Schrage i wyswietlenie rezultatu
	container = schrage_interrupts(container);
	Cq_max = simulate(container);
	show_results(container);
	cout << "Cq_max = " << Cq_max << "\n\n";
	return 0;
}
