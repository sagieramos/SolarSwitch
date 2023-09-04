#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include <EEPROM.h>

#define MAX_VOLTAGE_ADDRESS 0
#define SOLAR_THRESHOLDS_ADDRESS 8
#define SOLAR_INDEX_MAX_VALUE 1000.0

struct SolarThresholds {
  double max;
  double min;

  SolarThresholds(double max_value = SOLAR_INDEX_MAX_VALUE,
                  double min_value = 0.0)
      : max(max_value), min(min_value) {}

  bool operator==(const SolarThresholds &other) const {
    return (max == other.max) && (min == other.min);
  }

  bool operator!=(const SolarThresholds &other) const {
    return !(*this == other);
  }
};

class SolarIndexMonitor {
private:
  SolarThresholds _threshold;
  double currentSolarIndex = 0;
  unsigned long startMillisAboveMax = 0;
  unsigned long startMillisBelowMin = 0;
  double maxThresholdDuringExceed = 0;
  double minThresholdDuringFall = 0;
  unsigned long accumulatedDurationAboveMax = 0;
  unsigned long accumulatedDurationBelowMin = 0;

public:
  void resetTimmer();
  void setThresholds(SolarThresholds threshold);
  void updateSolarIndex(double newValue);
  void getAccumulatedDurations(unsigned long &durationAboveMax,
                               unsigned long &durationBelowMin);
  void debugRecordedData();
};

class SwitchController {
private:
  SolarThresholds threshold;
  u_int8_t _solarSensorPin;
  u_int8_t _relaySignalPin;
  static unsigned short thresholdsEepromAddress;
  const unsigned short thresholdEepromInstanceAddress;
  unsigned long previousMillis = 0;
  unsigned long intervalMinutes = 5;
  unsigned long intervalMillis = 0;
  SolarIndexMonitor indexMonitor;

public:
  SwitchController(u_int8_t solarSensorPin, u_int8_t relaySignalPin);
  bool setInterval(unsigned short duration);
  bool setSolarThresholds(SolarThresholds threshold);
  bool setSolarThresholds(double max, double min);
  bool setSolarThresholds(double min);
  void run();
  static unsigned short getInstanceCount();
  void debug();
};

double readSolarIndex(const uint8_t pin);

#endif
