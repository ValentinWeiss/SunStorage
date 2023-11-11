#include "driver/barometer.h"

#include <freertos/semphr.h>
#include <rom/ets_sys.h>

SemaphoreHandle_t barometerMutex = NULL;

typedef struct {
    int16_t AC1;
    int16_t AC2;
    int16_t AC3;
    uint16_t AC4;
    uint16_t AC5;
    uint16_t AC6;
    int16_t B1;
    int16_t B2;
    int16_t MB;
    int16_t MC;
    int16_t MD;
} bmp_calibration_data_t;

bmp_calibration_data_t bmpCalData = {
    .AC1 = 0,
    .AC2 = 0,
    .AC3 = 0,
    .AC4 = 0,
    .AC5 = 0,
    .AC6 = 0,
    .B1 = 0,
    .B2 = 0,
    .MB = 0,
    .MC = 0,
    .MD = 0,
};

esp_err_t barometerTestConnection() {
    esp_err_t errCode = ESP_OK;
    uint8_t id = 0;

    if (xSemaphorePoll(barometerMutex) != pdTRUE) {
        return ESP_FAIL;
    }

    errCode = i2cReadRegFromSlave(BAROMETER_ADDR, BMP_IDR, &id, 1, true);
    if (errCode != ESP_OK) {
        xSemaphoreGive(barometerMutex);
        return errCode;
    }

    if (id != BMP_CHIP_ID) {
        xSemaphoreGive(barometerMutex);
        return ESP_FAIL;
    }

    xSemaphoreGive(barometerMutex);
    return errCode;
}

esp_err_t barometerReset() {
    esp_err_t errCode = ESP_OK;
    uint8_t msg[2] = {0};

    if (xSemaphorePoll(barometerMutex) != pdTRUE) {
        return ESP_FAIL;
    }

    msg[0] = BMP_RST;
    msg[1] = BMP_POWER_ON_RESET;
    errCode = i2cWriteToSlave(BAROMETER_ADDR, msg, 2, true);
    if (errCode != ESP_OK) {
        xSemaphoreGive(barometerMutex);
        return errCode;
    }

    xSemaphoreGive(barometerMutex);
    return errCode;
}

esp_err_t barometerInit() {
    esp_err_t errCode = ESP_OK;
    uint8_t data[2] = {0};

    barometerMutex = xSemaphoreCreateMutex();
    if (!barometerMutex) {
        return ESP_FAIL;
    }

    // set AC1
    errCode = i2cReadRegFromSlave(BAROMETER_ADDR, BMP_CAL_AC1H, data, 2, true);
    if (errCode != ESP_OK) {
        return errCode;
    }

    bmpCalData.AC1 = ((int16_t)data[0] << 8) | data[1];

    // set AC2
    errCode = i2cReadRegFromSlave(BAROMETER_ADDR, BMP_CAL_AC2H, data, 2, true);
    if (errCode != ESP_OK) {
        return errCode;
    }

    bmpCalData.AC2 = ((int16_t)data[0] << 8) | data[1];

    // set AC3
    errCode = i2cReadRegFromSlave(BAROMETER_ADDR, BMP_CAL_AC3H, data, 2, true);
    if (errCode != ESP_OK) {
        return errCode;
    }

    bmpCalData.AC3 = ((int16_t)data[0] << 8) | data[1];

    // set AC4
    errCode = i2cReadRegFromSlave(BAROMETER_ADDR, BMP_CAL_AC4H, data, 2, true);
    if (errCode != ESP_OK) {
        return errCode;
    }

    bmpCalData.AC4 = ((uint16_t)data[0] << 8) | data[1];

    // set AC5
    errCode = i2cReadRegFromSlave(BAROMETER_ADDR, BMP_CAL_AC5H, data, 2, true);
    if (errCode != ESP_OK) {
        return errCode;
    }

    bmpCalData.AC5 = ((uint16_t)data[0] << 8) | data[1];

    // set AC6
    errCode = i2cReadRegFromSlave(BAROMETER_ADDR, BMP_CAL_AC6H, data, 2, true);
    if (errCode != ESP_OK) {
        return errCode;
    }

    bmpCalData.AC6 = ((uint16_t)data[0] << 8) | data[1];

    // set B1
    errCode = i2cReadRegFromSlave(BAROMETER_ADDR, BMP_CAL_B1H, data, 2, true);
    if (errCode != ESP_OK) {
        return errCode;
    }

    bmpCalData.B1 = ((int16_t)data[0] << 8) | data[1];

    // set B2
    errCode = i2cReadRegFromSlave(BAROMETER_ADDR, BMP_CAL_B2H, data, 2, true);
    if (errCode != ESP_OK) {
        return errCode;
    }

    bmpCalData.B2 = ((int16_t)data[0] << 8) | data[1];

    // set MB
    errCode = i2cReadRegFromSlave(BAROMETER_ADDR, BMP_CAL_MBH, data, 2, true);
    if (errCode != ESP_OK) {
        return errCode;
    }

    bmpCalData.MB = ((int16_t)data[0] << 8) | data[1];

    // set MC
    errCode = i2cReadRegFromSlave(BAROMETER_ADDR, BMP_CAL_MCH, data, 2, true);
    if (errCode != ESP_OK) {
        return errCode;
    }

    bmpCalData.MC = ((int16_t)data[0] << 8) | data[1];

    // set MD
    errCode = i2cReadRegFromSlave(BAROMETER_ADDR, BMP_CAL_MDH, data, 2, true);
    if (errCode != ESP_OK) {
        return errCode;
    }

    bmpCalData.MD = ((int16_t)data[0] << 8) | data[1];

    // check calibration data for 0x0000 or 0xFFFF
    if ((bmpCalData.AC1 == 0) || (bmpCalData.AC1 == -1) || (bmpCalData.AC2 == 0) || (bmpCalData.AC2 == -1) || (bmpCalData.AC3 == 0) || (bmpCalData.AC3 == -1) || (bmpCalData.AC4 == 0) || (bmpCalData.AC4 == (uint8_t)-1) || (bmpCalData.AC5 == 0) || (bmpCalData.AC5 == (uint8_t)-1) || (bmpCalData.AC6 == 0) || (bmpCalData.AC6 == (uint8_t)-1) || (bmpCalData.B1 == 0) || (bmpCalData.B1 == -1) || (bmpCalData.B2 == 0) || (bmpCalData.B2 == -1) || (bmpCalData.MB == 0) || (bmpCalData.MB == -1) || (bmpCalData.MC == 0) || (bmpCalData.MC == -1) || (bmpCalData.MD == 0) || (bmpCalData.MD == -1)) {
        return ESP_FAIL;
    }

    return errCode;
}

// temperature in degrees Celsius and pressure in Pa
esp_err_t barometerMeasure(float* temperature, int32_t* pressure) {
    esp_err_t errCode = ESP_OK;
    uint8_t data[3] = {0};

    if (xSemaphorePoll(barometerMutex) != pdTRUE) {
        return ESP_FAIL;
    }

    // get temperature
    int32_t T, P;
    int32_t UT = 0, X1, X2, B5;

    // start temperature measurement
    data[0] = BMP_CR;
    data[1] = BMP_MEAS_CTRL_TEMP;
    errCode = i2cWriteToSlave(BAROMETER_ADDR, data, 2, true);
    if (errCode != ESP_OK) {
        xSemaphoreGive(barometerMutex);
        return errCode;
    }

    ets_delay_us(BMP_MEASUREMENT_DELAY_US);

    // read measured temperature
    errCode = i2cReadRegFromSlave(BAROMETER_ADDR, BMP_DOMSB, data, 2, true);
    if (errCode != ESP_OK) {
        xSemaphoreGive(barometerMutex);
        return errCode;
    }

    UT = ((int16_t)data[0] << 8) | data[1];

    X1 = ((UT - (int32_t)bmpCalData.AC6) * (int32_t)bmpCalData.AC5) >> 15;
    X2 = ((int32_t)bmpCalData.MC << 11) / (X1 + (int32_t)bmpCalData.MD);
    B5 = X1 + X2;
    T = (B5 + 8) >> 4;

    if (temperature) {
        *temperature = T / 10.0;
    }

    if (pressure) {
        int32_t X3, B3, B6;
        uint32_t B4, B7, UP = 0;

        // start pressure measurement
        data[0] = BMP_CR;
        data[1] = BMP_MEAS_CTRL_PRES | (BMP_OVER_SAMPLE_RATIO << 6);
        errCode = i2cWriteToSlave(BAROMETER_ADDR, data, 2, true);
        if (errCode != ESP_OK) {
            xSemaphoreGive(barometerMutex);
            return errCode;
        }

        ets_delay_us(BMP_MEASUREMENT_DELAY_US);

        // read measured pressure
        errCode = i2cReadRegFromSlave(BAROMETER_ADDR, BMP_DOMSB, data, 3, true);
        if (errCode != ESP_OK) {
            xSemaphoreGive(barometerMutex);
            return errCode;
        }

        UP = ((uint32_t)data[0] << 16) | ((uint32_t)data[1] << 8) | data[2];
        UP >>= (8 - BMP_OVER_SAMPLE_RATIO);

        B6 = B5 - 4000;
        X1 = ((int32_t)bmpCalData.B2 * ((B6 * B6) >> 12)) >> 11;
        X2 = ((int32_t)bmpCalData.AC2 * B6) >> 11;
        X3 = X1 + X2;

        B3 = ((((int32_t)bmpCalData.AC1 * 4 + X3) << BMP_OVER_SAMPLE_RATIO) + 2) >> 2;
        X1 = ((int32_t)bmpCalData.AC3 * B6) >> 13;
        X2 = ((int32_t)bmpCalData.B1 * ((B6 * B6) >> 12)) >> 16;
        X3 = ((X1 + X2) + 2) >> 2;
        B4 = ((uint32_t)bmpCalData.AC4 * (uint32_t)(X3 + 32768)) >> 15;
        B7 = ((uint32_t)UP - B3) * (uint32_t)(50000UL >> BMP_OVER_SAMPLE_RATIO);

        if (B7 < 0x80000000UL)
            P = (B7 * 2) / B4;
        else
            P = (B7 / B4) * 2;

        X1 = (P >> 8) * (P >> 8);
        X1 = (X1 * 3038) >> 16;
        X2 = (-7357 * P) >> 16;
        P = P + ((X1 + X2 + (int32_t)3791) >> 4);

        *pressure = P;
    }

    xSemaphoreGive(barometerMutex);
    return errCode;
}
