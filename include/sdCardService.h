#ifndef SD_CARD_SERVICE_H
#define SD_CARD_SERVICE_H

#include "esp_err.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "settings.h"

esp_err_t sdCardInit();
esp_err_t sdCardWriteFile(const char *, char *);

#endif  // SD_CARD_SERVICE_H
