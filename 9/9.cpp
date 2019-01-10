#include <iostream>
#include <math.h>
#include <mpi.h>
#include <vector>
#include <assert.h>

using namespace std;

struct Ticket {
  double amount;
  int type;
  int src;
  int dest;
};


double uToD(double u) {
  return u * 1.5;
}

double fToD(double f) {
  return f * 0.5;
}

void printInfo(double *arr, int n) {
  for(int i = 0; i < n; i++) {
    switch(i) {
      case 0:
        cout << "bank 0 send to bank 1 $: " << arr[i] << endl;
        break;
      case 1:
        cout << "bank 0 send to bank 2 $: " << arr[i] << endl;
        break;
      case 2:
        cout << "bank 1 send to bank 0 $: " << arr[i] << endl;
        break;
      case 3:
        cout << "bank 1 send to bank 2 $: " << arr[i] << endl;
        break;
      case 4:
        cout << "bank 2 send to bank 0 $: " << arr[i] << endl;
        break;
      case 5:
        cout << "bank 2 send to bank 1 $: " << arr[i] << endl;
        break;
    }
  }
}

void print(vector<Ticket> tickets) {
  for(int i = 0; i < tickets.size(); i++) {
    cout << "ticket number: " << i + 1 << endl;
    cout << "amount: " << tickets[i].amount << endl;
    cout << "type: " << tickets[i].type << endl;
    cout << "src: " << tickets[i].src << endl;
    cout << "dest: " << tickets[i].dest << endl;
    cout << "=========" << endl;
  }
}

void generate(vector<Ticket> *tickets, int subdomain_size) {
  for(int i = 0; i < subdomain_size; i++) {
    Ticket ticket;
    ticket.amount = rand() % 10 + 1;
    ticket.type = rand() % 3;
    ticket.src = rand() % 3;

    do {
      ticket.dest = rand() % 3;
    } while (ticket.dest == ticket.src);

    tickets->push_back(ticket);
  }
}

void decompose_domain(int domain_size, int world_rank, int world_size, int* subdomain_size) {
  *subdomain_size = domain_size / world_size;

  if (world_rank == world_size - 1) {
    *subdomain_size += domain_size % world_size;
  }
}

void calculate(vector<Ticket> *tickets, int subdomain_size, double *arr) {
  for(int i = 0; i < subdomain_size; i++) {
    if (tickets->at(i).type == 1) {
      tickets->at(i).amount = uToD(tickets->at(i).amount);
    } else if (tickets->at(i).type == 2) {
      tickets->at(i).amount = fToD(tickets->at(i).amount);
    }

    if (tickets->at(i).src == 0) {
      switch(tickets->at(i).dest) {
        case 1:
        arr[0] += tickets->at(i).amount;
        break;
        case 2:
        arr[1] += tickets->at(i).amount;
        break;
      }
    } else if (tickets->at(i).src == 1) {
      switch(tickets->at(i).dest) {
        case 0:
        arr[2] += tickets->at(i).amount;
        break;
        case 2:
        arr[3] += tickets->at(i).amount;
        break;
      }
    } else {
      switch(tickets->at(i).dest) {
        case 0:
        arr[4] += tickets->at(i).amount;
        break;
        case 1:
        arr[5] += tickets->at(i).amount;
        break;
      }
    }
  }
}

int main(int argc, char *argv[]) {
  int number_amount = 6;
  vector<Ticket> tickets;
  double info[number_amount];

  for(int i = 0; i < 6; ++i) {
    info[i] = 0;
  }

  int domain_size = atoi(argv[1]);

  MPI_Init(NULL, NULL);
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  srand((unsigned)time(NULL) + world_rank);

  int subdomain_size;

  decompose_domain(domain_size, world_rank, world_size, &subdomain_size);

  generate(&tickets, subdomain_size);

  //print(tickets);

  calculate(&tickets, subdomain_size, info);

  tickets.clear();

  /*for(int i = 0; i < number_amount; ++i) {
    cout << info[i] << endl;
  }*/

  if (world_rank != 0) {
    MPI_Send(info, number_amount, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
  } else {
    for(int i = 1; i < world_size; i++) {
      double infoCp[number_amount];
      MPI_Recv(infoCp, number_amount, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

      for(int j = 0; j < number_amount; j++) {
        info[j] += infoCp[j];
      }
    }

    printInfo(info, number_amount);
  }

  MPI_Finalize();

  return 0;
}

// int main(int argc, char *argv[]) {
//   int number_amount = 6;
//   vector<Ticket> tickets;
//   double info[number_amount];
//
//   for(int i = 0; i < 6; ++i) {
//     info[i] = 0;
//   }
//
//   int domain_size = atoi(argv[1]);
//
//   MPI_Init(NULL, NULL);
//   int world_size;
//   MPI_Comm_size(MPI_COMM_WORLD, &world_size);
//   int world_rank;
//   MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
//
//   srand((unsigned)time(NULL) + world_rank);
//
//   int subdomain_size;
//
//   decompose_domain(domain_size, world_rank, world_size, &subdomain_size);
//
//   generate(&tickets, subdomain_size);
//
//   //print(tickets);
//
//   calculate(&tickets, subdomain_size, info);
//
//   tickets.clear();
//
//   // for(int i = 0; i < number_amount; ++i) {
//   //   cout << info[i] << endl;
//   // }
//
//   double sub_infos[number_amount * world_size];
//   // if (world_rank == 0) {
//   //   assert(sub_infos != NULL);
//   // }
//   MPI_Gather(&info, number_amount, MPI_DOUBLE, sub_infos, number_amount, MPI_DOUBLE, 0, MPI_COMM_WORLD);
//   if (world_rank == 0) {
//     double infoCp[number_amount];
//
//     for(int i = 0; i < 6; ++i) {
//       infoCp[i] = 0;
//     }
//
//     for(int i = 0; i < world_size; i++) {
//       for(int j = 0; j < number_amount; j++) {
//         //cout << sub_infos[i * number_amount + j] << endl;
//         infoCp[j] += sub_infos[i * number_amount + j];
//       }
//     }
//
//     printInfo(infoCp, number_amount);
//   }
//
//   MPI_Finalize();
//
//   return 0;
// }
