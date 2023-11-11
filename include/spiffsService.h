#ifndef SPIFFS_SERVICE_H
#define SPIFFS_SERVICE_H

#include "esp_err.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include "settings.h"

esp_err_t initSpiffs(void);

#endif  // SPIFFS_SERVICE_H
