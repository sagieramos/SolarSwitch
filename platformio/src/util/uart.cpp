#include "main.h"

#define UART_BUFFER_SIZE 20

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

UartHandler::~UartHandler() { uart_driver_delete(uart_num_); }

void UartHandler::send(const char *message) {
  uart_write_bytes(uart_num_, message, strlen(message));
}

void UartHandler::send(float value) {
  char buffer[UART_BUFFER_SIZE];
  snprintf(buffer, UART_BUFFER_SIZE, "%f", value);
  send(buffer);
}

void UartHandler::send(double value) {
  char buffer[UART_BUFFER_SIZE];
  snprintf(buffer, UART_BUFFER_SIZE, "%f", value);
  send(buffer);
}

void UartHandler::send(int value) {
  char buffer[UART_BUFFER_SIZE];
  snprintf(buffer, UART_BUFFER_SIZE, "%d", value);
  send(buffer);
}

void UartHandler::send(unsigned int value) {
  char buffer[UART_BUFFER_SIZE];
  snprintf(buffer, UART_BUFFER_SIZE, "%u", value);
  send(buffer);
}

void UartHandler::send(unsigned long value) {
  char buffer[UART_BUFFER_SIZE];
  snprintf(buffer, UART_BUFFER_SIZE, "%lu\n", value);
}

void UartHandler::sendln() { send("\n"); }

/* Example usage:
void app_main(void) {
    UartHandler uart0_handler(UART_NUM_0, 115200);

    uart0_handler.send("Hello, ESP-IDF!\n");
    uart0_handler.send(3.14159f);
    uart0_handler.send(3.14159265359);
    uart0_handler.send(42);
    uart0_handler.send(1000u);

    UART cleanup will be automatically called when 'uart0_handler' goes out of
scope
}
 */