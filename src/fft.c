#include "pico/fft.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void fft_init(fft_t* fft, uint adc_pin, size_t fft_size, float sample_rate) {
    fft->adc_pin = adc_pin;
    fft->fft_size = fft_size;
    fft->dma_chan = -1;
    fft->fft_out = (kiss_fft_cpx*)malloc(sizeof(kiss_fft_cpx) * fft_size);
    fft->adc_buffer = (uint8_t*)malloc(sizeof(uint8_t) * fft_size);
    fft->cfg = kiss_fftr_alloc(fft_size, 0, NULL, NULL);
    fft->freqs = (float*)malloc(sizeof(float) * fft_size);

    adc_init();
    adc_gpio_init(adc_pin);
    adc_select_input(adc_pin - 26); // Assuming ADC pin is between 26 and 29

    adc_fifo_setup(
        true,    // Write each completed conversion to the sample FIFO
        true,    // Enable DMA data request (DREQ)
        1,       // DREQ (and IRQ) asserted when at least 1 sample present
        false,   // We won't see the ERR bit because of 8-bit reads; disable.
        true     // Shift each sample to 8 bits when pushing to FIFO
    );

    adc_set_clkdiv(48000000 / sample_rate);

    fft->dma_chan = dma_claim_unused_channel(true);
    dma_channel_config c = dma_channel_get_default_config(fft->dma_chan);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
    channel_config_set_read_increment(&c, false);
    channel_config_set_write_increment(&c, true);
    channel_config_set_dreq(&c, DREQ_ADC);

    dma_channel_configure(
        fft->dma_chan,
        &c,
        fft->adc_buffer,    // dst
        &adc_hw->fifo,      // src
        fft_size,           // transfer count
        true                // start immediately
    );

    float f_max = sample_rate;
    float f_res = f_max / fft_size;
    for (size_t i = 0; i < fft_size; i++) {
        fft->freqs[i] = f_res * i;
    }
}

void fft_deinit(fft_t* fft) {
    if (fft->cfg) free(fft->cfg);
    if (fft->fft_out) free(fft->fft_out);
    if (fft->adc_buffer) free(fft->adc_buffer);
    if (fft->freqs) free(fft->freqs);
}

void fft_sample(fft_t* fft) {
    adc_fifo_drain();
    adc_run(false);

    dma_channel_set_read_addr(fft->dma_chan, &adc_hw->fifo, true);
    adc_run(true);
    dma_channel_wait_for_finish_blocking(fft->dma_chan);
    adc_run(false);
}

void fft_compute(fft_t* fft) {
    kiss_fft_scalar fft_in[NSAMP]; // kiss_fft_scalar is a float
    uint64_t sum = 0;
    for (size_t i = 0; i < fft->fft_size; i++) {
        sum += fft->adc_buffer[i];
    }
    float avg = (float)sum / fft->fft_size;
    for (size_t i = 0; i < fft->fft_size; i++) {
        fft_in[i] = (float)fft->adc_buffer[i] - avg;
    }

    kiss_fftr(fft->cfg, fft_in, fft->fft_out);
}

void fft_print(const fft_t* fft) {
    for (size_t i = 0; i < fft->fft_size / 2; ++i) {
        float magnitude = sqrt(fft->fft_out[i].r * fft->fft_out[i].r + fft->fft_out[i].i * fft->fft_out[i].i);
        printf("Frequency bin %zu: %f Hz, Amplitude: %f\n", i, fft->freqs[i], magnitude);
    }
}
