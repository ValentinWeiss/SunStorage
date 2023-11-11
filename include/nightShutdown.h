#ifndef NIGHT_SHUTDOWN_H
#define NIGHT_SHUTDOWN_H

#include <stdio.h>

#include "driver/gps.h"
#include "esp_event.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "servo.h"
#include "settings.h"
#include "soc.h"
#include "state.h"
#include "sunpos.h"

extern EventGroupHandle_t nightShutdownEvents;

/**
 * @brief calculates if night shutdown is active and for how much longer
 */
bool isNightShutdown(float* hoursLeft, double latitude, double longitude, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute);

#endif