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

class FSP {
	public:
		int tasks;
		int machines;
		vector<vector<int>>arr;
		int LB;
		int UB;

		FSP(int t, int m) {
			tasks = t;
			machines = m;
			arr.resize(tasks);
			for (int i = 0; i < tasks; i++) {
				arr[i].resize(machines);
			}
		}

		~FSP() { }

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
			FSP c = FSP(tasks, machines);
			c.arr[0][0] = arr[0][0];
			for(int j = 1; j < machines; j++) {
				c.arr[0][j] = c.arr[0][j-1] + arr[0][j];
				//cout << "c.arr[0]["<<j<<"]=" << c.arr[0][j] << "\n";
			}
			for (unsigned int i = 1; i < arr.size(); i++) {
				c.arr[i][0] = c.arr[i-1][0] + arr[i][0];
				for(int j = 1; j < machines; j++) {
					c.arr[i][j] = max(c.arr[i][j-1], c.arr[i-1][j]) + arr[i][j];
				}
			}
			/*
			cout << "C: ";
			c.print();
			*/
			return c.arr[tasks - 1][machines - 1];
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

/*
 * algorytm johnsona dla Flowshopu, dziala optymalnie dla dwoch maszyn,
 * dla wiekszej ilosci moze polepszyc wynik lecz nie ma pewnosci
 */
FSP johnsons_algorithm(FSP N) {
	FSP pi(N.tasks, N.machines);
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
FSP brute_force(FSP N) {
	FSP pi = N;
	sort(N.arr.begin(), N.arr.end());
	do {
		if (pi.solve_cost() > N.solve_cost()) {
			pi = N;
		}
	} while (next_permutation(N.arr.begin(), N.arr.end()));
	return pi;
}

int _LB(int index, FSP pi, FSP N) {
	int LB=0;
	if (index == 0) {
		LB = pi.solve_cost() + N.sum();
	}
	if (index >= 1) {
		for (int i = 0; i < N.machines; i++)
		{
			max(LB, pi.solve_cost()+ N.single_sum(i));
		}
	}
	return LB;
}

void bnb_procedure(int j, FSP &N2, FSP &pi2, FSP &pi3, int &UB) {
	FSP pi = pi2;
	FSP N = N2;
	pi.arr.push_back(N.arr[j]);
	N.arr.erase(N.arr.begin() + j);
	if (!N.arr.empty()) {
		N.LB = _LB(N.arr.size(), pi, N);
		if (N.LB < UB) {
			for (unsigned int i = 0; i < N.arr.size(); i++)
				bnb_procedure(i, N, pi, pi3, UB);
		}
	}
	else {
		int c_max = pi.solve_cost();
		if (c_max < UB) {
			UB = c_max;
			pi3 = pi;
		}
	}
}

FSP bnb_algorithm(FSP N, int _UB) {
	FSP pi(N.tasks, N.machines);
	FSP pi2(N.tasks, N.machines);
	pi.arr.clear();
	int UB = _UB;
	for (unsigned int i = 0; i < N.arr.size(); i++)
		bnb_procedure(i, N, pi, pi2, UB);
	cout << UB;
	return pi2;
}
int main() {
	int seed;
	int size[2];
	cout << "Wprowadz ziarno: ";
	cin >> seed;
	cout << "Wprowadz rozmiar problemu: ";
	cin >> size[0] >> size[1];

	RandomNumberGenerator rng(seed);
	FSP vec(size[0], size[1]);
	vec.generate(rng);
	vec.print();

	cout << "C max: " << vec.solve_cost() << endl;
	
	FSP pi = johnsons_algorithm(vec);
	cout << "Algorytm Johnsona:\n";
	pi.print();
	cout << "C max: " << pi.solve_cost() << endl;

	cout << "Brute force:\n";
	pi = brute_force(vec);
	pi.print();
	cout << "C max: " << pi.solve_cost() << endl;

	cout << "Algorytm BnB:\n";
	pi = bnb_algorithm(vec, pi.solve_cost());
	pi.print();
	cout << "C max: " << pi.solve_cost() << endl;
	return 0;
}
