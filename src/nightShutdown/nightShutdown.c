#include "nightShutdown.h"

EventGroupHandle_t nightShutdownEvents;

bool isNightShutdown(float* hoursLeft, double latitude, double longitude, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute) {
    float hours = hour + (minute / 60.0);

    // calculate sunrise and sunset time
    // timezone and daylight savings time should not matter here
    float sunrise = calculateSunTimeExtrems(true, latitude, longitude, month, day, 0, 0);
    float sunset = calculateSunTimeExtrems(false, latitude, longitude, month, day, 0, 0);

    // add buffer
    sunrise += SUNRISE_BUFFER;
    sunset += SUNSET_BUFFER;

    // between 0:00 and sunrise or between sunset and 23:59
    // => during night shutdown
    if (hours < sunrise || hours > sunset) {
        xEventGroupSetBits(nightShutdownEvents, NIGHT_SHUTDOWN_EVENT);

        // calculate time left
        if (hours < sunrise) {  // between 0:00 and sunrise
            *hoursLeft = sunrise - hours;
        } else {  // between sunset and 23:59
            *hoursLeft = (24.0 - hours) + sunrise;
        }

        return true;
    } else {  // during day; not night shutdown
        xEventGroupClearBits(nightShutdownEvents, NIGHT_SHUTDOWN_EVENT);

        return false;
    }
}