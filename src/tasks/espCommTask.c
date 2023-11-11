#include "tasks/espCommTask.h"

#include "cJSON.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "settings.h"
#include "state.h"

void espCommReceiveTask() {
    esp_err_t errCode = ESP_OK;

    char readBuffer[COM_BUFFER] = {0};

    char jsonBuffer[COM_BUFFER + 1] = {0};
    char* jsonBufferPosPtr = jsonBuffer;

    size_t bufferedDataLen = 0;
    bool bufferOverflowDetected = false;

    while (1) {
        errCode = uart_get_buffered_data_len(COM_RECEIVER_NUM, &bufferedDataLen);
        if (errCode != ESP_OK) {
            ESP_LOGE("esp_comm_receive_task", "failed to get buffered data length");
        } else {
            if (bufferedDataLen == 0) {
                ESP_LOGI("esp_comm_receive_task", "nothing received");
            } else {
                int len = uart_read_bytes(COM_RECEIVER_NUM, readBuffer, bufferedDataLen, 100);
                if (len < 0) {
                    ESP_LOGE("esp_comm_receive_task", "failed to read bytes");
                } else {
                    for (int i = 0; i < len; i++) {
                        // copy char to jsonBuffer
                        *jsonBufferPosPtr = readBuffer[i];
                        jsonBufferPosPtr++;

                        // if end of line finalize json and clear buffer
                        if (readBuffer[i] == '\n') {
                            *jsonBufferPosPtr = '\0';

                            // discard if overflow detected
                            if (!bufferOverflowDetected) {
                                cJSON* state = cJSON_Parse(jsonBuffer);
                                setState(state);
                                cJSON_Delete(state);
                                ESP_LOGI("esp_comm_receive_task", "STATE SAVED SUCCESSFULLY");
                            }

                            jsonBufferPosPtr = jsonBuffer;
                        }

                        // if jsonBufferPosPtr goes out of scope signalize buffer overflow
                        if (jsonBufferPosPtr >= &jsonBuffer[COM_BUFFER]) {
                            ESP_LOGE("esp_comm_receive_task", "jsonBuffer overflow");
                            bufferOverflowDetected = true;
                            jsonBufferPosPtr = jsonBuffer;
                        }
                    }
                }
            }
        }

        vTaskDelay(ESP_COMM_RECEIVE_TASK_DELAY);
    }
}
