#ifndef FFT_H
#define FFT_H

#include "pico/stdlib.h"
#include "pico/kiss_fftr.h"
#include "hardware/adc.h"
#include "hardware/dma.h"

#define NSAMP 512

typedef struct {
    uint adc_pin;
    size_t fft_size;
    int dma_chan;
    kiss_fft_cpx* fft_out;
    uint8_t* adc_buffer;
    kiss_fftr_cfg cfg;
    float* freqs;
} fft_t;

void fft_init(fft_t* fft, uint adc_pin, size_t fft_size, float sample_rate);
void fft_deinit(fft_t* fft);
void fft_sample(fft_t* fft);
void fft_compute(fft_t* fft);
void fft_clear(fft_t* fft);

#endif // FFT_H
