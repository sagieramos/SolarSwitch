#include "main.h"

#define R1 30000.0
#define R2 7500.0

double readVoltageFromSensor(const uint8_t pin) {
  double adc_value = analogRead(pin);
  double adc_voltage = (adc_value * 3.3) / 4095.0;
  double in_voltage = adc_voltage / (R2 / (R1 + R2));
  return in_voltage;
}

double getMaxVoltageRecordedInEEPROM(double volt) {
  if (isnan(volt))
    return -1;

  double max_volt;
  EEPROM.get(MAX_VOLTAGE_ADDRESS, max_volt);

  if (isnan(max_volt) || volt > max_volt) {
    max_volt = volt;
    EEPROM.put(MAX_VOLTAGE_ADDRESS, max_volt);
    EEPROM.commit();
  }

  return max_volt;
}

double readSolarIndex(const uint8_t pin) {
  double voltage = readVoltageFromSensor(pin);
  double max_volt = getMaxVoltageRecordedInEEPROM(voltage);
  return SOLAR_INDEX_MAX_VALUE * (voltage / max_volt);
}