#include <iostream>
#include <math.h>
#include <omp.h>
#include <vector>
#include <boost/align/aligned_allocator.hpp>

using namespace std;

int numThreads;

int col_max(const vector<vector<double, boost::alignment::aligned_allocator<double, 16>> > &matrix, int col, int n) {
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

int triangulation(vector<vector<double, boost::alignment::aligned_allocator<double, 16>> > &matrix, int n) {
  unsigned int swapCount = 0;

  if (0 == n)
  return swapCount;

  const int num_cols = matrix[0].size();

  for (int i = 0; i < n-1; ++i) {
    unsigned int imax = col_max(matrix, i, n);

    if (i != imax) {
      swap(matrix[i], matrix[imax]);
      ++swapCount;
    }

    #pragma omp parallel for num_threads(numThreads)
    for (int j = i + 1; j < n; ++j) {
      double mul = -matrix[j][i]/matrix[i][i];

      for (int k = i; k < num_cols; ++k) {
        matrix[j][k] += matrix[i][k]*mul;
      }
    }
  }
  return swapCount;
}

vector<double, boost::alignment::aligned_allocator<double, 16>> gauss_solving(vector<vector<double, boost::alignment::aligned_allocator<double, 16>> > &matrix, vector<double, boost::alignment::aligned_allocator<double, 16>> &free_term_column, int n) {
  vector<double, boost::alignment::aligned_allocator<double, 16>> solution(n);

  for (int i = 0; i < n; ++i) {
    matrix[i].push_back(free_term_column[i]);
  }

  triangulation(matrix, n);

  for (int i = n-1; i>=0; --i) {
    solution[i] = matrix[i][n]/matrix[i][i];

    for (int j = 0; j < i; ++j) {
      matrix[j][n] -= matrix[j][i]*solution[i];
    }
  }

  return solution;
}

void random_matrix(int amount, int srandom, int amountThreads) {
  int n = amount;
  srand(srandom);

  numThreads = amountThreads;
  if (numThreads > omp_get_max_threads()) {
    numThreads = omp_get_max_threads();
  }

  vector <vector<double, boost::alignment::aligned_allocator<double, 16>>> matrix (n, vector<double, boost::alignment::aligned_allocator<double, 16>> (n));
  vector<double, boost::alignment::aligned_allocator<double, 16>> free_term_column (n);
  vector<double, boost::alignment::aligned_allocator<double, 16>> solution (n);

  //cout << "Matrix: " << endl;

  for (int i = 0; i < n; i++) {
    free_term_column[i] = rand()%10;
    for (int j = 0 ; j < n; j++) {
      matrix[i][j] = rand()%10;
    //  cout << matrix[i][j] << " ";
    }
    //cout << endl;
  }

  cout << "N and time: " << endl;

  double start_time = omp_get_wtime();

  solution = gauss_solving(matrix, free_term_column, n);

  cout << n << ", " << omp_get_wtime() - start_time << endl;

  //cout << "Solution: " << endl;

  for (int i = 0; i < n; i++) {
    //cout << solution[i] << endl;
  }
}
