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

class SolarIndex {
private:
  const adc1_channel_t _pin;
  const char *_key;
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

class SolarIndexMonitor {
private:
  SolarThresholds _threshold;
  double currentSolarIndex = 0;
  unsigned long startMillisAboveMax = 0;
  unsigned long startMillisBelowMin = 0;
  double maxThresholdDuringExceed = 0;
  double minThresholdDuringFall = 0;
  unsigned long accumulatedDurationAboveMax = 0;
  unsigned long accumulatedDurationBelowMin = 0;

public:
  void resetTimmer();
  void setThresholds(SolarThresholds threshold);
  void updateSolarIndex(double newValue);
  void getAccumulatedDurations(unsigned long &durationAboveMax,
                               unsigned long &durationBelowMin);
  void debugRecordedData();
};

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
