#include "main.h"

#define UART_BUFFER_SIZE 20

/**
 * @brief Construct a UartHandler object.
 *
 * @param uart_num The UART port to be used.
 * @param baud_rate The baud rate for UART communication.
 */

UartHandler::UartHandler(uart_port_t uart_num, int baud_rate)
    : uart_num_(uart_num) {
  uart_config_t uart_config = {
      .baud_rate = baud_rate,
      .data_bits = UART_DATA_8_BITS,
      .parity = UART_PARITY_DISABLE,
      .stop_bits = UART_STOP_BITS_1,
  };

  uart_param_config(uart_num_, &uart_config);
  uart_driver_install(uart_num_, 256, 0, 0, NULL, 0);
}

/**
 * @brief Destructor for cleaning up UART resources.
 *
 * When a UartHandler object goes out of scope, this destructor is automatically
 * called to release UART resources.
 */
UartHandler::~UartHandler() { uart_driver_delete(uart_num_); }

/**
 * @brief Send a string message over UART.
 *
 * @param message The message to be sent as a null-terminated string.
 */

void UartHandler::send(const char *message) {
  uart_write_bytes(uart_num_, message, strlen(message));
}

/**
 * @brief Send a float value over UART.
 *
 * @param value The float value to be sent.
 */

void UartHandler::send(float value) {
  char buffer[UART_BUFFER_SIZE];
  snprintf(buffer, UART_BUFFER_SIZE, "%f", value);
  send(buffer);
}

/**
 * @brief Send a double value over UART.
 *
 * @param value The double value to be sent.
 */

void UartHandler::send(double value) {
  char buffer[UART_BUFFER_SIZE];
  snprintf(buffer, UART_BUFFER_SIZE, "%f", value);
  send(buffer);
}

/**
 * @brief Send a integer value over UART.
 *
 * @param value The integer value to be sent.
 */

void UartHandler::send(int value) {
  char buffer[UART_BUFFER_SIZE];
  snprintf(buffer, UART_BUFFER_SIZE, "%d", value);
  send(buffer);
}

/**
 * @brief Send an unsigned integer value over UART.
 *
 * @param value The unsigned integer value to be sent.
 */

void UartHandler::send(unsigned int value) {
  char buffer[UART_BUFFER_SIZE];
  snprintf(buffer, UART_BUFFER_SIZE, "%u", value);
  send(buffer);
}

/**
 * @brief Send an unsigned long value over UART.
 *
 * @param value The unsigned long value to be sent.
 */

void UartHandler::send(unsigned long value) {
  char buffer[UART_BUFFER_SIZE];
  snprintf(buffer, UART_BUFFER_SIZE, "%lu\n", value);
  send(buffer);
}

/**
 * @brief Send a newline character over UART.
 */

void UartHandler::sendln() { send("\n"); }

/**
 * @brief Example usage of UartHandler for sending various data types over UART.
 *
 * @code{.cpp}
 * void app_main(void) {
 *     UartHandler uart0_handler(UART_NUM_0, 115200);
 *
 *     uart0_handler.send("Hello, ESP-IDF!\n");
 *     uart0_handler.send(3.14159f);
 *     uart0_handler.send(3.14159265359);
 *     uart0_handler.send(42);
 *     uart0_handler.send(1000u);
 * }
 * @endcode
 */