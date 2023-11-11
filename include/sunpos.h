#ifndef SUNPOS_H
#define SUNPOS_H

#include "math.h"
#include "stdbool.h"

typedef struct {
    double azimuthAngle;
    double elevationAngle;
} sun_direction_t;

typedef struct {
    double latitude;
    double longitude;
} location_t;

float calculateSunTimeExtrems(bool isRise, float latitude, float longitude, uint8_t month, uint8_t day, uint8_t tz, uint8_t dts);

sun_direction_t calculateSunDirection(location_t loc, int year, int month, int day, int hour, int minute, int second, int timeZoneOffsetInHours);

int getDayOfYear(int year, int month, int day);

double toRadians(double degrees);

double toDegrees(double radians);

#endif  // SUNPOS_HsetContettypeBasedOnFile