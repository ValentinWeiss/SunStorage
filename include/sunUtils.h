#ifndef SUNUTILS_H
#define SUNUTILS_H

#include "math.h"

float getHeading(int16_t x, int16_t xcorr, int16_t y, int16_t ycorr, int16_t z, int16_t zcorr, float xA, float yA, float zA);
void applyCalibration(int16_t x, int16_t y, int16_t z, int16_t *xcorr, int16_t *ycorr, int16_t *zcorr, int16_t **calibration_data);

/**
 * @brief calculates the pole orientation of solarpanel
 *
 * @return degrees: 0° equal to North, 90° equal to West an so on...
 */
float getPanelOrientation();

#endif  // SUNUTILS_H