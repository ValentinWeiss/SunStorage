#ifndef SERVO_H
#define SERVO_H

#include "driver/mcpwm_prelude.h"
#include "esp_err.h"
#include "settings.h"

extern mcpwm_cmpr_handle_t comparatorRotate;
extern mcpwm_cmpr_handle_t comparatorTilt;

esp_err_t initServo();

void setDutyRotate(uint16_t duty);

void setDutyRotate(uint16_t duty);

void setSunpos(int16_t rotate_angle, int16_t tilt_angle);

#endif  // SERVO_H
