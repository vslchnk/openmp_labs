#include <iostream>
#include <math.h>
#include <mpi.h>
#include <vector>
#include <assert.h>

using namespace std;

void block(int world_size, int world_rank) {
  int number_amount = 6;
  double info[number_amount];

  for(int i = 0; i < 6; ++i) {
    info[i] = 0;
  }

  for(int i = 0; i < world_size; ++i) {
    if(i != world_rank) {
      cout << "inside 1: " << world_rank << ": " << i << endl;
      MPI_Send(info, number_amount, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
    }
  }

  for(int i = 0; i < world_size; ++i) {
    if(i != world_rank) {
      cout << "inside 2: " << world_rank << ": " << i << endl;
      MPI_Recv(info, number_amount, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
  }
}

void block2(int world_size, int world_rank) {
  int number_amount = 6;
  double info[number_amount];

  for(int i = 0; i < 6; ++i) {
    info[i] = 0;
  }

  int send = 0;
  MPI_Allgather(&send, 1, MPI_DOUBLE, info, 1, MPI_DOUBLE, MPI_COMM_WORLD);
}

int main(int argc, char *argv[]) {
  MPI_Init(NULL, NULL);
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  srand((unsigned)time(NULL) + world_rank);

  int subdomain_size;

  if (world_rank == 0) {
    cout << "entered" << endl;
    std::cin.ignore();
    cout << "exited" << endl;
  }

  block(world_size, world_rank);

  cout << world_rank << endl;

  MPI_Finalize();

  return 0;
}
