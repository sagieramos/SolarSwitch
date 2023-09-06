#include "./util/main.h"

extern "C" void app_main() {
  if (init_nvs() != ESP_OK) {
    // Handle NVS initialization error
    return;
  }

  while (true) {
  }
}