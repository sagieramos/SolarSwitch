#include "main.h"

SwitchController::SwitchController(u_int8_t solarSensorPin,
                                   u_int8_t relaySignalPin)
    : _solarSensorPin(solarSensorPin), _relaySignalPin(relaySignalPin),
      previousMinutes(0), interval(5) {
  indexMonitor.setThresholds(1000.0, 0.0);
}

void SwitchController::setInterval(unsigned int duration) {
  if (duration != interval && (duration) >= 1 && duration <= 60) {
    interval = duration;
  }
}

void SwitchController::setSolarThresholds(double max, double min) {
  indexMonitor.setThresholds(max, min);
}

void SwitchController::run() {
  double solarIndex = readSolarIndex(_solarSensorPin);
  indexMonitor.updateSolarIndex(solarIndex);

  unsigned long currentMinutes = millis() / 60000UL;

  if (currentMinutes - previousMinutes >= interval) {
    unsigned long durationAboveMax, durationBelowMin;
    indexMonitor.getAccumulatedDurations(durationAboveMax, durationBelowMin);

    int relaySignal = analogRead(_relaySignalPin);
    bool shouldToggleRelay =
        (durationAboveMax > durationBelowMin && relaySignal == LOW) ||
        (durationAboveMax <= durationBelowMin && relaySignal == HIGH);

    if (shouldToggleRelay) {
      digitalWrite(_relaySignalPin, !relaySignal);
    }

    previousMinutes = currentMinutes;
  }
}

void SwitchController::debug() { indexMonitor.debugRecordedData(); }
