#include "driver/gyroscope.h"

#include <freertos/semphr.h>

SemaphoreHandle_t gyroscopeMutex = NULL;

esp_err_t gyroscopeTestConnection() {
    esp_err_t errCode = ESP_OK;

    if (xSemaphorePoll(gyroscopeMutex) != pdTRUE) {
        return ESP_FAIL;
    }

    uint8_t retData;
    errCode = i2cReadRegFromSlave(GYROSCOPE_ADDR, GY_WHO_AM_I, &retData, 1, true);
    if (errCode != ESP_OK) {
        xSemaphoreGive(gyroscopeMutex);
        return errCode;
    }

    if ((retData & GYROSCOPE_ADDR) != GYROSCOPE_ADDR_LOW) {
        xSemaphoreGive(gyroscopeMutex);
        return ESP_FAIL;
    }

    xSemaphoreGive(gyroscopeMutex);
    return errCode;
}

esp_err_t gyroscopeInit() {
    esp_err_t errCode = ESP_OK;
    uint8_t msg[2] = {0};

    gyroscopeMutex = xSemaphoreCreateMutex();
    if (!gyroscopeMutex) {
        return ESP_FAIL;
    }

    // reset module and signal paths
    errCode = gyroscopeFullReset();
    if (errCode != ESP_OK) {
        return errCode;
    }

    vTaskDelay(200 / portTICK_PERIOD_MS);

    errCode = gyroscopeResetPaths(GY_RST_GYRO_PATH | GY_RST_TEMP_PATH | GY_RST_ACCEL_PATH);
    if (errCode != ESP_OK) {
        return errCode;
    }

    vTaskDelay(100 / portTICK_PERIOD_MS);

    // initialize
    errCode = gyroscopeSetSampleRateDevisor(0);
    if (errCode != ESP_OK) {
        return errCode;
    }
    errCode = gyroscopeSetFilterBandwidth(GY_BAND_260_HZ);
    if (errCode != ESP_OK) {
        return errCode;
    }
    errCode = gyroscopeSetGyroRange(GY_GYRO_RANGE_500_DEG);
    if (errCode != ESP_OK) {
        return errCode;
    }
    errCode = gyroscopeSetAccelRange(GY_ACCEL_RANGE_2_G);
    if (errCode != ESP_OK) {
        return errCode;
    }

    // set clock to PLL with Gyro X reference
    msg[0] = GY_PWR_MGMT_1;
    msg[1] = 0x01;
    errCode = i2cWriteToSlave(GYROSCOPE_ADDR, msg, 2, true);
    if (errCode != ESP_OK) {
        return errCode;
    }

    vTaskDelay(100 / portTICK_PERIOD_MS);

    return errCode;
}

esp_err_t gyroscopeFullReset() {
    esp_err_t errCode = ESP_OK;
    uint8_t msg[2] = {0};

    if (xSemaphorePoll(gyroscopeMutex) != pdTRUE) {
        return ESP_FAIL;
    }

    msg[0] = GY_PWR_MGMT_1;
    msg[1] = 0x80;
    errCode = i2cWriteToSlave(GYROSCOPE_ADDR, msg, 2, true);
    if (errCode != ESP_OK) {
        xSemaphoreGive(gyroscopeMutex);
        return errCode;
    }

    xSemaphoreGive(gyroscopeMutex);
    return errCode;
}

esp_err_t gyroscopeResetPaths(uint8_t paths) {
    esp_err_t errCode = ESP_OK;
    uint8_t msg[2] = {0};

    if (xSemaphorePoll(gyroscopeMutex) != pdTRUE) {
        return ESP_FAIL;
    }

    msg[0] = GY_SIGNAL_PATH_RESET;
    msg[1] = paths;
    errCode = i2cWriteToSlave(GYROSCOPE_ADDR, msg, 2, true);
    if (errCode != ESP_OK) {
        xSemaphoreGive(gyroscopeMutex);
        return errCode;
    }

    xSemaphoreGive(gyroscopeMutex);
    return errCode;
}

esp_err_t gyroscopeSetSleepEnable(bool enable) {
    esp_err_t errCode = ESP_OK;

    if (xSemaphorePoll(gyroscopeMutex) != pdTRUE) {
        return ESP_FAIL;
    }

    uint8_t pwrMgmt1 = 0;

    errCode = i2cReadRegFromSlave(GYROSCOPE_ADDR, GY_PWR_MGMT_1, &pwrMgmt1, 1, true);
    if (errCode != ESP_OK) {
        xSemaphoreGive(gyroscopeMutex);
        return errCode;
    }

    pwrMgmt1 = pwrMgmt1 & 0xBF;
    if (enable) {
        pwrMgmt1 |= 0x40;
    }

    uint8_t msg[2] = {GY_PWR_MGMT_1, pwrMgmt1};
    errCode = i2cWriteToSlave(GYROSCOPE_ADDR, msg, 2, true);

    xSemaphoreGive(gyroscopeMutex);
    return errCode;
}

esp_err_t gyroscopeGetSampleRateDevisor(uint8_t* devisor) {
    if (!devisor) {
        return ESP_FAIL;
    }

    if (xSemaphorePoll(gyroscopeMutex) != pdTRUE) {
        return ESP_FAIL;
    }

    esp_err_t errCode = i2cReadRegFromSlave(GYROSCOPE_ADDR, GY_SMPRT_DIV, devisor, 1, true);

    xSemaphoreGive(gyroscopeMutex);
    return errCode;
}
esp_err_t gyroscopeSetSampleRateDevisor(uint8_t devisor) {
    uint8_t msg[2] = {GY_SMPRT_DIV, devisor};

    if (xSemaphorePoll(gyroscopeMutex) != pdTRUE) {
        return ESP_FAIL;
    }

    esp_err_t errCode = i2cWriteToSlave(GYROSCOPE_ADDR, msg, 2, true);

    xSemaphoreGive(gyroscopeMutex);
    return errCode;
}

esp_err_t gyroscopeGetFilterBandwidth(uint8_t* bandwidth) {
    if (!bandwidth) {
        return ESP_FAIL;
    }

    if (xSemaphorePoll(gyroscopeMutex) != pdTRUE) {
        return ESP_FAIL;
    }

    uint8_t config = 0;

    esp_err_t errCode = i2cReadRegFromSlave(GYROSCOPE_ADDR, GY_CONFIG, &config, 1, true);
    if (errCode == ESP_OK) {
        *bandwidth = config & 0x07;
    }

    xSemaphoreGive(gyroscopeMutex);
    return errCode;
}
esp_err_t gyroscopeSetFilterBandwidth(uint8_t bandwidth) {
    esp_err_t errCode = ESP_OK;

    if (xSemaphorePoll(gyroscopeMutex) != pdTRUE) {
        return ESP_FAIL;
    }

    uint8_t config = 0;

    errCode = i2cReadRegFromSlave(GYROSCOPE_ADDR, GY_CONFIG, &config, 1, true);
    if (errCode != ESP_OK) {
        xSemaphoreGive(gyroscopeMutex);
        return errCode;
    }

    config = (config & 0xF8) | (bandwidth & 0x07);

    uint8_t msg[2] = {GY_CONFIG, config};
    errCode = i2cWriteToSlave(GYROSCOPE_ADDR, msg, 2, true);

    xSemaphoreGive(gyroscopeMutex);
    return errCode;
}

esp_err_t localGyroscopeGetGyroRange(uint8_t* range) {
    if (!range) {
        return ESP_FAIL;
    }

    uint8_t gyroConfig = 0;

    esp_err_t errCode = i2cReadRegFromSlave(GYROSCOPE_ADDR, GY_GYRO_CONFIG, &gyroConfig, 1, true);
    if (errCode == ESP_OK) {
        *range = gyroConfig & 0x18;
    }

    return errCode;
}

esp_err_t gyroscopeGetGyroRange(uint8_t* range) {
    if (xSemaphorePoll(gyroscopeMutex) != pdTRUE) {
        return ESP_FAIL;
    }

    esp_err_t errCode = localGyroscopeGetGyroRange(range);

    xSemaphoreGive(gyroscopeMutex);
    return errCode;
}

esp_err_t gyroscopeSetGyroRange(uint8_t range) {
    esp_err_t errCode = ESP_OK;

    if (xSemaphorePoll(gyroscopeMutex) != pdTRUE) {
        return ESP_FAIL;
    }

    uint8_t gyroConfig = 0;

    errCode = i2cReadRegFromSlave(GYROSCOPE_ADDR, GY_CONFIG, &gyroConfig, 1, true);
    if (errCode != ESP_OK) {
        xSemaphoreGive(gyroscopeMutex);
        return errCode;
    }

    gyroConfig = (gyroConfig & 0xE7) | (range & 0x18);

    uint8_t msg[2] = {GY_GYRO_CONFIG, gyroConfig};
    errCode = i2cWriteToSlave(GYROSCOPE_ADDR, msg, 2, true);

    xSemaphoreGive(gyroscopeMutex);
    return errCode;
}

esp_err_t localGyroscopeGetAccelRange(uint8_t* range) {
    if (!range) {
        return ESP_FAIL;
    }

    uint8_t accelConfig = 0;

    esp_err_t errCode = i2cReadRegFromSlave(GYROSCOPE_ADDR, GY_ACCEL_CONFIG, &accelConfig, 1, true);
    if (errCode == ESP_OK) {
        *range = accelConfig & 0x18;
    }

    return errCode;
}

esp_err_t gyroscopeGetAccelRange(uint8_t* range) {
    if (xSemaphorePoll(gyroscopeMutex) != pdTRUE) {
        return ESP_FAIL;
    }

    esp_err_t errCode = localGyroscopeGetAccelRange(range);

    xSemaphoreGive(gyroscopeMutex);
    return errCode;
}

esp_err_t gyroscopeSetAccelRange(uint8_t range) {
    esp_err_t errCode = ESP_OK;

    if (xSemaphorePoll(gyroscopeMutex) != pdTRUE) {
        return ESP_FAIL;
    }

    uint8_t accelConfig = 0;

    errCode = i2cReadRegFromSlave(GYROSCOPE_ADDR, GY_CONFIG, &accelConfig, 1, true);
    if (errCode != ESP_OK) {
        xSemaphoreGive(gyroscopeMutex);
        return errCode;
    }

    accelConfig = (accelConfig & 0xE7) | (range & 0x18);

    uint8_t msg[2] = {GY_GYRO_CONFIG, accelConfig};
    errCode = i2cWriteToSlave(GYROSCOPE_ADDR, msg, 2, true);

    xSemaphoreGive(gyroscopeMutex);
    return errCode;
}

esp_err_t gyroscopeGetAcceleration(float* x, float* y, float* z) {
    esp_err_t errCode = ESP_OK;
    uint8_t data[6] = {0};

    if (xSemaphorePoll(gyroscopeMutex) != pdTRUE) {
        return ESP_FAIL;
    }

    errCode = i2cReadRegFromSlave(GYROSCOPE_ADDR, GY_ACCEL_XOUT_H, data, 6, true);
    if (errCode != ESP_OK) {
        xSemaphoreGive(gyroscopeMutex);
        return errCode;
    }

    uint8_t accelRange = 0;
    errCode = localGyroscopeGetAccelRange(&accelRange);
    if (errCode != ESP_OK) {
        xSemaphoreGive(gyroscopeMutex);
        return errCode;
    }

    float accelScale = 1.0f;
    switch (accelRange) {
        case GY_ACCEL_RANGE_2_G:
            accelScale = 16384.0f;
            break;
        case GY_ACCEL_RANGE_4_G:
            accelScale = 8192.0f;
            break;
        case GY_ACCEL_RANGE_8_G:
            accelScale = 4096.0f;
            break;
        case GY_ACCEL_RANGE_16_G:
            accelScale = 2048.0f;
            break;
        default:
            xSemaphoreGive(gyroscopeMutex);
            return ESP_FAIL;
    }

    if (x) {
        int16_t rawX = ((int16_t)data[0] << 8) | data[1];
        *x = ((float)rawX) / accelScale;
    }
    if (y) {
        int16_t rawY = ((int16_t)data[2] << 8) | data[3];
        *y = ((float)rawY) / accelScale;
    }
    if (z) {
        int16_t rawZ = ((int16_t)data[4] << 8) | data[5];
        *z = ((float)rawZ) / accelScale;
    }

    xSemaphoreGive(gyroscopeMutex);
    return errCode;
}

esp_err_t gyroscopeGetTemperature(float* t) {
    esp_err_t errCode = ESP_OK;
    uint8_t data[2] = {0};

    if (xSemaphorePoll(gyroscopeMutex) != pdTRUE) {
        return ESP_FAIL;
    }

    errCode = i2cReadRegFromSlave(GYROSCOPE_ADDR, GY_TEMP_OUT_H, data, 2, true);
    if (errCode != ESP_OK) {
        xSemaphoreGive(gyroscopeMutex);
        return errCode;
    }

    if (t) {
        int16_t rawTemp = ((int16_t)data[0] << 8) | data[1];
        *t = ((float)rawTemp / 340.0f) + 36.53f;
    }

    xSemaphoreGive(gyroscopeMutex);
    return errCode;
}

esp_err_t gyroscopeGetRotation(float* x, float* y, float* z) {
    esp_err_t errCode = ESP_OK;
    uint8_t data[6] = {0};

    if (xSemaphorePoll(gyroscopeMutex) != pdTRUE) {
        return ESP_FAIL;
    }

    errCode = i2cReadRegFromSlave(GYROSCOPE_ADDR, GY_GYRO_XOUT_H, data, 6, true);
    if (errCode != ESP_OK) {
        xSemaphoreGive(gyroscopeMutex);
        return errCode;
    }

    uint8_t gyroRange = 0;
    errCode = localGyroscopeGetGyroRange(&gyroRange);
    if (errCode != ESP_OK) {
        xSemaphoreGive(gyroscopeMutex);
        return errCode;
    }

    float gyroScale = 1.0f;
    switch (gyroRange) {
        case GY_GYRO_RANGE_250_DEG:
            gyroScale = 131.0f;
            break;
        case GY_GYRO_RANGE_500_DEG:
            gyroScale = 65.5f;
            break;
        case GY_GYRO_RANGE_1000_DEG:
            gyroScale = 32.8f;
            break;
        case GY_GYRO_RANGE_2000_DEG:
            gyroScale = 16.4f;
            break;
        default:
            xSemaphoreGive(gyroscopeMutex);
            return ESP_FAIL;
    }

    if (x) {
        int16_t rawX = ((int16_t)data[0] << 8) | data[1];
        *x = ((float)rawX) / gyroScale;
    }
    if (y) {
        int16_t rawY = ((int16_t)data[2] << 8) | data[3];
        *y = ((float)rawY) / gyroScale;
    }
    if (z) {
        int16_t rawZ = ((int16_t)data[4] << 8) | data[5];
        *z = ((float)rawZ) / gyroScale;
    }

    xSemaphoreGive(gyroscopeMutex);
    return errCode;
}

esp_err_t gyroscopeGetAll(float* xa, float* ya, float* za, float* xg, float* yg, float* zg, float* t) {
    esp_err_t errCode = ESP_OK;
    uint8_t data[14] = {0};

    if (xSemaphorePoll(gyroscopeMutex) != pdTRUE) {
        return ESP_FAIL;
    }

    errCode = i2cReadRegFromSlave(GYROSCOPE_ADDR, GY_ACCEL_XOUT_H, data, 14, true);
    if (errCode != ESP_OK) {
        xSemaphoreGive(gyroscopeMutex);
        return errCode;
    }

    uint8_t accelRange = 0;
    errCode = localGyroscopeGetAccelRange(&accelRange);
    if (errCode != ESP_OK) {
        xSemaphoreGive(gyroscopeMutex);
        return errCode;
    }

    float accelScale = 1.0f;
    switch (accelRange) {
        case GY_ACCEL_RANGE_2_G:
            accelScale = 16384.0f;
            break;
        case GY_ACCEL_RANGE_4_G:
            accelScale = 8192.0f;
            break;
        case GY_ACCEL_RANGE_8_G:
            accelScale = 4096.0f;
            break;
        case GY_ACCEL_RANGE_16_G:
            accelScale = 2048.0f;
            break;
        default:
            xSemaphoreGive(gyroscopeMutex);
            return ESP_FAIL;
    }

    if (xa) {
        int16_t rawX = ((int16_t)data[0] << 8) | data[1];
        *xa = ((float)rawX) / accelScale;
    }
    if (ya) {
        int16_t rawY = ((int16_t)data[2] << 8) | data[3];
        *ya = ((float)rawY) / accelScale;
    }
    if (za) {
        int16_t rawZ = ((int16_t)data[4] << 8) | data[5];
        *za = ((float)rawZ) / accelScale;
    }

    if (t) {
        int16_t rawTemp = ((int16_t)data[6] << 8) | data[7];
        *t = ((float)rawTemp / 340.0f) + 36.53f;
    }

    uint8_t gyroRange = 0;
    errCode = localGyroscopeGetGyroRange(&gyroRange);
    if (errCode != ESP_OK) {
        xSemaphoreGive(gyroscopeMutex);
        return errCode;
    }

    float gyroScale = 1.0f;
    switch (gyroRange) {
        case GY_GYRO_RANGE_250_DEG:
            gyroScale = 131.0f;
            break;
        case GY_GYRO_RANGE_500_DEG:
            gyroScale = 65.5f;
            break;
        case GY_GYRO_RANGE_1000_DEG:
            gyroScale = 32.8f;
            break;
        case GY_GYRO_RANGE_2000_DEG:
            gyroScale = 16.4f;
            break;
        default:
            xSemaphoreGive(gyroscopeMutex);
            return ESP_FAIL;
    }

    if (xg) {
        int16_t rawX = ((int16_t)data[8] << 8) | data[9];
        *xg = ((float)rawX) / gyroScale;
    }
    if (yg) {
        int16_t rawY = ((int16_t)data[10] << 8) | data[11];
        *yg = ((float)rawY) / gyroScale;
    }
    if (zg) {
        int16_t rawZ = ((int16_t)data[12] << 8) | data[13];
        *zg = ((float)rawZ) / gyroScale;
    }

    xSemaphoreGive(gyroscopeMutex);
    return errCode;
}
