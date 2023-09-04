#include "main.h"

#define MINUTES_TO_MILLIS 60000

SwitchController::SwitchController(u_int8_t solarSensorPin,
                                   u_int8_t relaySignalPin)
    : _solarSensorPin(solarSensorPin), _relaySignalPin(relaySignalPin),
      thresholdEepromInstanceAddress(thresholdsEepromAddress +=
                                     sizeof(SolarThresholds)),
      intervalMillis(intervalMinutes * MINUTES_TO_MILLIS) {
  EEPROM.get(thresholdEepromInstanceAddress, threshold);

  if (isnan(threshold.max) && isnan(threshold.min)) {
    indexMonitor.setThresholds(threshold);
  } else {
    SolarThresholds newThreshold;

    EEPROM.put(thresholdEepromInstanceAddress, newThreshold);
    EEPROM.commit();
  }
}

bool SwitchController::setInterval(unsigned short durationInMinutes) {
  if (durationInMinutes < 1 || durationInMinutes > 60)
    return false;

  intervalMillis = durationInMinutes * MINUTES_TO_MILLIS;

  return true;
}

bool SwitchController::setSolarThresholds(SolarThresholds newThreshold) {
  if (newThreshold.max >= newThreshold.min && newThreshold != threshold) {
    threshold = newThreshold;

    EEPROM.put(thresholdEepromInstanceAddress, threshold);
    EEPROM.commit();
    indexMonitor.setThresholds(threshold);
    return true;
  }

  return false;
}

bool SwitchController::setSolarThresholds(double max, double min) {
  if (max < min || (max > SOLAR_INDEX_MAX_VALUE || min < 0))
    return false;

  SolarThresholds newValue{max, min};

  if (threshold != newValue) {
    threshold = newValue;
    EEPROM.put(thresholdEepromInstanceAddress, threshold);
    EEPROM.commit();
    indexMonitor.setThresholds(threshold);
  }

  return true;
}

bool SwitchController::setSolarThresholds(double min) {
  if (min < 0 || min > SOLAR_INDEX_MAX_VALUE)
    return false;

  SolarThresholds newValue(SOLAR_INDEX_MAX_VALUE, min);

  if (threshold != newValue) {
    threshold = newValue;
    EEPROM.put(thresholdEepromInstanceAddress, threshold);
    EEPROM.commit();
    indexMonitor.setThresholds(threshold);
  }

  return true;
}

void SwitchController::run() {
  double solarIndex = readSolarIndex(_solarSensorPin);
  indexMonitor.updateSolarIndex(solarIndex);

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= intervalMillis) {
    unsigned long durationAboveMax, durationBelowMin;
    indexMonitor.getAccumulatedDurations(durationAboveMax, durationBelowMin);

    unsigned long rangeDuration = durationAboveMax + durationBelowMin;
    int relaySignal = analogRead(_relaySignalPin);

    if ((rangeDuration > intervalMillis && relaySignal == LOW) ||
        (rangeDuration <= intervalMillis && relaySignal == HIGH)) {
      digitalWrite(_relaySignalPin, !relaySignal);
    }

    previousMillis = currentMillis;
    indexMonitor.resetTimmer();
  }
}

unsigned short SwitchController::getInstanceCount() {
  return (thresholdsEepromAddress / sizeof(SolarThresholds));
}

void SwitchController::debug() { indexMonitor.debugRecordedData(); }
