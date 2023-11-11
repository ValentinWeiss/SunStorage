#include "tasks/displayTask.h"

#include "driver/gpio.h"
#include "esp_log.h"
#include "lcdButtons.h"
#include "lcdDisplay.h"
#include "lcdMenu.h"
#include "settings.h"

TaskHandle_t displayButtonsNotificationHandle;

void displayTask() {
    int8_t displayState = 0;
    uint32_t notificationValue;

    lcd_4bit_t lcd;
    lcdInit4BitDefault(&lcd);

    lcdOn(&lcd);

    getDisplayConfig(&displayConfig);

    while (1) {
        if (!lcdMenuDisplayState((uint8_t)displayState, &lcd)) {
            displayState = (displayState + 1) % (NUM_DISPLAY_ITEMS);
        }

        else if (xTaskNotifyWait(0, 0, &notificationValue, LCD_MENU_TIMEOUT) == pdFALSE) {
            // if timeout occured
            lcdMenuSleep(&lcd);
        } else {
            if (notificationValue == ACTION_NEXT)
                displayState = (displayState + 1) % (NUM_DISPLAY_ITEMS);
            else if (notificationValue == ACTION_PREV) {
                if (--displayState < 0)
                    displayState = NUM_DISPLAY_ITEMS - 1;
            } else if (notificationValue == ACTION_CONF) {
                ESP_LOGE("lcdDisplayTask", "Reloaded LCD display");
                lcdInit4BitDefault(&lcd);
                getDisplayConfig(&displayConfig);
            }
            getDisplayConfig(&displayConfig);
        }
    }
}
