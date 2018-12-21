#include <stdio.h>
#include <string>
#include <iostream>
#include <omp.h>

using namespace std;

int main(int argc, char *argv[]) {
  if (argc == 3) {
    char a[] = { 'a', 'b', 'c', 'd', 'e', 'f'};
    char b[] = { 'd', 'e', 'f', 'g', 'h', 'i'};

    string input = argv[1];
    string inputCopy = input;
    int len = input.length();
    int num = stoi(argv[2]);
    int chunk =  1;

    if (num > omp_get_max_threads()) {
      num = omp_get_max_threads();
    }

    #pragma omp parallel for  num_threads(num) schedule(static, chunk)
    for (int i = 0; i < 6; i++) {
      printf("Нить %d выполнила итерацию %c\n", omp_get_thread_num(), a[i]);

      for (int j = 0; j < len; j++) {
        if (input[j] == a[i]) {
          inputCopy[j] = b[i];
        }
      }
    }
    std::cout << inputCopy << "\n";

    return 0;
  }

  return 0;
}
