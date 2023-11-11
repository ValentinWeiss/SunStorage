#ifndef PANELPOS_H
#define PANELPOS_H

#include "driver/compass.h"
#include "driver/gps.h"
#include "driver/gyroscope.h"
#include "esp_log.h"
#include "esp_system.h"
#include "limits.h"
#include "servo.h"
#include "sunpos.h"

void calcServoPos(float xa, float ya, float za, float azimuth, float elevation, float *angleRotate, float *angleTilt);
#endif  // PANELPOS_H