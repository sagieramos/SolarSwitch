#include "main.h"

#define MINUTES_TO_MILLIS 60000

const char *swMemStorage[] = {"sw0", "sw1", "sw2", "sw3"};
static unsigned short nextSwMem = 0;
SolarIndex solar("SolarRead", ADC1_CHANNEL_0);

SwitchController::SwitchController(gpio_num_t relaySignalPin)
    : _relaySignalPin(relaySignalPin),
      swThresholdAdrress(swMemStorage[nextSwMem]),
      intervalMillis(intervalMinutes * MINUTES_TO_MILLIS) {

  if (retrieveSolarThresholds(swThresholdAdrress, threshold)) {
    indexMonitor.setThresholds(threshold);
    nextSwMem++;
  } else {
    SolarThresholds newThreshold;
    if (storeSolarThresholds(swThresholdAdrress, newThreshold))
      nextSwMem++;
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
    storeSolarThresholds(swThresholdAdrress, threshold);
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
    indexMonitor.setThresholds(threshold);
    storeSolarThresholds(swThresholdAdrress, threshold);
  }

  return true;
}

bool SwitchController::setSolarThresholds(double min) {
  if (min < 0 || min > SOLAR_INDEX_MAX_VALUE)
    return false;

  SolarThresholds newValue(SOLAR_INDEX_MAX_VALUE, min);

  if (threshold != newValue) {
    threshold = newValue;
    indexMonitor.setThresholds(threshold);
    indexMonitor.setThresholds(threshold);
    storeSolarThresholds(swThresholdAdrress, threshold);
  }

  return true;
}

void SwitchController::run() {

  double solarIndex = solar.read();
  indexMonitor.updateSolarIndex(solarIndex);

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= intervalMillis) {
    unsigned long durationAboveMax, durationBelowMin;
    indexMonitor.getAccumulatedDurations(durationAboveMax, durationBelowMin);

    unsigned long rangeDuration = durationAboveMax + durationBelowMin;
    int relaySignal = analogRead(_relaySignalPin);

    if (rangeDuration > intervalMillis && relaySignal <= PIN_HIGH_THRESHOLD) {
      digitalWrite(_relaySignalPin, 1);
    } else if (rangeDuration <= intervalMillis &&
               relaySignal > PIN_HIGH_THRESHOLD) {
      digitalWrite(_relaySignalPin, 0);
    }

    if ((rangeDuration > intervalMillis && relaySignal <= PIN_HIGH_THRESHOLD) ||
        (rangeDuration <= intervalMillis && relaySignal > PIN_HIGH_THRESHOLD)) {
      digitalWrite(_relaySignalPin, !relaySignal);
    }

    previousMillis = currentMillis;
    indexMonitor.resetTimmer();
  }
}

void SwitchController::debug() { indexMonitor.debugRecordedData(); }
