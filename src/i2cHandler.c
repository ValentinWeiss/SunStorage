#include "i2cHandler.h"

esp_err_t i2cInit() {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master = {
            .clk_speed = I2C_MASTER_FREQ_HZ,
        },
        .clk_flags = 0,
    };

    esp_err_t errCode = i2c_param_config(I2C_MASTER_PORT, &conf);
    if (errCode != ESP_OK) {
        return errCode;
    }

    errCode = i2c_driver_install(I2C_MASTER_PORT, I2C_MODE_MASTER, 0, 0, ESP_INTR_FLAG_LEVEL1);
    if (errCode != ESP_OK) {
        return errCode;
    }

    return ESP_OK;
}

esp_err_t i2cReadFromSlave(uint8_t slaveAddress, uint8_t* data, size_t len, bool ackEnable) {
    // data must not be nullptr
    if (!data) {
        return ESP_FAIL;
    }

    i2c_cmd_handle_t cmdHandle = i2c_cmd_link_create();

    esp_err_t errCode = i2c_master_start(cmdHandle);
    if (errCode != ESP_OK) {
        i2c_cmd_link_delete(cmdHandle);
        return errCode;
    }

    errCode = i2c_master_write_byte(cmdHandle, (slaveAddress << 1) | I2C_MASTER_READ, ackEnable);
    if (errCode != ESP_OK) {
        i2c_cmd_link_delete(cmdHandle);
        return errCode;
    }

    errCode = i2c_master_read(cmdHandle, data, len, I2C_MASTER_LAST_NACK);
    if (errCode != ESP_OK) {
        i2c_cmd_link_delete(cmdHandle);
        return errCode;
    }

    errCode = i2c_master_stop(cmdHandle);
    if (errCode != ESP_OK) {
        i2c_cmd_link_delete(cmdHandle);
        return errCode;
    }

    errCode = i2c_master_cmd_begin(I2C_MASTER_PORT, cmdHandle, I2C_MS_TO_WAIT / portTICK_PERIOD_MS);
    if (errCode != ESP_OK) {
        i2c_cmd_link_delete(cmdHandle);
        return errCode;
    }

    i2c_cmd_link_delete(cmdHandle);

    return ESP_OK;
}

esp_err_t i2cWriteToSlave(uint8_t slaveAddress, uint8_t* data, size_t len, bool ackEnable) {
    // data must not be nullptr
    if (!data) {
        return ESP_FAIL;
    }

    i2c_cmd_handle_t cmdHandle = i2c_cmd_link_create();

    esp_err_t errCode = i2c_master_start(cmdHandle);
    if (errCode != ESP_OK) {
        i2c_cmd_link_delete(cmdHandle);
        return errCode;
    }

    errCode = i2c_master_write_byte(cmdHandle, (slaveAddress << 1) | I2C_MASTER_WRITE, ackEnable);
    if (errCode != ESP_OK) {
        i2c_cmd_link_delete(cmdHandle);
        return errCode;
    }

    errCode = i2c_master_write(cmdHandle, data, len, ackEnable);
    if (errCode != ESP_OK) {
        i2c_cmd_link_delete(cmdHandle);
        return errCode;
    }

    errCode = i2c_master_stop(cmdHandle);
    if (errCode != ESP_OK) {
        i2c_cmd_link_delete(cmdHandle);
        return errCode;
    }

    errCode = i2c_master_cmd_begin(I2C_MASTER_PORT, cmdHandle, I2C_MS_TO_WAIT / portTICK_PERIOD_MS);
    if (errCode != ESP_OK) {
        i2c_cmd_link_delete(cmdHandle);
        return errCode;
    }

    i2c_cmd_link_delete(cmdHandle);

    return ESP_OK;
}

esp_err_t i2cReadRegFromSlave(uint8_t slaveAddress, uint8_t reg, uint8_t* data, size_t len, bool ackEnable) {
    esp_err_t errCode = ESP_OK;

    errCode = i2cWriteToSlave(slaveAddress, &reg, 1, ackEnable);
    if (errCode != ESP_OK) {
        return errCode;
    }

    errCode = i2cReadFromSlave(slaveAddress, data, len, ackEnable);
    if (errCode != ESP_OK) {
        return errCode;
    }

    return errCode;
}
