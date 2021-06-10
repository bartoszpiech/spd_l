#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define P_MIN 1
#define P_MAX 29
#define R_MIN 1

/*
 * struktura RandomNumberGenerator
 */
typedef struct {
	long seed;
} rng_t;

/*
 * struktura zawierajaca wartosci problemu jednomaszynowego
 */
typedef struct {
	int id;
	int p;
	int r;
	int S;
	int C;
} RPQ_t;

void rng_init(rng_t* rng, long seed_value) {
	rng->seed = seed_value;
}

int rng_next_int(rng_t* rng, int low, int high) {
	long k;
	double value_0_1;
	long m = 2147483647l, a = 16807l, b = 127773l, c = 2836l;
	k = rng->seed / b;
	rng->seed = a * (rng->seed % b) - k * c;
	if (rng->seed < 0) {
		rng->seed = rng->seed + m;
	}
	value_0_1 = rng->seed;
	value_0_1 /= m;
	return low + (int)floor(value_0_1 * (high - low + 1));
}

float rng_next_float(rng_t* rng, int low, int high) {
	low *= 100000;
	high *= 100000;
	float val = rng_next_int(rng, low, high) / 100000.0;
	return val;
}

void RPQ_print(const RPQ_t* container, int size) {
	printf("nr:[\t");
	for (int i = 0; i < size; i++) {
		printf("%d\t", container[i].id);
	}
	printf("]\nr: [\t");
	for (int i = 0; i < size; i++) {
		printf("%d\t", container[i].r);
	}
	printf("]\np: [\t");
	for (int i = 0; i < size; i++) {
		printf("%d\t", container[i].p);
	}
	printf("]\nS: [\t");
	for (int i = 0; i < size; i++) {
		printf("%d\t", container[i].S);
	}
	printf("]\nC: [\t");
	for (int i = 0; i < size; i++) {
		printf("%d\t", container[i].C);
	}
	printf("]\n");
}

/*
 * helper function
 */
int max(int a, int b) {
	return a > b ? a : b;
}

/*
 * komparator uzyty do qsorta
 */
int RPQ_cmp_less_r(const void* a, const void* b) {
	return (*(RPQ_t*)a).r - (*(RPQ_t*)b).r;
}

/*
 * symulacja problemu jednomaszynowego
 */
void RPQ_simulate(RPQ_t* container, int size) {
	container[0].S = container[0].r;
	container[0].C = container[0].p + container[0].S;
	for (int i = 1; i < size; i++) {
		container[i].S = max(container[i].r, container[i - 1].C);
		container[i].C = container[i].p + container[i].S;
	}
}

int main(int argc, char** argv) {
	int seed, size, A = 0;
	rng_t rng;

	printf("Wprowadz ziarno: ");
	scanf("%d", &seed);
	printf("Wprowadz rozmiar problemu: ");
	scanf("%d", &size);

	RPQ_t* container = (RPQ_t*)malloc(size * sizeof(RPQ_t));
	rng_init(&rng, seed);

	for (int i = 0; i < size; i++) {
		container[i].id = i + 1;
		container[i].p = rng_next_int(&rng, P_MIN, P_MAX);
		A += container[i].p;
	}
	for (int i = 0; i < size; i++) {
		container[i].r = rng_next_int(&rng, R_MIN, A);
	}
	qsort(container, size, sizeof(RPQ_t), RPQ_cmp_less_r);
	RPQ_simulate(container, size);
	RPQ_print(container, size);
	free(container);
	return 0;
}
