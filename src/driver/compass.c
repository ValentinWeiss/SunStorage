#include "driver/compass.h"

#include <freertos/semphr.h>

SemaphoreHandle_t compassMutex = NULL;

esp_err_t compassModuleInit() {
    esp_err_t errCode = ESP_OK;
    uint8_t msg[2] = {0};

    compassMutex = xSemaphoreCreateMutex();
    if (!compassMutex) {
        return ESP_FAIL;
    }

    // set 0x0B to 0x01 (recommended)
    msg[0] = CM_FBR;
    msg[1] = 0x01;
    errCode = i2cWriteToSlave(COMPASS_MODULE_ADDR, msg, 2, true);
    if (errCode != ESP_OK) {
        return errCode;
    }

    // set 0x09
    msg[0] = CM_CR0;
    msg[1] = (CM_OVER_SAMPLE_RATIO << 6) | (CM_MAGNETIC_FIELD_RANGE << 4) | (CM_UPDATE_DATA_RATE << 2) | CM_MODE_STANDBY;
    errCode = i2cWriteToSlave(COMPASS_MODULE_ADDR, msg, 2, true);
    if (errCode != ESP_OK) {
        return errCode;
    }

    // set 0x0A
    msg[0] = CM_CR1;
    msg[1] = (CM_ROL_PNT << 6) | CM_INT_DISABLE;
    errCode = i2cWriteToSlave(COMPASS_MODULE_ADDR, msg, 2, true);
    if (errCode != ESP_OK) {
        return errCode;
    }

    return errCode;
}

esp_err_t compassModuleReset() {
    esp_err_t errCode = ESP_OK;
    uint8_t msg[2] = {0};

    if (xSemaphorePoll(compassMutex) != pdTRUE) {
        return ESP_FAIL;
    }

    // set bit 7 of 0x0A to 1
    msg[0] = CM_CR1;
    msg[1] = (1 << 7);
    errCode = i2cWriteToSlave(COMPASS_MODULE_ADDR, msg, 2, true);
    if (errCode != ESP_OK) {
        xSemaphoreGive(compassMutex);
        return errCode;
    }

    xSemaphoreGive(compassMutex);
    return errCode;
}

esp_err_t compassModuleChangeMode(uint8_t mode) {
    esp_err_t errCode = ESP_OK;
    uint8_t msg[2] = {0};

    if (xSemaphorePoll(compassMutex) != pdTRUE) {
        return ESP_FAIL;
    }

    msg[0] = CM_CR0;
    msg[1] = (CM_OVER_SAMPLE_RATIO << 6) | (CM_MAGNETIC_FIELD_RANGE << 4) | (CM_UPDATE_DATA_RATE << 2) | mode;  //! expects init state
    errCode = i2cWriteToSlave(COMPASS_MODULE_ADDR, msg, 2, true);
    if (errCode != ESP_OK) {
        xSemaphoreGive(compassMutex);
        return errCode;
    }

    xSemaphoreGive(compassMutex);
    return errCode;
}

esp_err_t compassModuleReadAxis(int16_t* x, int16_t* y, int16_t* z) {
    esp_err_t errCode = ESP_OK;
    uint8_t startReg = CM_DOXL;
    uint8_t data[6] = {0};

    if (xSemaphorePoll(compassMutex) != pdTRUE) {
        return ESP_FAIL;
    }

    errCode = i2cReadRegFromSlave(COMPASS_MODULE_ADDR, startReg, data, 6, true);
    if (errCode != ESP_OK) {
        xSemaphoreGive(compassMutex);
        return errCode;
    }

    if (x) {
        *x = ((int16_t)data[1] << 8) | data[0];
    }
    if (y) {
        *y = ((int16_t)data[3] << 8) | data[2];
    }
    if (z) {
        *z = ((int16_t)data[5] << 8) | data[4];
    }

    xSemaphoreGive(compassMutex);
    return errCode;
}

esp_err_t compassModuleReadTemperature(int16_t* t) {
    esp_err_t errCode = ESP_OK;
    uint8_t startReg = CM_TOL;
    uint8_t data[2] = {0};

    if (xSemaphorePoll(compassMutex) != pdTRUE) {
        return ESP_FAIL;
    }

    errCode = i2cReadRegFromSlave(COMPASS_MODULE_ADDR, startReg, data, 2, true);
    if (errCode != ESP_OK) {
        xSemaphoreGive(compassMutex);
        return errCode;
    }

    if (t) {
        *t = ((int16_t)data[1] << 8) | data[0];
    }

    xSemaphoreGive(compassMutex);
    return errCode;
}
