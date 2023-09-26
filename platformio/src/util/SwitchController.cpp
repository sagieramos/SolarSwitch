#include "main.h"

#define MINUTES_TO_MILLIS 60000

const char *swMemStorage[] = {"sw0", "sw1", "sw2", "sw3"};
static unsigned short nextSwMem = 0;
SolarIndex solar("SolarRead", ADC1_CHANNEL_0);

/**
 * @brief Constructs a SwitchController object.
 *
 * @param relaySignalPin The GPIO pin connected to the relay control signal.
 *
 * This constructor initializes the `SwitchController` object with the specified
 * relay signal pin. It also sets up the initial threshold, interval, and
 * connects to the solar index sensor.
 */

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

/**
 * @brief Set the interval between switch control operations.
 *
 * @param durationInMinutes The interval duration in minutes (1-60).
 * @return `true` if the interval is set successfully, `false` otherwise.
 *
 * This method allows you to set the time interval between switch control
 * operations. The interval is specified in minutes, and it must be between 1
 * and 60 minutes.
 */

bool SwitchController::setInterval(unsigned short durationInMinutes) {
  if (durationInMinutes < 1 || durationInMinutes > 60)
    return false;

  intervalMillis = durationInMinutes * MINUTES_TO_MILLIS;

  return true;
}

/**
 * @brief Set the solar index sensor thresholds.
 *
 * @param newThreshold The new solar index thresholds.
 * @return `true` if the thresholds are set successfully, `false` otherwise.
 *
 * This method allows you to set the solar index thresholds for the switch
 * controller. The provided `newThreshold` object must have a maximum value
 * greater than or equal to the minimum value, and it must be different from the
 * current thresholds.
 */

bool SwitchController::setSolarThresholds(SolarThresholds newThreshold) {
  if (newThreshold.max >= newThreshold.min && newThreshold != threshold) {
    threshold = newThreshold;
    storeSolarThresholds(swThresholdAdrress, threshold);
    indexMonitor.setThresholds(threshold);
    return true;
  }

  return false;
}

/**
 * @brief Set the solar index sensor thresholds (maximum and minimum values).
 *
 * @param max The maximum solar index value.
 * @param min The minimum solar index value.
 * @return `true` if the thresholds are set successfully, `false` otherwise.
 *
 * This method allows you to set the solar index thresholds by specifying
 * maximum and minimum values. Both `max` and `min` must be within valid ranges
 * (max < min and 0 <= min <= SOLAR_INDEX_MAX_VALUE).
 */

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

/**
 * @brief Set the solar index sensor minimum threshold.
 *
 * @param min The minimum solar index value.
 * @return `true` if the threshold is set successfully, `false` otherwise.
 *
 * This method allows you to set the minimum solar index threshold.
 * The `min` value must be within a valid range (0 <= min <=
 * SOLAR_INDEX_MAX_VALUE).
 */

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

/**
 * @brief Run the solar-powered switch controller.
 *
 * This method reads the solar index sensor, updates accumulated durations, and
 * controls the switch based on the defined thresholds and time intervals.
 */

void SwitchController::run() {

  double solarIndex = solar.read();
  indexMonitor.updateSolarIndex(solarIndex);

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= intervalMillis) {
    unsigned long rangeDuration;
    indexMonitor.getDurationWithinThreshold(rangeDuration);
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
    indexMonitor.resetTimer();
  }
}

/**
 * @brief Debug recorded data of the solar index monitor.
 *
 * This method retrieves and displays recorded data from the solar index
 * monitor, including durations within thresholds, and more.
 */

void SwitchController::debug() { indexMonitor.debugRecordedData(); }
