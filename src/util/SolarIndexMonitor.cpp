#include "main.h"

void SolarIndexMonitor::setThresholds(SolarThresholds threshold) {
  if (_threshold.max >= _threshold.min && _threshold != threshold)
    _threshold = threshold;
}

void SolarIndexMonitor::updateSolarIndex(double newValue) {
  if (newValue < 0)
    return;

  unsigned long currentMillis = millis();
  bool isAboveMax = (newValue > _threshold.max);
  bool isBelowMin = (newValue < _threshold.min);

  if (isAboveMax && startMillisAboveMax == 0) {
    startMillisAboveMax = currentMillis;
    maxThresholdDuringExceed = _threshold.max;
  } else if (!isAboveMax && startMillisAboveMax != 0) {
    accumulatedDurationAboveMax += (currentMillis - startMillisAboveMax);
    startMillisAboveMax = 0;
  }

  if (isBelowMin && startMillisBelowMin == 0) {
    startMillisBelowMin = currentMillis;
    minThresholdDuringFall = _threshold.min;
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

void SolarIndexMonitor::resetTimmer() {
  accumulatedDurationAboveMax = 0;
  accumulatedDurationBelowMin = 0;
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
