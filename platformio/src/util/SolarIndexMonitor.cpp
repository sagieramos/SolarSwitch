/**
 * @file SolarIndexMonitor.cpp
 * @brief Implementation of the SolarIndexMonitor class.
 */

#include "main.h"

/**
 * @brief Reset Timer and Accumulated Durations
 *
 * This method resets the timer and accumulated durations within specified
 * thresholds. It sets the accumulated duration values for durations above the
 * maximum threshold, below the minimum threshold, and within the defined
 * thresholds to zero. This is typically used to start tracking durations from a
 * clean slate.
 */

void SolarIndexMonitor::resetTimer() {
  accumulatedDurationAboveMax = 0;
  accumulatedDurationBelowMin = 0;
  accumulatedDurationWithinThresholds = 0;
}

/**
 * @brief Sets the solar thresholds.
 *
 * @param threshold The new solar thresholds to set.
 *
 * This method sets the current solar thresholds based on the provided
 * SolarThresholds structure, if the thresholds are valid.
 */

void SolarIndexMonitor::setThresholds(const SolarThresholds &threshold) {
  if (isValidThreshold(threshold)) {
    _currentThreshold = threshold;
  }
}

/**
 * @brief Updates the solar index and accumulates durations.
 *
 * @param newValue The new solar index value.
 *
 * This method updates the current solar index value and accumulates
 * durations based on whether the value is within the specified thresholds.
 */

void SolarIndexMonitor::updateSolarIndex(double newValue) {
  if (newValue < 0) {
    return;
  }

  unsigned long currentMillis = millis();
  bool isAboveMax = (newValue > _currentThreshold.max);
  bool isBelowMin = (newValue < _currentThreshold.min);
  bool isWithinThresholds = (!isAboveMax && !isBelowMin);

  handleThresholdExceed(isAboveMax, currentMillis);
  handleThresholdFall(isBelowMin, currentMillis);
  handleDurationWithinThreshold(isWithinThresholds, currentMillis);

  currentSolarIndex = newValue;
}

/**
 * @brief Retrieves the accumulated durations above and below thresholds.
 *
 * @param durationAboveMax [out] Accumulated duration above the max threshold.
 * @param durationBelowMin [out] Accumulated duration below the min threshold.
 *
 * This method allows you to retrieve the accumulated durations above and
 * below the specified thresholds.
 */

void SolarIndexMonitor::getAccumulatedDurations(
    unsigned long &durationAboveMax, unsigned long &durationBelowMin) {
  durationAboveMax = accumulatedDurationAboveMax;
  durationBelowMin = accumulatedDurationBelowMin;
}

void SolarIndexMonitor::getDurationWithinThreshold(
    unsigned long &durationWithinMax) {
  durationWithinMax = accumulatedDurationWithinThresholds;
}

/**
 * @brief Outputs recorded data for debugging purposes.
 *
 * This method outputs recorded data, including the total duration above
 * the max threshold, max threshold during exceedance, total duration below
 * the min threshold, and min threshold during a fall.
 * the threshold range.
 */

void SolarIndexMonitor::debugRecordedData() {
  Serial.send("Total Duration above max threshold (ms): ");
  Serial.send(accumulatedDurationAboveMax);
  Serial.sendln();
  Serial.send("Max Threshold During Exceed: ");
  Serial.send(maxThresholdDuringExceed);
  Serial.sendln();
  Serial.send("Total Duration below min threshold (ms): ");
  Serial.send(accumulatedDurationBelowMin);
  Serial.sendln();
  Serial.send("Min Threshold During Fall: ");
  Serial.send(minThresholdDuringFall);
  Serial.sendln();
  Serial.send("Total Duration within threshold (ms): ");
  Serial.send(accumulatedDurationWithinThresholds);
  Serial.sendln();
}

/**
 * @brief Checks if the provided thresholds are valid.
 *
 * @param threshold The SolarThresholds structure to validate.
 * @return true if the thresholds are valid; false otherwise.
 *
 * This private method checks if the provided thresholds are valid,
 * i.e., the maximum threshold is greater than or equal to the minimum
 * threshold.
 */

bool SolarIndexMonitor::isValidThreshold(const SolarThresholds &threshold) {
  return (threshold.max >= threshold.min);
}

/**
 * @brief Handles the exceedance of the max threshold.
 *
 * @param isAboveMax Indicates if the solar index is above the max threshold.
 * @param currentMillis The current timestamp in milliseconds.
 *
 * This private method tracks the duration above the max threshold and records
 * the max threshold value during exceedance.
 */

void SolarIndexMonitor::handleThresholdExceed(bool isAboveMax,
                                              unsigned long currentMillis) {
  if (isAboveMax && startMillisAboveMax == 0) {
    startMillisAboveMax = currentMillis;
    maxThresholdDuringExceed = _currentThreshold.max;
  } else if (!isAboveMax && startMillisAboveMax != 0) {
    accumulatedDurationAboveMax += (currentMillis - startMillisAboveMax);
    startMillisAboveMax = 0;
  }
}

/**
 * @brief Handles the fall below the min threshold.
 *
 * @param isBelowMin Indicates if the solar index is below the min threshold.
 * @param currentMillis The current timestamp in milliseconds.
 *
 * This private method tracks the duration below the min threshold and records
 * the min threshold value during a fall.
 */

void SolarIndexMonitor::handleThresholdFall(bool isBelowMin,
                                            unsigned long currentMillis) {
  if (isBelowMin && startMillisBelowMin == 0) {
    startMillisBelowMin = currentMillis;
    minThresholdDuringFall = _currentThreshold.min;
  } else if (!isBelowMin && startMillisBelowMin != 0) {
    accumulatedDurationBelowMin += (currentMillis - startMillisBelowMin);
    startMillisBelowMin = 0;
  }
}

/**
 * @brief Handles the duration within the max threshold.
 *
 * @param isWithinThresholds Indicates if the solar index is within thresholds.
 * @param currentMillis The current timestamp in milliseconds.
 *
 * This private method tracks the duration within the threshold.
 */

void SolarIndexMonitor::handleDurationWithinThreshold(
    bool isWithinThresholds, unsigned long currentMillis) {
  if (isWithinThresholds && startMillisWithinThresholds == 0) {
    startMillisWithinThresholds = currentMillis;
  } else if (!isWithinThresholds && startMillisWithinThresholds != 0) {
    accumulatedDurationWithinThresholds +=
        (currentMillis - startMillisWithinThresholds);
    startMillisWithinThresholds = 0;
  }
}
