#include <stdio.h>
#include <stdlib.h>
#ifndef W
#define W 20                                    // Width
#endif
int main(int argc, char **argv) {
  int L = atoi(argv[1]);                        // Length
  int iteration = atoi(argv[2]);                // Iteration
  srand(atoi(argv[3]));                         // Seed
  float d = (float) random() / RAND_MAX * 0.2;  // Diffusivity
  int *temp = malloc(L*W*sizeof(int));          // Current temperature
  int *next = malloc(L*W*sizeof(int));          // Next time step

  for (int i = 0; i < L; i++) {
    for (int j = 0; j < W; j++) {
      temp[i*W+j] = random()>>3;
    }
  }
  int count = 0, balance = 0;
  while (iteration--) {     // Compute with up, left, right, down points
    balance = 1;
    count++;
    for (int i = 0; i < L; i++) {
      for (int j = 0; j < W; j++) {
        float t = temp[i*W+j] / d;
        t += temp[i*W+j] * -4;
        t += temp[(i - 1 <  0 ? 0 : i - 1) * W + j];
        t += temp[(i + 1 >= L ? i : i + 1)*W+j];
        t += temp[i*W+(j - 1 <  0 ? 0 : j - 1)];
        t += temp[i*W+(j + 1 >= W ? j : j + 1)];
        t *= d;
        next[i*W+j] = t ;
        if (next[i*W+j] != temp[i*W+j]) {
          balance = 0;
        }
      }
    }
    if (balance) {
      break;
    }
    int *tmp = temp;
    temp = next;
    next = tmp;
  }
  int min = temp[0];
  for (int i = 0; i < L; i++) {
    for (int j = 0; j < W; j++) {
      if (temp[i*W+j] < min) {
        min = temp[i*W+j];
      }
    }
  }
  printf("Size: %d*%d, Iteration: %d, Min Temp: %d\n", L, W, count, min);
  return 0;
}
