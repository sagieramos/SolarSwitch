#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include <EEPROM.h>

#define MAX_VOLTAGE_ADDRESS 0
#define SOLAR_INDEX_MAX_VALUE 1000

class SolarIndexMonitor {
private:
  double maxThreshold;
  double minThreshold;
  double currentSolarIndex;
  unsigned long startMillisAboveMax;
  unsigned long startMillisBelowMin;
  double maxThresholdDuringExceed;
  double minThresholdDuringFall;
  unsigned long accumulatedDurationAboveMax;
  unsigned long accumulatedDurationBelowMin;

public:
  SolarIndexMonitor();
  void setThresholds(double max, double min);
  void updateSolarIndex(double newValue);
  void getAccumulatedDurations(unsigned long &durationAboveMax,
                               unsigned long &durationBelowMin);
  void debugRecordedData();
};

double readSolarIndex(const uint8_t pin);

#endif
