#include "lcdButtons.h"

#define MIN_INTERRUPTE_DELAY 10  // ms

esp_err_t lcdButtonsInitButtons() {
    esp_err_t ret = ESP_OK;

    gpio_config_t io_config;
    io_config.pin_bit_mask = (1ULL << LCD_MENU_BUTTON_0);
    io_config.mode = GPIO_MODE_INPUT;
    io_config.pull_up_en = GPIO_PULLUP_ENABLE;
    io_config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_config.intr_type = GPIO_INTR_DISABLE;
    ret = gpio_config(&io_config);
    if (ret != ESP_OK) ESP_LOGE("initLcd", "failed to init lcd");

    gpio_config_t io_config2;
    io_config2.pin_bit_mask = (1ULL << LCD_MENU_BUTTON_1);
    io_config2.mode = GPIO_MODE_INPUT;
    io_config2.pull_up_en = GPIO_PULLUP_DISABLE;
    io_config2.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_config2.intr_type = GPIO_INTR_DISABLE;
    ret = gpio_config(&io_config2);
    if (ret != ESP_OK) ESP_LOGE("initLcd", "failed to init lcd");

    return ret;
}

esp_err_t initLcdButtons() {
    esp_err_t errCode = ESP_OK;

    errCode = lcdButtonsInitButtons();
    if (errCode != ESP_OK) {
        ESP_LOGE("display_buttons_task", "Initalization of buttons failed");
    }

    return errCode;
}
