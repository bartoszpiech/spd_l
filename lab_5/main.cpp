#include <iostream>
#include <cmath>
#include <algorithm>
#include <vector>
#include <cstring>

using namespace std;

const int P_MIN = 1;
const int P_MAX = 29;

vector<int> tablica_id;
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
		for (unsigned int i = 0; i < arr.size(); i++) {
			cout << "[ ";
			for (int j = 0; j < machines; j++) {
				cout << arr[i][j] << " ";
			}
			cout << "] ";
		}
		cout << "]\n";
	}

	int solve_cost(bool show) {
		FSP c = FSP(arr.size(), machines);
		c.arr[0][0] = arr[0][0];
		for (int j = 1; j < machines; j++) {
			c.arr[0][j] = c.arr[0][j - 1] + arr[0][j];
		}
		for (unsigned int i = 1; i < arr.size(); i++) {
			c.arr[i][0] = c.arr[i - 1][0] + arr[i][0];
			for (int j = 1; j < machines; j++) {
				c.arr[i][j] = max(c.arr[i][j - 1], c.arr[i - 1][j]) + arr[i][j];
			}
		}
		if (show) {
			cout << "\nC: ";
			c.print();
		}
		return c.arr[arr.size() - 1][machines - 1];
	}

	int single_solve_cost(int i) {
		int sum = 0;
		for (int j = 0; j < machines; j++) {
			sum += arr[i][j];
		}
		return sum;
	}

	int get_index_of_shortest_execution_time() {
		if (arr.size() < 0) {
			cerr << "Blad, probujesz dostac element z pustej tablicy\n";
			exit(1);
		}
		int result = 0;
		int shortest = arr[0][0];
		for (unsigned int i = 0; i < arr.size(); i++) {
			for (int j = 0; j < machines; j++) {
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
FSP johnsons_algorithm(FSP N) {
	FSP pi(N.tasks, N.machines);
	int l = 0;	// zaczynamy od 0-ego elementu nie od 1 jak w pseudokodzie
	int k = N.tasks - 1;	// tu to samo
	while (!N.arr.empty()) {
		int j = N.get_index_of_shortest_execution_time();
		if (N.arr[j][0] < N.arr[j][N.machines - 1]) {
			pi.arr[l] = N.arr[j];
			l++;
		}
		else {
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
		if (pi.solve_cost(false) > N.solve_cost(false)) {
			pi = N;
		}
	} while (next_permutation(N.arr.begin(), N.arr.end()));
	return pi;
}


/*
 * funkcja pomocnicza NEH szuka najbardziej korzystnego indeksu
 * korzystny indeks = najwieksza zlozonosc czasowa wiec szukamy max
 */
int help_NEH(FSP &N, FSP &pi, int iteration) {
	int current_best = -1;
	int best_index;
	int solved = 0;
	if (iteration != 0)
		for (int i = 0; i < iteration; i++) {
			solved += pi.single_solve_cost(i);
		}
	for (unsigned int i = 0; i < N.arr.size(); i++) {
		if (current_best < N.single_solve_cost(i) + solved) {
			current_best = N.single_solve_cost(i) + solved;
			best_index = i;
		}
	}
	return best_index;
}

/*
 * funkcja pomocnicza NEH, oblicza zlozonosc czasową tymczasowego rozwiązania
 * najpierw wstawia nową wartość w odpowiednim miejscu
 */
int solve_NEH(FSP &pi, vector<int> N_arr, int j) {
	pi.arr.insert(pi.arr.begin() + j, N_arr);
	return pi.solve_cost(false);
}

/*
 * Algorytm NEH
 */
FSP NEH(FSP N) {
	FSP pi(0, N.machines);
	FSP temp(0, N.machines);
	int index;
	int current;
	int _min = 999999;

	for (int i = 0; i < N.tasks; i++) {
		temp = pi;
		index = help_NEH(N, temp, i);
		cout << "-------------------" << endl << index + 1 << " size "<< N.arr.size()  << endl;
		if (i == 0) {
			temp.arr.insert(temp.arr.begin(), N.arr[index]);
			pi = temp;
		}
		else{
			for (unsigned int j = 0; j <= temp.arr.size(); j++) {
				current = solve_NEH(temp, N.arr[index], j);
				//_min = min(_min, current);
				//if (_min == current) 
				//	pi = temp;
				if (current < _min){
					_min = current;
					pi = temp;
				}
				cout << temp.solve_cost(true);
				temp.arr.erase(temp.arr.begin() + j);
			}
		}
		N.arr.erase(N.arr.begin() + index);
		_min = 999999;
		//cout << pi.solve_cost(true);
	}
	return pi;
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

	vec.single_solve_cost(0);
	cout << "C max: " << vec.solve_cost(true) << endl;


	FSP pi = johnsons_algorithm(vec);
	cout << "Algorytm Johnsona:\n";
	pi.print();
	cout << "C max: " << pi.solve_cost(true) << endl;

	/*
	for (int i = 0; i < size[0]; i++) {
		cout << "task " << i << ": " << pi.single_solve_cost(i) << endl;
	}
	*/

	pi = NEH(vec);
	cout << "Algorytm NEH:\n";
	cout << "C max: " << pi.solve_cost(true) << endl;

	/*
	for (int i = 0; i < size[0]; i++) {
		cout << "task " << i << ": " << pi.single_solve_cost(i) << endl;
	}
	*/
	
	cout << "Brute force:\n";
	pi = brute_force(vec);
	pi.print();
	cout << "C max: " << pi.solve_cost(false) << endl;
	
	return 0;
}
