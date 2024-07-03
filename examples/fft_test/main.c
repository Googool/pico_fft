#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/fft.h"

#define N 8

int main(void) {
  float real[N] = {1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
  float imag[N] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

  printf("Before FFT:\n");
  for (int i = 0; i < N; ++i) {
    printf("real[%d] = %f, imag[%d] = %f\n", i, real[i], i, imag[i]);
  }

  fft(real, imag, N);

  printf("\nAfter FFT:\n");
  for (int i = 0; i < N; ++i) {
    printf("real[%d] = %f, imag[%d] = %f\n", i, real[i], i, imag[i]);
  }

  return 0;
}
