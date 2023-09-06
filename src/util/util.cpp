#include "main.h"

#define HIGH_THRESHOLD 2000

/*
GPIO_NUM_0
GPIO_NUM_1
GPIO_NUM_2
GPIO_NUM_3
GPIO_NUM_4
GPIO_NUM_5
GPIO_NUM_6
GPIO_NUM_7
GPIO_NUM_8
GPIO_NUM_9
GPIO_NUM_10
GPIO_NUM_11
GPIO_NUM_12
GPIO_NUM_13
GPIO_NUM_14
GPIO_NUM_15
GPIO_NUM_16
GPIO_NUM_17
GPIO_NUM_18
GPIO_NUM_19
GPIO_NUM_21
GPIO_NUM_22
GPIO_NUM_23
GPIO_NUM_25
GPIO_NUM_26
GPIO_NUM_27
GPIO_NUM_32
GPIO_NUM_33
GPIO_NUM_34
GPIO_NUM_35
GPIO_NUM_36
GPIO_NUM_37
GPIO_NUM_38
GPIO_NUM_39
*/

int analogRead(gpio_num_t pin) {
  if (!GPIO_IS_VALID_GPIO(pin) || !GPIO_IS_VALID_OUTPUT_GPIO(pin)) {
    return -1; // Invalid GPIO pin
  }

  // Configure the ADC if needed
  adc1_config_width(ADC_WIDTH_BIT_12); // Set ADC width to 12 bits (0-4095)
  adc1_config_channel_atten(
      (adc1_channel_t)pin,
      ADC_ATTEN_DB_11); // Set attenuation for the analog input pin

  return adc1_get_raw((adc1_channel_t)pin);
}

void digitalWrite(gpio_num_t pin, int value) {
  if (gpio_set_level(pin, value) != ESP_OK) {
    // Handle error
  }
}

uint32_t analogRead(adc1_channel_t channel) {
  // Configure the ADC
  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(channel, ADC_ATTEN_DB_11);

  // Read ADC sample
  uint32_t adc_reading = adc1_get_raw(channel);

  return adc_reading;
}