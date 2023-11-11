#ifndef WIFI_SERVICE_H
#define WIFI_SERVICE_H

#include <cJSON.h>
#include <string.h>

#include "esp_log.h"
#include "esp_mac.h"
#include "esp_spiffs.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "settings.h"
#include "stringConcat.h"

esp_err_t initWifi(void);
esp_err_t initAccessPoint(void);
esp_err_t connectToAccessPoint(char *, char *);
esp_err_t wifiStop();
void wifiScan(cJSON *);

#endif  // WIFI_SERVICE_H
