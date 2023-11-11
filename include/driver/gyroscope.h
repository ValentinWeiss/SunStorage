#ifndef GYROSCOPE_H
#define GYROSCOPE_H

#include "i2cHandler.h"
#include "settings.h"

// tries to read the WHO_AM_I register
esp_err_t gyroscopeTestConnection();

// initializes CONFIG registers
esp_err_t gyroscopeInit();

esp_err_t gyroscopeFullReset();

esp_err_t gyroscopeResetPaths(uint8_t paths);

esp_err_t gyroscopeSetSleepEnable(bool enable);

esp_err_t gyroscopeGetSampleRateDevisor(uint8_t* devisor);
esp_err_t gyroscopeSetSampleRateDevisor(uint8_t devisor);

esp_err_t gyroscopeGetFilterBandwidth(uint8_t* bandwidth);
esp_err_t gyroscopeSetFilterBandwidth(uint8_t bandwidth);

esp_err_t gyroscopeGetGyroRange(uint8_t* range);
esp_err_t gyroscopeSetGyroRange(uint8_t range);

esp_err_t gyroscopeGetAccelRange(uint8_t* range);
esp_err_t gyroscopeSetAccelRange(uint8_t range);

// output in G (output * GY_SENSOR_GRAVITY => output in m/s^2)
esp_err_t gyroscopeGetAcceleration(float* x, float* y, float* z);

// output in degrees Celsius
esp_err_t gyroscopeGetTemperature(float* t);

// output in deg/s
esp_err_t gyroscopeGetRotation(float* x, float* y, float* z);

esp_err_t gyroscopeGetAll(float* xa, float* ya, float* za, float* xg, float* yg, float* zg, float* t);

#endif  // GYROSCOPE_H
