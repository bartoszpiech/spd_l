#include <iostream>
#include <cmath>
#include <algorithm>
#include <vector>
#include <queue>

using namespace std;

const int P_MIN = 1;
const int P_MAX = 29;
const int R_MIN = 1;

int _size = 6;
int _seed = 1;

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
 * struktura pomocnicza (komparator)
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

void show_results(vector<RPQ> container){
	cout << "nr: [ ";
	for (int i = 0; i < _size; i++)
		cout << container[i]._id() << " ";
	cout << "]\nS: [ ";
	for (int i = 0; i < _size; i++)
		cout << container[i].S << " ";
	cout << "]\nC: [ ";
	for (int i = 0; i < _size; i++)
		cout << container[i].C << " ";
	cout << "]\nCq: [ ";
	for (int i = 0; i < _size; i++)
		cout << container[i].Cq << " ";
	cout << "]\n";
}

void show_generated(vector<RPQ> container) {
	cout << "nr: [ ";
	for (int i = 0; i < _size; i++)
		cout << container[i]._id() << " ";
	cout << "]\nr: [ ";
	for (int i = 0; i < _size; i++)
		cout << container[i]._r() << " ";
	cout << "]\np: [ ";
	for (int i = 0; i < _size; i++)
		cout << container[i]._p() << " ";
	cout << "]\nq: [ ";
	for (int i = 0; i < _size; i++)
		cout << container[i]._q() << " ";
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
 * algorytm Schrage z przerwaniami
 */
 /*
vector<RPQ> schrage_interrupts(vector<RPQ> N) {
	vector<RPQ> pi;
	priority_queue<RPQ, vector<RPQ>, less_than_by_Q> G; // kolejka priorytetowa dla Q
	sort(N.begin(), N.end(), [](RPQ& x, RPQ& y) { return (x._r() > y._r()); });
    
	int t = N.back()._r();
    int C_max = t;
	while (!G.empty() || !N.empty()) {
		while (!N.empty() && N.back()._r() <= t) {
			G.push(N.back());
			N.pop_back();
		}
		if (!G.empty()) {
			pi.push_back(G.top());
			t += pi.back()._p();
            int tmp = C_max;
            C_max = max(C_max, t+G.top()._q());
            if (tmp != C_max) {
                int r = t - N.back()._r();
                G.top().S = 1;
                
            }
     
            G.pop();
		}
		else t = N.back()._r();
	}
	return pi;
}
*/
vector<RPQ> schrage_interrupts(vector<RPQ> N) {
	vector<RPQ> pi;
	vector<RPQ> G; // wektor dla Q
	sort(N.begin(), N.end(), [](RPQ& x, RPQ& y) { return (x._r() > y._r()); });
	int t = N.back()._r();
    int C_max = t;
	while (!G.empty() || !N.empty()) {
		while (!N.empty() && N.back()._r() <= t) {
			G.emplace_back(N.back());
			N.pop_back();
		}
		if (!G.empty()) {
            sort(G.begin(), G.end(), [](RPQ& x, RPQ& y) { return (x._q() < y._q()); });
			pi.push_back(G.back());
            t += pi.back()._p();
            int tmp = C_max;
            
            C_max = max(C_max, t+G.front()._q());
            if (tmp != C_max ) {
                int r = t - N.back()._r();
                G.back().make_p(r);
                
            } 
			G.pop_back();
            
			
		}
		else t = N.back()._r();
	}
	return pi;
}

int main() {
	cout << "Wprowadz ziarno: ";
	cin >> _seed;
	cout << "Wprowadz rozmiar problemu: ";
	cin >> _size;

	RandomNumberGenerator rng(_seed);
	std::vector<RPQ> container;
	int A = 0;

	//******************generowanie instancji******************
	for (int i = 0; i < _size; i++) {
		container.emplace_back(rng.nextInt(P_MIN, P_MAX), i+1);
		A += container[i]._p();
	}
	for (int i = 0; i < _size; i++)
		container[i].make_r(rng.nextInt(R_MIN, A));
	for (int i = 0; i < _size; i++)
		container[i].make_q(rng.nextInt(R_MIN, A));
	show_generated(container);

	//******************symulowanie rezulatu*******************
	int Cq_max = simulate(container);
	show_results(container);
    cout << "Cq_max = " << Cq_max << "\n\n";

	//******optymalizacja algorytmem Schrage i ponowne symulowanie rezultatu******
	container = schrage_interrupts(container);
	Cq_max = simulate(container);
	show_results(container);
    cout << "Cq_max = " << Cq_max;


	return 0;
}
