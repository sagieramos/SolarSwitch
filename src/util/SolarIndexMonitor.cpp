#include "main.h"

SolarIndexMonitor::SolarIndexMonitor()
    : maxThreshold(0.0), minThreshold(0.0), currentSolarIndex(0.0) {
  startMillisAboveMax = 0;
  startMillisBelowMin = 0;
  accumulatedDurationAboveMax = 0;
  accumulatedDurationBelowMin = 0;
}

void SolarIndexMonitor::setThresholds(double max, double min) {
  maxThreshold = max;
  minThreshold = min;
}

void SolarIndexMonitor::updateSolarIndex(double newValue) {
  currentSolarIndex = newValue;
  unsigned long currentMillis = millis();

  if (currentSolarIndex > maxThreshold) {
    if (startMillisAboveMax == 0) {
      startMillisAboveMax = currentMillis;
      maxThresholdDuringExceed = maxThreshold;
    }
  } else {
    if (startMillisAboveMax != 0) {
      unsigned long duration = currentMillis - startMillisAboveMax;
      accumulatedDurationAboveMax += duration;
      startMillisAboveMax = 0;
    }
  }

  if (currentSolarIndex < minThreshold) {
    if (startMillisBelowMin == 0) {
      startMillisBelowMin = currentMillis;
      minThresholdDuringFall = minThreshold;
    }
  } else {
    if (startMillisBelowMin != 0) {
      unsigned long duration = currentMillis - startMillisBelowMin;
      accumulatedDurationBelowMin += duration;
      startMillisBelowMin = 0;
    }
  }
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
