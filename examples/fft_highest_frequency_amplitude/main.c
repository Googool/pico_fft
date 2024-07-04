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

void detect_highest_amplitude_tone(frequency_bin_t *bins, int bin_count) {
  int max_index = 0;
  float max_amplitude = bins[0].amplitude;

  for (int i = 1; i < bin_count; i++) {
    if (bins[i].amplitude > max_amplitude) {
      max_amplitude = bins[i].amplitude;
      max_index = i;
    }
  }

  printf("%s had the highest tone with an amplitude of: %f\n", bins[max_index].name, bins[max_index].amplitude);
  /* Example: "Midrange had the highest tone with an amplitude of: 150.324234" */
}

int main() {
  uint8_t cap_buf[NSAMP];
  fft_setup();

  while (1) {
    fft_sample(cap_buf);
    fft_process(cap_buf, bins, sizeof(bins) / sizeof(frequency_bin_t));
    detect_highest_amplitude_tone(bins, sizeof(bins) / sizeof(frequency_bin_t));

    sleep_ms(1000);
  }

  return 0;
}
