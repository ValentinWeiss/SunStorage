#ifndef I2CHANDLER_H
#define I2CHANDLER_H

#include "driver/i2c.h"
#include "freertos/timers.h"
#include "settings.h"

esp_err_t i2cInit();

esp_err_t i2cReadFromSlave(uint8_t slaveAddress, uint8_t* data, size_t len, bool ackEnable);

esp_err_t i2cWriteToSlave(uint8_t slaveAddress, uint8_t* data, size_t len, bool ackEnable);

esp_err_t i2cReadRegFromSlave(uint8_t slaveAddress, uint8_t reg, uint8_t* data, size_t len, bool ackEnable);

#endif  // I2CHANDLER_H
