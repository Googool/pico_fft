#ifndef FFT_H
#define FFT_H

<<<<<<< HEAD
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/dma.h"
#include "kiss_fftr.h"

typedef struct {
  uint adc_pin;
  size_t fft_size;
  int dma_chan;
  kiss_fft_cpx* fft_out;
  int16_t* adc_buffer;
  kiss_fftr_cfg cfg;
} fft_t;

void fft_init(fft_t* fft, uint adc_pin, size_t fft_size);
void fft_deinit(fft_t* fft);
void fft_sample(fft_t* fft);
void fft_compute(fft_t* fft);
void fft_print(const fft_t* fft);
=======
#include <stdint.h>

void fft(float *real, float *imag, uint16_t n);
>>>>>>> 6eb4603840bd719395db3059dfcb735ff3d6ca45

#endif /* FFT_H */
