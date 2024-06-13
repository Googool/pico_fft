#include "pico/fft.h"
#include <math.h>

#define PI 3.14159265358979323846

static void fft_rec(float *real, float *imag, uint16_t n, uint16_t step) {
  if (step < n) {
    fft_rec(real, imag, n, step * 2);
    fft_rec(real + step, imag + step, n, step * 2);

    for (uint16_t i = 0; i < n; i += 2 * step) {
      float t_real = cos(-PI * i / n) * real[i + step] - sin(-PI * i / n) * imag[i + step];
      float t_imag = sin(-PI * i / n) * real[i + step] + cos(-PI * i / n) * imag[i + step];
      real[i + step] = real[i] - t_real;
      imag[i + step] = imag[i] - t_imag;
      real[i] = real[i] + t_real;
      imag[i] = imag[i] + t_imag;
    }
  }
}

void fft(float *real, float *imag, uint16_t n) {
  uint16_t j = 0;
  for (uint16_t i = 0; i < n; ++i) {
    if (i < j) {
      float temp_real = real[i];
      float temp_imag = imag[i];
      real[i] = real[j];
      imag[i] = imag[j];
      real[j] = temp_real;
      imag[j] = temp_imag;
    }

    uint16_t m = n >> 1;
    while (j >= m && m >= 2) {
      j -= m;
      m >>= 1;
    }
    j += m;
  }

  fft_rec(real, imag, n, 1);
}
