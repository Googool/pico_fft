#include "pico/stdlib.h"
#include "pico/fft.h"

frequency_bin_t bins[] = {
  {"Sub-bass", 0, 63, 0},
  {"Bass", 64, 250, 0},
  {"Low Midrange", 251, 500, 0},
  {"Midrange", 501, 2000, 0},
  {"Upper Midrange", 2001, 4000, 0},
  {"Presence", 4001, 6000, 0},
  {"Brilliance", 6001, 20000, 0}
};

#define BIN_COUNT (sizeof(bins) / sizeof(frequency_bin_t))

float freqs[NSAMP];

void calculate_freqs() {
  float f_max = FSAMP / 2.0;
  float f_res = f_max / (NSAMP / 2);
  for (int i = 0; i < NSAMP / 2; i++) {
    freqs[i] = f_res * i;
  }
}

int main() {
  uint8_t capture_buf[NSAMP];
  fft_setup();
  calculate_freqs();

  while (1) {
    fft_sample(capture_buf);
    fft_process(capture_buf, bins, BIN_COUNT);

    for (int i = 0; i < BIN_COUNT; i++) {
      bins[i].amplitude = 0;
    }

    int bin_counts[BIN_COUNT] = {0};

    for (int i = 0; i < NSAMP / 2; i++) {
      float freq = freqs[i];
      float amplitude = capture_buf[i];

      for (int j = 0; j < BIN_COUNT; j++) {
        if (freq >= bins[j].freq_min && freq <= bins[j].freq_max) {
          bins[j].amplitude += amplitude;
          bin_counts[j]++;
          break;
        }
      }
    }

    for (int i = 0; i < BIN_COUNT; i++) {
      if (bin_counts[i] > 0) {
        bins[i].amplitude /= bin_counts[i];
      }
      printf("%s: Amplitude: %f\n", bins[i].name, bins[i].amplitude);
    }

    /* Example output:
    * Sub-bass: Amplitude: 59.000000
    * Bass: Amplitude: 59.500000
    * Low Midrange: Amplitude: 59.666668
    * Midrange: Amplitude: 64.000000
    * Upper Midrange: Amplitude: 69.133331
    * Presence: Amplitude: 66.043480
    * Brilliance: Amplitude: 57.255520
    */

    sleep_ms(1000);
  }

  return 0;
}
