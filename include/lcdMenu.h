#ifndef LCD_MENU_H
#define LCD_MENU_H

#include <rom/ets_sys.h>
#include <stdbool.h>
#include <unistd.h>

#include "charger.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lcdButtons.h"
#include "lcdDisplay.h"
#include "settings.h"
#include "state.h"

#define NUM_VALID_DISPLAY_ITEMS 17

extern display_config_t displayConfig;
extern const uint8_t validStates[NUM_VALID_DISPLAY_ITEMS];
bool lcdMenuDisplayState(uint8_t state, lcd_4bit_t* lcd);
void lcdMenuSleep(lcd_4bit_t* lcd);

#endif