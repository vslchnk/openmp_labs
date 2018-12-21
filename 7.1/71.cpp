#include <stdio.h>
#include <math.h>
#include <omp.h>
#define N 10

int main(int argc, char** argv) {
	double a[N],b[N];
	
	a[0] = 0;
	#pragma parallel for nowait
	for (int i = 1; i < N; i++) {
		a[i] = 2.0 * i;
		b[i] = 2.0 * i + 2.0 * (i - 1);
	}
	b[0]=a[N-1];

  for (int i = 0; i < N; ++i) {
		printf("a[%d] = %f\n", i, a[i]);
		printf("b[%d] = %f\n", i, b[i]);
	}

	return 0;
}
