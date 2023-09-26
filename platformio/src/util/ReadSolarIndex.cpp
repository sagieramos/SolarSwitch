#include "main.h"

/**
 * @brief Constructs a SolarIndex object.
 *
 * @param key The key for storing the highest voltage value in NVS.
 * @param pin The ADC pin connected to the solar index sensor.
 * @param r1 The resistance value R1 in the voltage divider circuit.
 * @param r2 The resistance value R2 in the voltage divider circuit.
 *
 * This constructor initializes the SolarIndex object with the provided
 * key, ADC pin, and resistance values R1 and R2. It also configures the
 * ADC settings and retrieves the highest voltage value from non-volatile
 * storage.
 */

SolarIndex::SolarIndex(const char *key, adc1_channel_t pin, double r1 = 30000.0,
                       double r2 = 7500.0)
    : _key(key), _pin(pin), R1(r1), R2(r2) {
  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten((adc1_channel_t)pin, ADC_ATTEN_DB_11);
  retrieveHighestVoltFromNVS();
}

/**
 * @brief Reads the current voltage output from the solar index sensor.
 *
 * @return The voltage reading in volts.
 *
 * This method reads the voltage output from the solar index sensor, converts
 * it to volts, and adjusts it based on the voltage divider circuit (R1 and R2).
 */

double SolarIndex::readVoltage() {
  uint32_t adc_reading = adc1_get_raw(_pin);
  double adc_voltage = (adc_reading * 3.3) / 4095.0;
  double in_voltage = adc_voltage / (R2 / (R1 + R2));
  return in_voltage;
}

/**
 * @brief Retrieves the highest voltage value from non-volatile storage.
 *
 * This method retrieves the highest recorded voltage value from non-volatile
 * storage using the provided key and stores it in the `highestVolt` member
 * variable.
 */

void SolarIndex::retrieveHighestVoltFromNVS() {
  double volt;
  if (retrieveDouble(_key, &volt))
    highestVolt = volt;
  else
    highestVolt = -1; // negative value indicate failure
}

/**
 * @brief Reads the solar index value based on the voltage reading.
 *
 * @return The solar index value.
 *
 * This method reads the voltage output from the sensor, calculates the solar
 * index value, and stores the highest voltage value in non-volatile storage
 * if the current voltage is higher. It then returns the calculated solar index
 * value.
 */

double SolarIndex::read() {
  double volt = readVoltage();
  if (volt > highestVolt) {
    if (storeDouble(_key, volt))
      highestVolt = volt;
  }
  return SOLAR_INDEX_MAX_VALUE * (volt / highestVolt);
}