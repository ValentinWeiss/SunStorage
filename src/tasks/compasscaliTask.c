#include "tasks/compasscaliTask.h"

#include "driver/compass.h"
#include "initTracker.h"
#include "sunUtils.h"

void compasscaliTask() {
    int16_t xCompass, yCompass, zCompass;
    int16_t calibrationData[3][2] = {{1642, -8082}, {5622, -6640}, {5245, -6122}};
    calibrationData[0][0] = -32768;
    calibrationData[1][0] = -32768;
    calibrationData[2][0] = -32768;
    calibrationData[0][1] = 32767;
    calibrationData[1][1] = 32767;
    calibrationData[2][1] = 32767;
    esp_err_t errCode = ESP_OK;
    while (1) {
        if (compassModuleIsInitialized) {
            // compass data
            errCode = compassModuleChangeMode(CM_MODE_CONTINUOUS_MEASUREMENT);
            if (errCode != ESP_OK) {
                ESP_LOGE("sunAligntask", "mode change failed");
            } else {
                errCode = compassModuleReadAxis(&xCompass, &yCompass, &zCompass);
                if (errCode != ESP_OK) {
                    ESP_LOGE("sunAlign_task", "read axis failed");
                }
            }
            errCode = compassModuleChangeMode(CM_MODE_STANDBY);
            if (errCode != ESP_OK) {
                ESP_LOGE("sunAlign_task", "mode change failed");
            }
        }
        if (!compassModuleIsInitialized || errCode != ESP_OK) {
            ESP_LOGE("sunAlignTask", "no live compass data");
            errCode = ESP_OK;
        }

        if (xCompass > calibrationData[0][0]) {
            calibrationData[0][0] = xCompass;
        }
        if (xCompass < calibrationData[0][1]) {
            calibrationData[0][1] = xCompass;
        }
        if (yCompass > calibrationData[1][0]) {
            calibrationData[1][0] = yCompass;
        }
        if (yCompass < calibrationData[1][1]) {
            calibrationData[1][1] = yCompass;
        }
        if (zCompass > calibrationData[2][0]) {
            calibrationData[2][0] = zCompass;
        }
        if (zCompass < calibrationData[2][1]) {
            calibrationData[2][1] = zCompass;
        }
        printf("calidata:\n");
        printf("max: %i %i %i\n", (int)calibrationData[0][0], (int)calibrationData[1][0], (int)calibrationData[2][0]);
        printf("min: %i %i %i\n", (int)calibrationData[0][1], (int)calibrationData[1][1], (int)calibrationData[2][1]);
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}