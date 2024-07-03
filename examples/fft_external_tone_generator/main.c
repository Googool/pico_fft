#include "pico/stdlib.h"
#include "pico/fft.h"

/**
 * This example demonstrates the use of the FFT library.
 * It samples data from an ADC pin and performs an FFT on the sampled data.
 */

int main(void) {
  fft_t fft;
  fft_init(&fft, 26, 1024);
  while (true) {
    fft_sample(&fft);
    fft_compute(&fft);
    fft_print(&fft);
    sleep_ms(1000);
  }

  fft_deinit(&fft);
  return 0;
}
