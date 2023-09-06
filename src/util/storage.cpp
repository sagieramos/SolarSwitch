#include "esp_system.h"
#include "esp_timer.h"
#include "main.h"
#include "nvs.h"
#include "nvs_flash.h"

#define SW_STORAGE "storage"

int64_t millis() { return (esp_timer_get_time() / 1000); }

nvs_handle_t SwNamespace;

// Initialize NVS
esp_err_t init_nvs() {
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    // Handle NVS partitioning and version conflicts
    return ESP_FAIL;
  }
  return ESP_OK;
}

// Open an NVS namespace
esp_err_t open_nvs_namespace(const char *nameSpace) {
  return nvs_open(nameSpace, NVS_READWRITE, &SwNamespace);
}

// Close the NVS namespace
void close_nvs_namespace() { nvs_close(SwNamespace); }

// Store an integer value in NVS
bool storeValue(const char *key, int32_t value) {
  if (open_nvs_namespace(SW_STORAGE) != ESP_OK)
    return false;

  if (nvs_set_i32(SwNamespace, key, value) != ESP_OK) {
    close_nvs_namespace();
    return false;
  }

  if (nvs_commit(SwNamespace) != ESP_OK) {
    close_nvs_namespace();
    return false;
  }

  close_nvs_namespace();
  return true;
}

// Store a string value in NVS
bool storeValue(const char *key, const char *value) {
  if (open_nvs_namespace(SW_STORAGE) != ESP_OK)
    return false;

  if (nvs_set_str(SwNamespace, key, value) != ESP_OK) {
    close_nvs_namespace();
    return false;
  }

  if (nvs_commit(SwNamespace) != ESP_OK) {
    close_nvs_namespace();
    return false;
  }

  close_nvs_namespace();
  return true;
}

// Store a double value in NVS as a string
bool storeDouble(const char *key, double value) {
  char doubleStr[32]; // Adjust the buffer size as needed
  snprintf(doubleStr, sizeof(doubleStr), "%lf", value);

  if (open_nvs_namespace(SW_STORAGE) != ESP_OK)
    return false;

  if (nvs_set_str(SwNamespace, key, doubleStr) != ESP_OK) {
    close_nvs_namespace();
    return false;
  }

  if (nvs_commit(SwNamespace) != ESP_OK) {
    close_nvs_namespace();
    return false;
  }

  close_nvs_namespace();
  return true;
}

// Function to store the struct in NVS
bool storeSolarThresholds(const char *key, const SolarThresholds &value) {
    if (open_nvs_namespace("storage") != ESP_OK)
        return false;

    // Serialize the struct to a byte array
    uint8_t serializedData[sizeof(SolarThresholds)];
    memcpy(serializedData, &value, sizeof(SolarThresholds));

    if (nvs_set_blob(SwNamespace, key, serializedData, sizeof(SolarThresholds)) != ESP_OK) {
        close_nvs_namespace();
        return false;
    }

    if (nvs_commit(SwNamespace) != ESP_OK) {
        close_nvs_namespace();
        return false;
    }

    close_nvs_namespace();
    return true;
}

// Retrieve a string value from NVS
bool retrieveValue(const char *key, char *value, size_t max_len) {
  if (open_nvs_namespace(SW_STORAGE) != ESP_OK)
    return false;

  if (nvs_get_str(SwNamespace, key, value, &max_len) == ESP_OK) {
    close_nvs_namespace();
    return true;
  } else {
    close_nvs_namespace();
    return false;
  }
}

bool retrieveDouble(const char *key, double *value) {
  char doubleStr[32]; // Adjust the buffer size as needed
  size_t max_len = sizeof(doubleStr);

  if (open_nvs_namespace(SW_STORAGE) != ESP_OK)
    return false;

  if (nvs_get_str(SwNamespace, key, doubleStr, &max_len) == ESP_OK) {
    *value = atof(doubleStr); // Convert the string back to a double
    close_nvs_namespace();
    return true;
  } else {
    close_nvs_namespace();
    return false;
  }
}

// Retrieve an integer value from NVS
bool retrieveIntValue(const char *key, int32_t *value) {
  if (open_nvs_namespace(SW_STORAGE) != ESP_OK)
    return false;

  if (nvs_get_i32(SwNamespace, key, value) == ESP_OK) {
    close_nvs_namespace();
    return true;
  } else {
    close_nvs_namespace();
    return false;
  }
}

// Retrieve the olarThresholds from NVS
bool retrieveSolarThresholds(const char *key, SolarThresholds &value) {
    if (open_nvs_namespace("storage") != ESP_OK)
        return false;

    size_t data_size = sizeof(SolarThresholds);
    uint8_t serializedData[data_size];

    if (nvs_get_blob(SwNamespace, key, serializedData, &data_size) == ESP_OK) {
        // Deserialize the byte array back to the struct
        memcpy(&value, serializedData, sizeof(SolarThresholds));
        close_nvs_namespace();
        return true;
    } else {
        close_nvs_namespace();
        return false;
    }
}

