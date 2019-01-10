#include <iostream>
#include <math.h>
#include <omp.h>
#include <vector>

using namespace std;

int numThreads;

int col_max(const vector<vector<double> > &matrix, int col, int n) {
  double max = abs(matrix[col][col]);
  int maxPos = col;
  for (int i = col+1; i < n; i++) {
    double element = std::abs(matrix[i][col]);
    if (element > max) {
      max = element;
      maxPos = i;
    }
  }
  return maxPos;
}

int triangulation(vector<vector<double> > &matrix, int n) {
  unsigned int swapCount = 0;
  for (int i = 0; i < n-1; i++) {
    unsigned int imax = col_max(matrix, i, n);
    if (i != imax) {
      swap(matrix[i], matrix[imax]);
      ++swapCount;
    }
    #pragma omp parallel for num_threads(numThreads)
    for (int j = i + 1; j < n; j++) {
      printf("Нить %d выполнила итерацию %d\n", omp_get_thread_num(), j);
      double mul = -matrix[j][i]/matrix[i][i];
      for (int k = i; k < n; k++) {
        matrix[j][k] += matrix[i][k]*mul;
      }
    }
  }
  return swapCount;
}

double gauss_determinant(vector<vector<double> > &matrix, int n) {
  unsigned int swapCount = triangulation(matrix, n);
  double determinant = 1;
  if (swapCount % 2 == 1)
    determinant = -1;
  for (int i = 0; i < n; i++) {
    determinant *= matrix[i][i];
  }
  return determinant;
}

int main(int argc, char *argv[]) {
  int n = stoi(argv[1]);
  srand(stoi(argv[2]));

  if (argc == 4) {
    numThreads = stoi(argv[3]);
    if (numThreads > omp_get_max_threads()) {
      numThreads = omp_get_max_threads();
    }
  } else {
    numThreads = omp_get_max_threads();
  }

  vector <vector<double>> matrix (n, vector<double> (n));

  for (int i = 0; i < n; i++) {
    for (int j = 0 ; j < n; j++) {
      matrix[i][j] = rand()%10;
      cout << matrix[i][j] << " ";
    }
    cout << endl;
  }

  cout << gauss_determinant(matrix, n) << endl;

  return 0;
}
