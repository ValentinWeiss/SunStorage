#include "espComm.h"

esp_err_t initCommUART(uint8_t type) {
    esp_err_t res = ESP_OK;
    const uart_config_t uartConfig = {
        .baud_rate = COM_BAUDRATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .source_clk = UART_SCLK_DEFAULT,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE};
    if (type == 1) {
        res = uart_param_config(COM_RECEIVER_NUM, &uartConfig);

    } else {
        res = uart_param_config(COM_SENDER_NUM, &uartConfig);
    }

    if (res != ESP_OK) {
        ESP_LOGE("COMTX::INIT", "uart param config failed");
    }

    if (type == 1) {
        res = uart_set_pin(COM_RECEIVER_NUM, COM_RECEIVER_TX, COM_RECEIVER_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    } else {
        res = uart_set_pin(COM_SENDER_NUM, COM_SENDER_TX, COM_SENDER_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    }

    if (res != ESP_OK) {
        ESP_LOGE("COMTX::INIT", "UART set pin failed");
    }

    if (type == 1) {
        res = uart_driver_install(COM_RECEIVER_NUM, COM_BUFFER, 0, 10, NULL, 0);

    } else {
        res = uart_driver_install(COM_SENDER_NUM, COM_BUFFER, COM_BUFFER, 10, NULL, 0);
    }

    if (res != ESP_OK) {
        ESP_LOGE("COMTX::INIT", "UART driver installed failed");
    }

    return res;
}

esp_err_t espCommTransmit() {
    cJSON *state;
    getState(&state);
    char *output = cJSON_PrintUnformatted(state);
    size_t length = strlen(output);

    int len = uart_write_bytes(COM_SENDER_NUM, output, length);
    ESP_LOGI("", "Length: %d, JSON: %s\n", length, output);
    if (len <= 0) {
        ESP_LOGE("UART::TX", "Sent length is %d", len);
        return ESP_FAIL;
    }
    len = uart_write_bytes(COM_SENDER_NUM, "\n", 1);
    if (len <= 0) {
        ESP_LOGE("UART::TX", "Sent \\n length is %d", len);
        return ESP_FAIL;
    }
    ESP_LOGI("", "Sending done %d\n", len);
    cJSON_Delete(state);
    free(output);
    return ESP_OK;
}
