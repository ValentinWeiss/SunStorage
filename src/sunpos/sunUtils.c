#include "sunUtils.h"

#include "state.h"
float getHeading(int16_t x, int16_t xcorr, int16_t y, int16_t ycorr, int16_t z, int16_t zcorr, float xA, float yA, float zA) {
    float xAccel = xA;
    float yAccel = yA;
    float zAccel = zA;
    float xCompass = x - xcorr;
    float yCompass = y - ycorr;
    float zCompass = z - zcorr;
    float ratio = ((xCompass * xAccel + yCompass * yAccel + zCompass * zAccel) / (xAccel * xAccel + yAccel * yAccel + zAccel * zAccel));
    xAccel *= ratio;
    yAccel *= ratio;
    zAccel *= ratio;
    xCompass -= xAccel;
    yCompass -= yAccel;
    return ((float)(180 / M_PI) * atan2f(xCompass, yCompass));
}

// azimuth 0° N 90° W; heading 0° N, + -> W, - -> S
// return: 0° N, 90° W;
float getPanelOrientation() {
    return getEsp2Azimuth() + getEsp2Heading();
}

void applyCalibration(int16_t x, int16_t y, int16_t z, int16_t *xcorr, int16_t *ycorr, int16_t *zcorr, int16_t **calibrationData) {
    calibrationData[0][0] = -32768;
    calibrationData[1][0] = -32768;
    calibrationData[2][0] = -32768;
    calibrationData[0][1] = 32767;
    calibrationData[1][1] = 32767;
    calibrationData[2][1] = 32767;

    if (x > calibrationData[0][0]) {
        calibrationData[0][0] = x;
    }
    if (x < calibrationData[0][1]) {
        calibrationData[0][1] = x;
    }
    if (y > calibrationData[1][0]) {
        calibrationData[1][0] = y;
    }
    if (y < calibrationData[1][1]) {
        calibrationData[1][1] = y;
    }
    if (z > calibrationData[2][0]) {
        calibrationData[2][0] = z;
    }
    if (z < calibrationData[2][1]) {
        calibrationData[2][1] = z;
    }

    *xcorr = ((calibrationData[0][0] + calibrationData[0][1]) / 2);
    *ycorr = ((calibrationData[1][0] + calibrationData[1][1]) / 2);
    *zcorr = ((calibrationData[2][0] + calibrationData[2][1]) / 2);
}
