#ifndef LCD_BUTTONS_H
#define LCD_BUTTONS_H

#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lcdMenu.h"
#include "settings.h"

typedef enum {
    ACTION_NEXT,
    ACTION_PREV,
    ACTION_CONF
} buttonAction_t;

esp_err_t initLcdButtons();

#endif