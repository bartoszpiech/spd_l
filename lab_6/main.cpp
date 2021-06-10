#include <iostream>
#include <cmath>
#include <algorithm>
#include <vector>
#include <cstring>

using namespace std;

const int P_MIN = 1;
const int P_MAX = 29;

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
 * klasa zawierajaca informacje o instancji dla problemu FP||Cmax
 */
class FP {
	public:
		int tasks;					// liczba zadań
		int machines;				// liczba maszyn
		vector<vector<int>>arr;		// wektor wektorow operacji dla kazdego zadania

		FP(int t, int m) {
			tasks = t;
			machines = m;
			arr.resize(tasks);
			for (int i = 0; i < tasks; i++) {
				arr[i].resize(machines);
			}
		}

		~FP() { }

		void generate(RandomNumberGenerator &rng) {
			for (int i = 0; i < tasks; i++) {
				for (int j = 0; j < machines; j++) {
					arr[i][j] = rng.nextInt(P_MIN, P_MAX);
				}
			}
		}

		void print() {
			cout << "[ ";
			for (int i = 0; i < tasks; i++) {
				cout << "[ ";
				for (int j = 0; j < machines; j++) {
					cout << arr[i][j] << " ";
				}
				cout << "] ";
			}
			cout << "]\n";
		}

		int solve_cost() {
			FP c = FP(arr.size(), machines);
			c.arr[0][0] = arr[0][0];
			for(int j = 1; j < machines; j++) {
				c.arr[0][j] = c.arr[0][j-1] + arr[0][j];
				//cout << "c.arr[0]["<<j<<"]=" << c.arr[0][j] << "\n";
			}
			for (unsigned int i = 1; i < arr.size(); i++) {
				c.arr[i][0] = c.arr[i-1][0] + arr[i][0];
				for(int j = 1; j < machines; j++) {
					c.arr[i][j] = max(c.arr[i][j - 1], c.arr[i - 1][j]) + arr[i][j];
				}
			}
			/*
			cout << "C: ";
			c.print();
			*/
			return c.arr[arr.size() - 1][machines - 1];
		}


		int get_index_of_shortest_execution_time() {
			if (arr.size() < 0) {
				cerr << "Blad, probujesz dostac element z pustej tablicy\n";
				exit(1);
			}
			int result = 0;
			int shortest = arr[0][0];
			for(unsigned int i = 0; i < arr.size(); i++) {
				for(int j = 0; j < machines; j++) {
					if (shortest > arr[i][j]) {
						shortest = arr[i][j];
						result = i;
					}
				}
			}
			return result;
		}

		// zwraca sume dla wszystkich maszyn
		int sum() {
			int result = 0;
			for (unsigned int i = 0; i < arr.size(); i++) {
				for (int j = 0; j < machines; j++) {
					result += arr[i][j];
				}
			}
			return result;
		}

		// zwraca sume dla pojedynczej maszyny
		int single_sum(int index) {
			int result = 0;
			for (unsigned int i = 0; i < arr.size(); i++) {
					result += arr[i][index];
			}
			return result;
		}

};

// zwraca maksimum dwoch liczb
int max(int a, int b) {
	return a > b ? a : b;
}

// zwraca minimum dwoch liczb
int min(int a, int b) {
	return a < b ? a : b;
}

/*
 * algorytm johnsona dla Flowshopu, dziala optymalnie dla dwoch maszyn,
 * dla wiekszej ilosci moze polepszyc wynik lecz nie ma pewnosci
 */
FP johnsons_algorithm(FP N) {
	FP pi(N.tasks, N.machines);
	int l = 0;	// zaczynamy od 0-ego elementu nie od 1 jak w pseudokodzie
	int k = N.tasks - 1;	// tu to samo
	while (!N.arr.empty()) {
		int j = N.get_index_of_shortest_execution_time();
		if (N.arr[j][0] < N.arr[j][N.machines - 1]) {
			pi.arr[l] = N.arr[j];
			l++;
		} else {
			pi.arr[k] = N.arr[j];
			k--;
		}
		N.arr.erase(N.arr.begin() + j);
	}
	return pi;
}

/*
 * algorytm brute force dla Flowshopu, dziala optymalnie ale ma zlozonosc o(n!)
 */
FP brute_force(FP N) {
	FP pi = N;
	sort(N.arr.begin(), N.arr.end());
	do {
		if (pi.solve_cost() > N.solve_cost()) {
			pi = N;
		}
	} while (next_permutation(N.arr.begin(), N.arr.end()));
	return pi;
}

double random_double(double min, double max) {
	double tmp = (double)rand() / RAND_MAX;
	return min + tmp * (max - min);
}
/*
 * algorytm symulowanego wyzarzania, inspirowany przemyslem metalurgicznym,
 * im cieplejszy element, tym jest bardziej plastyczny - w tym przypadku
 * im większa temperatura, tym większa szansa na zmianę wyniku na
 * zaakceptowanie wyniku z gorszym wynikiem
 */
FP simulated_annealing(FP N, int T_0, int T_end, int L) {
	// aktualna temperatura
    int T = T_0;
    // nasze rozwiazanie poczatkowe
	FP pi = N;
    // najlepsze rozwiazanie dotychczas
	FP best_pi = pi;
    // iterator do chlodzenia logarytmicznego
	int it = 0;
	while (T > T_end) {
		for (int k = 1; k < L; k++) {
            // wylosowanie dwoch indeksow do zamiany
			int i = rand() % pi.tasks;
			int j = rand() % pi.tasks;
			while (i == j) {
            	j = rand() % pi.tasks;
			}
			FP new_pi = pi;
			// zamiana elementow
			new_pi.arr[i] = pi.arr[j];
			new_pi.arr[j] = pi.arr[i];
			if (new_pi.solve_cost() > pi.solve_cost()) {
				// wylosowanie losowej liczby od 0 do 1
                double r = random_double(0, 1);
				if (r >= exp((pi.solve_cost() - new_pi.solve_cost()) / T)) {
					new_pi = pi;
				}
			}
			pi = new_pi;
			if (pi.solve_cost() < best_pi.solve_cost()) {
				best_pi = pi;
			}
		}
		T = T - 1;				// liniowe chlodzenie
		//T = 0.5 * T;			// geometryczne chlodzenie

		/*
		T = T / log(it + 1);	// logarytmiczne chlodzenie
		it++;
		*/
	}
	return pi;
}

int main() {
	srand(time(NULL));
	int seed;
	int size[2];
	clock_t start, stop;
	cout << "Wprowadz ziarno: ";
	cin >> seed;
	cout << "Wprowadz rozmiar problemu: ";
	cin >> size[0] >> size[1];

	RandomNumberGenerator rng(seed);
	FP vec(size[0], size[1]);
	vec.generate(rng);
	vec.print();

	cout << "C max: " << vec.solve_cost() << endl;
	
	cout << "Algorytm Johnsona:\n";
	start = clock();
	FP pi = johnsons_algorithm(vec);
	stop = clock();
	cout << (double)(stop - start) / CLOCKS_PER_SEC << "s\n";
	pi.print();
	cout << "C max: " << pi.solve_cost() << endl;

	
	cout << "Brute force:\n";
	start = clock();
	pi = brute_force(vec);
	stop = clock();
	cout << (double)(stop - start) / CLOCKS_PER_SEC << "s\n";
	pi.print();
	cout << "C max: " << pi.solve_cost() << endl;
	

	cout << "Wyżarzanie:\n";
	start = clock();
	pi = simulated_annealing(vec, 100, 0, 100);
	stop = clock();
	cout << (double)(stop - start) / CLOCKS_PER_SEC << "s\n";
	pi.print();
	cout << "C max: " << pi.solve_cost() << endl;
	

	return 0;
}
