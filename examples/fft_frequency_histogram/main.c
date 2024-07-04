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

void print_frequency_histogram(frequency_bin_t *bins, int bin_count) {
  for (int i = 0; i < bin_count; i++) {
    printf("%s: ", bins[i].name);
    int bar_length = (int)(bins[i].amplitude / 10);
    for (int j = 0; j < bar_length; j++) {
      printf("#");
    }
    printf("\n");
  }

  /* Example output:
  * Sub-bass: ###
  * Bass: #####
  * Low Midrange: ####
  * Midrange: ########
  * Upper Midrange: ######
  * Presence: ######
  * Brilliance: ####
  */
}

int main() {
  uint8_t cap_buf[NSAMP];
  fft_setup();

  while (1) {
    fft_sample(cap_buf);
    fft_process(cap_buf, bins, sizeof(bins) / sizeof(frequency_bin_t));
    print_frequency_histogram(bins, sizeof(bins) / sizeof(frequency_bin_t));

    sleep_ms(1000);
  }

  return 0;
}
