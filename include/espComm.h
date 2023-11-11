#ifndef ESP_COMM_H
#define ESP_COMM_H

#include "cJSON.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_err.h"
#include "esp_log.h"
#include "settings.h"
#include "state.h"

esp_err_t initCommUART(uint8_t type);
esp_err_t espCommTransmit();

#endif  // ESP_COMM_H