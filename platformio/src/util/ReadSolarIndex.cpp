#include "main.h"

SolarIndex::SolarIndex(const char *key, adc1_channel_t pin, double r1,
                       double r2)
    : _key(key), _pin(pin), R1(r1), R2(r2) {
  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten((adc1_channel_t)pin, ADC_ATTEN_DB_11);
  retrieveHighestVoltFromNVS();
}

double SolarIndex::readVoltage() {
  uint32_t adc_reading = adc1_get_raw(_pin);
  double adc_voltage = (adc_reading * 3.3) / 4095.0;
  double in_voltage = adc_voltage / (R2 / (R1 + R2));
  return in_voltage;
}

void SolarIndex::retrieveHighestVoltFromNVS() {
  double volt;
  if (retrieveDouble(_key, &volt))
    highestVolt = volt;
  else
    highestVolt = -1; // negative value indicate failure
}

double SolarIndex::read() {
  double volt = readVoltage();
  if (volt > highestVolt) {
    if (storeDouble(_key, volt))
      highestVolt = volt;
  }
  return SOLAR_INDEX_MAX_VALUE * (volt / highestVolt);
}