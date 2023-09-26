#ifndef MAIN_H
#define MAIN_H
#include "driver/adc.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>

#define MAX_VOLTAGE_ADDRESS 0
#define SOLAR_THRESHOLDS_ADDRESS 8
#define SOLAR_INDEX_MAX_VALUE 1000.0
#define PIN_HIGH_THRESHOLD 2000

UartHandler Serial(UART_NUM_0, 115200);

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

/**
 * @class UartHandler
 * @brief A utility class for UART communication.
 *
 * The UartHandler class provides a simple interface for sending various data
 * types over UART in ESP-IDF applications. It allows you to send messages as
 * strings, float, double, int, unsigned int, and unsigned long.
 */

class UartHandler {
private:
  uart_port_t uart_num_;

public:
  UartHandler(uart_port_t uart_num, int baud_rate);
  ~UartHandler();

  void send(const char *message);
  void send(float value);
  void send(double value);
  void send(int value);
  void send(unsigned int value);
  void send(unsigned long value);
  void sendln();
};

/**
 * @class SolarIndex
 * @brief Represents a solar index sensor with voltage reading capabilities.
 *
 * The SolarIndex class provides functionality to read the voltage output
 * of a solar index sensor, calculate the solar index value based on the
 * voltage readings, and store and retrieve the highest voltage value in
 * non-volatile storage.
 */

class SolarIndex {
private:
  const char *_key;
  const adc1_channel_t _pin;
  double R1;
  double R2;
  double highestVolt;

  double readVoltage();
  void retrieveHighestVoltFromNVS();

public:
  SolarIndex(const char *key, adc1_channel_t pin, double r1 = 30000.0,
             double r2 = 7500.0);
  double read();
};

/**
 * @class SolarIndexMonitor
 * @brief Monitors and records solar index data and durations.
 *
 * The SolarIndexMonitor class is responsible for monitoring and recording solar
 * index data, including durations above and below specified thresholds, as well
 * as the duration within the thresholds. It provides methods to set thresholds,
 * update the solar index, retrieve accumulated durations, and debug recorded
 * data.
 */

class SolarIndexMonitor {
private:
  SolarThresholds _currentThreshold;
  double currentSolarIndex = 0;
  unsigned long startMillisAboveMax = 0;
  unsigned long startMillisBelowMin = 0;
  unsigned long startMillisWithinThresholds = 0;
  double maxThresholdDuringExceed = 0;
  double minThresholdDuringFall = 0;
  unsigned long accumulatedDurationAboveMax = 0;
  unsigned long accumulatedDurationBelowMin = 0;
  unsigned long accumulatedDurationWithinThresholds = 0;

public:
  void resetTimer();
  void setThresholds(const SolarThresholds &threshold);
  void updateSolarIndex(double newValue);
  void getAccumulatedDurations(unsigned long &durationAboveMax,
                               unsigned long &durationBelowMin);
  void getDurationWithinThreshold(unsigned long &durationWithinMax);
  void debugRecordedData();

private:
  bool isValidThreshold(const SolarThresholds &threshold);
  void handleThresholdExceed(bool isAboveMax, unsigned long currentMillis);
  void handleThresholdFall(bool isBelowMin, unsigned long currentMillis);
  void handleDurationWithinThreshold(bool isWithinThresholds,
                                     unsigned long currentMillis);
};

/**
 * @brief Solar-Powered Switch Controller
 *
 * The `SwitchController` class manages a solar-powered switch, monitoring a
 * solar index sensor and controlling the switch based on predefined thresholds
 * and time intervals. It provides methods to set thresholds, intervals, run the
 * controller, and debug recorded data.
 */

class SwitchController {
private:
  SolarThresholds threshold;
  gpio_num_t _relaySignalPin;
  const char *swThresholdAdrress;
  unsigned long previousMillis = 0;
  unsigned long intervalMinutes = 5;
  unsigned long intervalMillis = 0;
  SolarIndexMonitor indexMonitor;

public:
  SwitchController(gpio_num_t relaySignalPin);
  bool setInterval(unsigned short duration);
  bool setSolarThresholds(SolarThresholds threshold);
  bool setSolarThresholds(double max, double min);
  bool setSolarThresholds(double min);
  void run();
  void debug();
};

int64_t millis();

esp_err_t init_nvs();
bool storeValue(const char *key, int32_t value);
bool storeValue(const char *key, const char *value);
bool storeDouble(const char *key, double value);
bool retrieveValue(const char *key, char *value, size_t max_len);
bool retrieveDouble(const char *key, double *value);
bool retrieveIntValue(const char *key, int32_t *value);
bool storeSolarThresholds(const char *key, const SolarThresholds &value);
bool retrieveSolarThresholds(const char *key, SolarThresholds &value);

int analogRead(gpio_num_t pin);
uint32_t analogRead(adc_channel_t channel);
void digitalWrite(gpio_num_t pin, int value);

#endif
