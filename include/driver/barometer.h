#ifndef BAROMETER_H
#define BAROMETER_H

#include "i2cHandler.h"
#include "settings.h"

esp_err_t barometerTestConnection();

esp_err_t barometerReset();

esp_err_t barometerInit();

// temperature in degrees Celsius and pressure in Pa
esp_err_t barometerMeasure(float* temperature, int32_t* pressure);

#endif  // BAROMETER_H
