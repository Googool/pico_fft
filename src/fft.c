#include "pico/fft.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void fft_init(fft_t* fft, uint adc_pin, size_t fft_size) {
  fft->adc_pin = adc_pin;
  fft->fft_size = fft_size;
  fft->dma_chan = -1;
  fft->fft_out = (kiss_fft_cpx*)malloc(sizeof(kiss_fft_cpx) * fft_size);
  fft->adc_buffer = (int16_t*)malloc(sizeof(int16_t) * fft_size);
  fft->cfg = kiss_fftr_alloc(fft_size, 0, NULL, NULL);

  stdio_init_all();
  adc_init();
  adc_gpio_init(adc_pin);
  adc_select_input(0);

  fft->dma_chan = dma_claim_unused_channel(true);
  dma_channel_config c = dma_channel_get_default_config(fft->dma_chan);
  channel_config_set_transfer_data_size(&c, DMA_SIZE_16);
  channel_config_set_read_increment(&c, false);
  channel_config_set_write_increment(&c, true);
  channel_config_set_dreq(&c, DREQ_ADC);

  dma_channel_configure(
    fft->dma_chan,
    &c,
    fft->adc_buffer,
    &adc_hw->fifo,
    fft_size,
    true
  );

  adc_fifo_setup(
    true,
    true,
    1,
    false,
    true
  );

  adc_run(true);
}

void fft_deinit(fft_t* fft) {
  if (fft->cfg) free(fft->cfg);
  if (fft->fft_out) free(fft->fft_out);
  if (fft->adc_buffer) free(fft->adc_buffer);
}

void fft_sample(fft_t* fft) {
  dma_channel_wait_for_finish_blocking(fft->dma_chan);
  dma_channel_set_read_addr(fft->dma_chan, &adc_hw->fifo, true);
}

void fft_compute(fft_t* fft) {
  kiss_fftr(fft->cfg, (kiss_fft_scalar*)fft->adc_buffer, fft->fft_out);
}

void fft_print(const fft_t* fft) {
  for (size_t i = 0; i < fft->fft_size / 2; ++i) {
    float magnitude = sqrt(fft->fft_out[i].r * fft->fft_out[i].r + fft->fft_out[i].i * fft->fft_out[i].i);
    printf("Frequency bin %zu: %f\n", i, magnitude);
  }
}
