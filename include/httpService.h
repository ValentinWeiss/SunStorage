#ifndef HTTP_SERVICE_H
#define HTTP_SERVICE_H

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cJSON.h"
#include "dbInterface.h"
#include "driver/compass.h"
#include "esp_chip_info.h"
#include "esp_err.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_spiffs.h"
#include "esp_system.h"
#include "esp_vfs.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "settings.h"
#include "soc.h"
#include "state.h"
#include "stringConcat.h"
#include "wifiService.h"

typedef struct restServerContext {
    char base_path[ESP_VFS_PATH_MAX + 1];
    char scratch[HTTP_SCRATCH_BUFSIZE];
} restServerContext_t;

esp_err_t initHttpServer(void);
bool getGpsEnabled();
bool getDcf77Enabled();

#endif  // HTTP_SERVICE_H
