#include "./util/main.h"

extern "C" void app_main() {
  if (init_nvs() != ESP_OK) {
    return;
  }

  while (true) {
  }
}