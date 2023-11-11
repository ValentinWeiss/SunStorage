#include "tasks/displayButtonsTask.h"

#include "esp_log.h"
#include "lcdButtons.h"
#include "settings.h"
#include "tasks/displayTask.h"

bool old_state_0 = true;
bool old_state_1 = true;

bool button_0_pressed = false;
bool button_1_pressed = false;

void displayButtonsTask() {
    while (1) {
        if (!gpio_get_level(LCD_MENU_BUTTON_0)) {
            vTaskDelay(BOUNCE_DELAY);
            if (old_state_0) {
                button_0_pressed = true;
            }
            old_state_0 = false;
        } else {
            old_state_0 = true;
        }

        if (!gpio_get_level(LCD_MENU_BUTTON_1)) {
            vTaskDelay(BOUNCE_DELAY);
            if (old_state_1) {
                button_1_pressed = true;
            }
            old_state_1 = false;
        } else {
            old_state_1 = true;
        }

        if (button_0_pressed && button_1_pressed) {
            xTaskNotify(displayButtonsNotificationHandle, ACTION_CONF, eSetValueWithOverwrite);
        } else {
            if (button_0_pressed) xTaskNotify(displayButtonsNotificationHandle, ACTION_NEXT, eSetValueWithOverwrite);
            if (button_1_pressed) xTaskNotify(displayButtonsNotificationHandle, ACTION_PREV, eSetValueWithOverwrite);
        }

        button_0_pressed = false;
        button_1_pressed = false;

        vTaskDelay(BUTTONS_POLLING_RATE);
    }
}
