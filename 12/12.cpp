#include <iostream>
#include <math.h>
#include <mpi.h>
#include <vector>
#include <assert.h>

using namespace std;

void decompose_domain(int domain_size, int world_rank, int world_size, int* subdomain_size) {
  *subdomain_size = domain_size / world_size;

  if (world_rank == world_size - 1) {
    *subdomain_size += domain_size % world_size;
  }
}

double q(int x, int i, int r) {
  double qsum;
  int k = (i - 1) * r + 1;
  srand(k);

  for(int j = k; j < k + r - 2; ++j) {
    qsum += (double)(rand() % 100);
  }

  return (qsum + (double)(rand() % 100) * pow((double)x, r));
}

double calculate(int subdomain_size, int world_rank, int world_size, int s, int x, int r) {
  double sum = 0;
  int temp = world_rank * (s / world_size);

  for(int i = temp; i < temp + subdomain_size; i++) {
    //cout << "world rank: " << world_rank << " i: " << i << " temp sum: " << pow((double)x, i * r) * q(x, i + 1, r) << endl;
    sum += pow((double)x, i * r) * q(x, i + 1, r);
  }

  cout << "world rank: " << world_rank << " sum: " << sum << endl;

  return sum;
}

int main(int argc, char *argv[]) {
  int r = atoi(argv[1]);
  int x = atoi(argv[2]);
  int domain_size = (int)pow(2.0, r);

  MPI_Init(NULL, NULL);
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  int subdomain_size;

  decompose_domain(domain_size, world_rank, world_size, &subdomain_size);

  double sum = calculate(subdomain_size, world_rank, world_size, domain_size, x, r);

  int number_amount = 1;
  if (world_rank != 0) {
    MPI_Send(&sum, number_amount, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
  } else {
    for(int i = 1; i < world_size; i++) {
      double sumCp;

      MPI_Recv(&sumCp, number_amount, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

      sum += sumCp;
    }

    cout << "sum all: " << sum << endl;
  }

  MPI_Finalize();

  return 0;
}
