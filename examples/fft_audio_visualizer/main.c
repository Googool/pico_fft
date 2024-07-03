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

int main() {
    uint8_t cap_buf[NSAMP];
    fft_setup();

    while (1) {
        fft_sample(cap_buf);
        fft_process(cap_buf, bins, sizeof(bins) / sizeof(frequency_bin_t));
        for (int i = 0; i < sizeof(bins) / sizeof(frequency_bin_t); i++) {
            printf("%s: Amplitude: %f\n", bins[i].name, bins[i].amplitude);
        }

        /* Example:
        * Sub-bass: Amplitude: 235.854950
        * Bass: Amplitude: 24.894323
        * Low Midrange: Amplitude: 23.291584
        * Midrange: Amplitude: 56.030037
        * Upper Midrange: Amplitude: 71.086266
        * Presence: Amplitude: 64.704025
        * Brilliance: Amplitude: 212.929337
        */

        sleep_ms(1000);
    }

    return 0;
}
