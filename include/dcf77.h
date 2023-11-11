#ifndef DCF77_H
#define DCF77_H

#include <stdint.h>

#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "settings.h"

extern volatile uint8_t DCF77_SignalState;
extern volatile uint8_t DCF77_prevSignalState;

typedef struct {
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
    uint8_t weekDay;
    uint8_t month;
    uint16_t year;
    bool dst;
    bool leapYear;
} dcf77_date_t;

/**
 * @brief Initializes dcf77 module
 */
esp_err_t dcf77Init();

/**
 * @brief Reads the Signal and stores it to be decoded later
 */
void dcf77ReadSignal();

/**
 * @brief Reads Time from dcff77 and returns last valid dcf77 date received, even if more recent invalid ones were received
 *
 * @param date the date to be set to the last valid dcf77 date received; if no valid date has been received yet, all values are set to thier respective max value
 *
 * @return whether the last received signal was valid or not
 *
 * @retval true if last received signal is valid
 * @retval false if last received signal was not valid
 */
bool dcf77ReadTime(dcf77_date_t* date);

// for testing
void printDCF77Date(dcf77_date_t* date);

#endif // DCF77_H
