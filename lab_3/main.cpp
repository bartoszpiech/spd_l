#include <iostream>
#include <cmath>
#include <algorithm>
#include <vector>

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
		int **arr;

		FSP(int t, int m) {
			tasks = t;
			machines = m;
			arr = new int*[tasks];
			for (int i = 0; i < tasks; i++) {
				arr[i] = new int[machines];
			}
		}
		~FSP() {
			for (int i = 0; i < tasks; i++) {
				delete(arr[i]);
			}
			delete(arr);
		}
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
			FSP c = FSP(this->tasks, this->machines);
			c.arr[0][0] = this->arr[0][0];
			for(int j = 1; j < this->machines; j++) {
				c.arr[0][j] = c.arr[0][j-1] + this->arr[1][j-1];
			}
			for (int i = 1; i < this->tasks; i++) {
				c.arr[i][0] = c.arr[i-1][0] + this->arr[i][0];
				for(int j = 1; j < this->machines; j++) {
					c.arr[i][j] = max(c.arr[i][j-1], c.arr[i-1][j]) + this->arr[i][j];
				}
			}
			cout << "C: ";
			c.print();
			return c.arr[this->tasks - 1][this->machines - 1];
		}

};

void johnsons_algorithm(FSP N) {
	int l = 1;
	int k = N.tasks;
	

}

int max(int a, int b) {
	return a > b ? a : b;
}

int main() {
	int seed;
	int size[2];
	cout << "Wprowadz ziarno: ";
	cin >> seed;
	cout << "Wprowadz rozmiar problemu: ";
	cin >> size[0] >> size[1];

	RandomNumberGenerator rng(seed);

	FSP p = FSP(size[0], size[1]);

	p.generate(rng);
	p.print();

	cout << "c max: " << p.solve_cost() << endl;

	return 0;
}
