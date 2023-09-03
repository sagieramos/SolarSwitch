#include "main.h"

SolarIndexMonitor::SolarIndexMonitor()
    : maxThreshold(0.0), minThreshold(0.0), currentSolarIndex(0.0) {
  startMillisAboveMax = 0;
  startMillisBelowMin = 0;
  accumulatedDurationAboveMax = 0;
  accumulatedDurationBelowMin = 0;
}

void SolarIndexMonitor::setThresholds(double max, double min) {
  if (max >= min && (max != maxThreshold || min != minThreshold)) {
    maxThreshold = max;
    minThreshold = min;
  }
}

void SolarIndexMonitor::updateSolarIndex(double newValue) {
  if (newValue < 0)
    return;

  unsigned long currentMillis = millis();
  bool isAboveMax = (newValue > maxThreshold);
  bool isBelowMin = (newValue < minThreshold);

  if (isAboveMax && startMillisAboveMax == 0) {
    startMillisAboveMax = currentMillis;
    maxThresholdDuringExceed = maxThreshold;
  } else if (!isAboveMax && startMillisAboveMax != 0) {
    accumulatedDurationAboveMax += (currentMillis - startMillisAboveMax);
    startMillisAboveMax = 0;
  }

  if (isBelowMin && startMillisBelowMin == 0) {
    startMillisBelowMin = currentMillis;
    minThresholdDuringFall = minThreshold;
  } else if (!isBelowMin && startMillisBelowMin != 0) {
    accumulatedDurationBelowMin += (currentMillis - startMillisBelowMin);
    startMillisBelowMin = 0;
  }

  currentSolarIndex = newValue;
}

void SolarIndexMonitor::getAccumulatedDurations(
    unsigned long &durationAboveMax, unsigned long &durationBelowMin) {
  durationAboveMax = accumulatedDurationAboveMax;
  durationBelowMin = accumulatedDurationBelowMin;
}

void SolarIndexMonitor::debugRecordedData() {
  Serial.print("Total Duration above max threshold (ms): ");
  Serial.print(accumulatedDurationAboveMax);
  Serial.println(" milliseconds");
  Serial.print("Max Threshold During Exceed: ");
  Serial.println(maxThresholdDuringExceed);

  Serial.print("Total Duration below min threshold (ms): ");
  Serial.print(accumulatedDurationBelowMin);
  Serial.println(" milliseconds");
  Serial.print("Min Threshold During Fall: ");
  Serial.println(minThresholdDuringFall);
}
