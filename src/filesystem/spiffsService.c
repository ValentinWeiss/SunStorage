#include "spiffsService.h"

size_t total = 0, used = 0;

esp_err_t initSpiffs() {
    ESP_LOGI("SPIFFS", "Initializing SPIFFS");
    esp_vfs_spiffs_conf_t spiffsConfig = {.base_path = SPIFFS_BASE_PATH,
                                          .partition_label = "spiffs",
                                          .max_files = 10,
                                          .format_if_mount_failed = false};
    esp_err_t res = esp_vfs_spiffs_register(&spiffsConfig);

    if (res != ESP_OK) {
        if (res == ESP_FAIL) {
            ESP_LOGE("SPIFFS", "Failed to mount or format filesystem");
        } else if (res == ESP_ERR_NOT_FOUND) {
            ESP_LOGE("SPIFFS", "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE("SPIFFS", "Failed to initialize SPIFFS (%s)",
                     esp_err_to_name(res));
        }
        return res;
    }

    res = esp_spiffs_info(spiffsConfig.partition_label, &total, &used);
    if (res != ESP_OK) {
        ESP_LOGE(
            "SPIFFS",
            "Failed to get SPIFFS partition information (%s). Formatting...",
            esp_err_to_name(res));
        esp_spiffs_format(spiffsConfig.partition_label);
        return res;
    } else {
        ESP_LOGI("SPIFFS", "Partition size: total: %d, used: %d", total, used);
    }

    // Check consistency of reported partiton size info.
    if (used > total) {
        ESP_LOGW("SPIFFS",
                 "Number of used bytes cannot be larger than total. Performing "
                 "SPIFFS_check().");
        res = esp_spiffs_check(spiffsConfig.partition_label);
        // Could be also used to mend broken files, to clean unreferenced pages,
        // etc. More info at
        // https://github.com/pellepl/spiffs/wiki/FAQ#powerlosses-contd-when-should-i-run-spiffs_check
        if (res != ESP_OK) {
            ESP_LOGE("SPIFFS", "SPIFFS_check() failed (%s)",
                     esp_err_to_name(res));
            return res;
        } else {
            ESP_LOGI("SPIFFS", "SPIFFS_check() successful");
        }
    }

    return ESP_OK;
}
