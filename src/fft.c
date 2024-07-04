#include "pico/fft.h"

static dma_channel_config cfg;
static uint dma_chan;
static float freqs[NSAMP];

static void calculate_frequencies();
static float calculate_average(uint8_t *buffer, int size);
static void fill_fft_input(uint8_t *buffer, kiss_fft_scalar *fft_in, int size);
static void reset_bins(frequency_bin_t *bins, int bin_count);
static void compute_bin_amplitudes(kiss_fft_cpx *fft_out, frequency_bin_t *bins, int bin_count, int nsamp);

void fft_setup() {
  stdio_init_all();

  adc_gpio_init(26 + CAPTURE_CHANNEL);

  if (!adc_init()) {
    fprintf(stderr, "Failed to initialize ADC\n");
    return;
  }

  adc_select_input(CAPTURE_CHANNEL);
  adc_fifo_setup(
    true,  // Write each completed conversion to the sample FIFO
    true,  // Enable DMA data request (DREQ)
    1,     // DREQ (and IRQ) asserted when at least 1 sample present
    false, // We won't see the ERR bit because of 8 bit reads; disable.
    true   // Shift each sample to 8 bits when pushing to FIFO
  );

  adc_set_clkdiv(CLOCK_DIV);
  sleep_ms(1000);

  dma_chan = dma_claim_unused_channel(true);
  if (dma_chan == -1) {
    fprintf(stderr, "Failed to claim unused DMA channel\n");
    return;
  }

  cfg = dma_channel_get_default_config(dma_chan);
  channel_config_set_transfer_data_size(&cfg, DMA_SIZE_8);
  channel_config_set_read_increment(&cfg, false);
  channel_config_set_write_increment(&cfg, true);
  channel_config_set_dreq(&cfg, DREQ_ADC);

  calculate_frequencies();
}

void fft_sample(uint8_t *capture_buf) {
  adc_fifo_drain();
  adc_run(false);

  dma_channel_configure(dma_chan, &cfg,
    capture_buf,    // dst
    &adc_hw->fifo,  // src
    NSAMP,          // transfer count
    true            // start immediately
  );

  adc_run(true);
  dma_channel_wait_for_finish_blocking(dma_chan);
}

void fft_process(uint8_t *capture_buf, frequency_bin_t *bins, int bin_count) {
  kiss_fft_scalar fft_in[NSAMP];
  kiss_fft_cpx fft_out[NSAMP];
  kiss_fftr_cfg cfg = kiss_fftr_alloc(NSAMP, false, NULL, NULL);

  if (!cfg) {
    fprintf(stderr, "Failed to allocate FFT configuration\n");
    return;
  }

  fill_fft_input(capture_buf, fft_in, NSAMP);
  kiss_fftr(cfg, fft_in, fft_out);
  reset_bins(bins, bin_count);
  compute_bin_amplitudes(fft_out, bins, bin_count, NSAMP);
  kiss_fft_free(cfg);
}

static void calculate_frequencies() {
  float f_max = FSAMP;
  float f_res = f_max / NSAMP;
  for (int i = 0; i < NSAMP; i++) {
    freqs[i] = f_res * i;
  }
}

static float calculate_average(uint8_t *buffer, int size) {
  uint64_t sum = 0;
  for (int i = 0; i < size; i++) {
    sum += buffer[i];
  }
  return (float)sum / size;
}

static void fill_fft_input(uint8_t *buffer, kiss_fft_scalar *fft_in, int size) {
  float avg = calculate_average(buffer, size);
  for (int i = 0; i < size; i++) {
    fft_in[i] = (float)buffer[i] - avg;
  }
}

static void reset_bins(frequency_bin_t *bins, int bin_count) {
  for (int i = 0; i < bin_count; i++) {
    bins[i].amplitude = 0;
  }
}

static void compute_bin_amplitudes(kiss_fft_cpx *fft_out, frequency_bin_t *bins, int bin_count, int nsamp) {
  for (int i = 0; i < nsamp / 2; i++) {
    float power = fft_out[i].r * fft_out[i].r + fft_out[i].i * fft_out[i].i;
    float freq = freqs[i];

    for (int j = 0; j < bin_count; j++) {
      if (freq >= bins[j].freq_min && freq <= bins[j].freq_max) {
        bins[j].amplitude += power;
        break;
      }
    }
  }

  for (int i = 0; i < bin_count; i++) {
    bins[i].amplitude = sqrtf(bins[i].amplitude);
  }
}
