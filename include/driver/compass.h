#ifndef COMPASS_H
#define COMPASS_H

#include "i2cHandler.h"
#include "settings.h"

esp_err_t compassModuleInit();

esp_err_t compassModuleReset();

esp_err_t compassModuleChangeMode(uint8_t mode);

// magnetic north
// static calibration (offx/offy)
// offx = (maxx+minx)/2;
// offy = (maxy+miny)/2;
// calculate bearing
// int atan2val = 180/M_PI * atan2((float)(x-offx),(float)(y-offy));
// bearing = (-atan2val + 360 ) % 360;
esp_err_t compassModuleReadAxis(int16_t* x, int16_t* y, int16_t* z);

// only relativ temperature
esp_err_t compassModuleReadTemperature(int16_t* t);

#endif  // COMPASS_H
