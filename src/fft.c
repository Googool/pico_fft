#include "pico/fft.h"

static dma_channel_config cfg;
static uint dma_chan;
static float freqs[NSAMP];

void adc_fft_setup() {
    stdio_init_all();

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    adc_gpio_init(26 + CAPTURE_CHANNEL);

    adc_init();
    adc_select_input(CAPTURE_CHANNEL);
    adc_fifo_setup(
        true,    // Write each completed conversion to the sample FIFO
        true,    // Enable DMA data request (DREQ)
        1,       // DREQ (and IRQ) asserted when at least 1 sample present
        false,   // We won't see the ERR bit because of 8 bit reads; disable.
        true     // Shift each sample to 8 bits when pushing to FIFO
    );

    // Set sample rate
    adc_set_clkdiv(CLOCK_DIV);

    sleep_ms(1000);
    // Set up the DMA to start transferring data as soon as it appears in FIFO
    dma_chan = dma_claim_unused_channel(true);
    cfg = dma_channel_get_default_config(dma_chan);

    // Reading from constant address, writing to incrementing byte addresses
    channel_config_set_transfer_data_size(&cfg, DMA_SIZE_8);
    channel_config_set_read_increment(&cfg, false);
    channel_config_set_write_increment(&cfg, true);

    // Pace transfers based on availability of ADC samples
    channel_config_set_dreq(&cfg, DREQ_ADC);

    // Calculate frequencies of each bin
    float f_max = FSAMP;
    float f_res = f_max / NSAMP;
    for (int i = 0; i < NSAMP; i++) {
        freqs[i] = f_res * i;
    }
}

void adc_fft_sample(uint8_t *capture_buf) {
    adc_fifo_drain();
    adc_run(false);

    dma_channel_configure(dma_chan, &cfg,
                          capture_buf,    // dst
                          &adc_hw->fifo,  // src
                          NSAMP,          // transfer count
                          true            // start immediately
    );

    gpio_put(LED_PIN, 1);
    adc_run(true);
    dma_channel_wait_for_finish_blocking(dma_chan);
    gpio_put(LED_PIN, 0);
}

float adc_fft_process(uint8_t *capture_buf) {
    kiss_fft_scalar fft_in[NSAMP];
    kiss_fft_cpx fft_out[NSAMP];
    kiss_fftr_cfg cfg = kiss_fftr_alloc(NSAMP, false, 0, 0);

    // Fill Fourier transform input while subtracting DC component
    uint64_t sum = 0;
    for (int i = 0; i < NSAMP; i++) {
        sum += capture_buf[i];
    }
    float avg = (float)sum / NSAMP;
    for (int i = 0; i < NSAMP; i++) {
        fft_in[i] = (float)capture_buf[i] - avg;
    }

    // Compute fast Fourier transform
    kiss_fftr(cfg, fft_in, fft_out);

    // Compute power and calculate max freq component
    float max_power = 0;
    int max_idx = 0;
    // Any frequency bin over NSAMP/2 is aliased (Nyquist sampling theorem)
    for (int i = 0; i < NSAMP / 2; i++) {
        float power = fft_out[i].r * fft_out[i].r + fft_out[i].i * fft_out[i].i;
        if (power > max_power) {
            max_power = power;
            max_idx = i;
        }
    }

    float max_freq = freqs[max_idx];
    kiss_fft_free(cfg);

    return max_freq;
}
