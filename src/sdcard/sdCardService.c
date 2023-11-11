#include "sdCardService.h"

esp_err_t sdCardWriteFile(const char *path, char *data) {
    ESP_LOGI("SD_CARD", "Opening file %s", path);
    FILE *f = fopen(path, "w");
    if (f == NULL) {
        ESP_LOGE("SD_CARD", "Failed to open file for writing");
        return ESP_FAIL;
    }
    fprintf(f, data);
    fclose(f);
    ESP_LOGI("SD_CARD", "File written");
    return ESP_OK;
}

esp_err_t sdCardInit() {
    sdmmc_card_t *card;

    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024};

    const char mount_point[] = SDC_MOUNT_POINT;
    ESP_LOGI("SD_CARD", "Initializing SD card");

    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    host.max_freq_khz = 10000;
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = SDC_PIN_NUM_MOSI,
        .miso_io_num = SDC_PIN_NUM_MISO,
        .sclk_io_num = SDC_PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4000,
    };
    esp_err_t ret = spi_bus_initialize(host.slot, &bus_cfg, SDSPI_DEFAULT_DMA);
    if (ret != ESP_OK) {
        ESP_LOGE("SD_CARD", "Failed to initialize bus.");
        return ret;
    }
    // gpio_pulldown_dis(SDC_PIN_NUM_CS);
    esp_err_t res = gpio_set_pull_mode(SDC_PIN_NUM_CS, GPIO_PULLUP_ENABLE);
    // esp_err_t res = gpio_pullup_en(SDC_PIN_NUM_CS);
    if (res != ESP_OK) {
        ESP_LOGE("SD_CARD", "PULLUP FAILED");
    }
    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = SDC_PIN_NUM_CS;
    slot_config.host_id = host.slot;

    ESP_LOGI("SD_CARD", "Mounting filesystem");
    ret = esp_vfs_fat_sdspi_mount(mount_point, &host, &slot_config, &mount_config, &card);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE("SD_CARD",
                     "Failed to mount filesystem. "
                     "If you want the card to be formatted, set the CONFIG_EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
        } else {
            ESP_LOGE("SD_CARD",
                     "Failed to initialize the card (%s). "
                     "Make sure SD card lines have pull-up resistors in place.",
                     esp_err_to_name(ret));
        }
        return ret;
    }
    ESP_LOGI("SD_CARD", "Filesystem mounted");

    // Card has been initialized, print its properties
    sdmmc_card_print_info(stdout, card);

    return ESP_OK;
}
