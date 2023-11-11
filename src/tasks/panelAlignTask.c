#include "tasks/panelAlignTask.h"

#include "initTracker.h"
#include "servo.h"
#include "suntracking.h"

void panelAlignTask() {
    float azimuth;
    float elevation;
    float xGyro;
    float yGyro;
    float zGyro;
    float angleRotate;
    float angleTilt;
    while (1) {
        if (servoIsInitialized == true) {
            azimuth = getEsp2Azimuth();
            elevation = getEsp2Elevation();
            xGyro = getEsp2xGyro();
            yGyro = getEsp2yGyro();
            zGyro = getEsp2zGyro();
            ESP_LOGI("", "azi: %d ele: %d xg: %f yg: %f zg: %f\n", (int16_t)azimuth, (int16_t)elevation, -xGyro, -yGyro, zGyro);
            calcServoPos(xGyro, yGyro, zGyro, azimuth, elevation, &angleRotate, &angleTilt);
            setSunpos((int16_t)angleRotate, (int16_t)angleTilt);
        } else {
            ESP_LOGE("panelAlignTask", "servo not initialized");
        }
        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
}
