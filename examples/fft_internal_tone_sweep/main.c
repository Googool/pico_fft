#include "pico/stdlib.h"
#include "pico/fft.h"
#include "hardware/pwm.h"

/**
 * This example demonstrates the use of the FFT library.
 * It generates a sweeping tone using PWM, samples the tone with ADC, and performs FFT analysis.
 */

void setup_pwm(uint pin, float freq) {
  gpio_set_function(pin, GPIO_FUNC_PWM);
  uint slice_num = pwm_gpio_to_slice_num(pin);

  pwm_config config = pwm_get_default_config();
  pwm_config_set_clkdiv(&config, 4.0f); // Adjust this to change the PWM frequency range.
  pwm_init(slice_num, &config, true);

  uint32_t level = (uint32_t)(0.5f * (1 << 16)); // 50% duty cycle.
  pwm_set_gpio_level(pin, level);

  uint32_t wrap = (uint32_t)(125e6 / (4.0f * freq)) - 1;
  pwm_set_wrap(slice_num, wrap);
  pwm_set_chan_level(slice_num, pwm_gpio_to_channel(pin), level);
}

int main(void) {
  const uint PWM_PIN = 15;
  const uint ADC_PIN = 26;
  const float START_FREQ = 0.0f;
  const float END_FREQ = 20000.0f;
  const float STEP_FREQ = 100.0f;

  fft_t fft;
  fft_init(&fft, ADC_PIN, 1024);

  float freq = START_FREQ;

  while (true) {
    setup_pwm(PWM_PIN, freq);
    sleep_ms(500);

    fft_sample(&fft);
    fft_compute(&fft);
    fft_print(&fft);

    freq += STEP_FREQ;
    if (freq > END_FREQ) {
      freq = START_FREQ;  // Reset the frequency sweep
    }

    sleep_ms(1000);
  }

  fft_deinit(&fft);
  return 0;
}
