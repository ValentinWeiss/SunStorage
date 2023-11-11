#include <esp_log.h>
#include <esp_system.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "globalInit.h"

#ifdef TX
#include "tasks/adcReadTask.h"
#include "tasks/batteryChargerTask.h"
#include "tasks/compasscaliTask.h"
#include "tasks/dcf77Task.h"
#include "tasks/displayTask.h"
#include "tasks/socCalibrationTask.h"
#include "tasks/statsCalcTask.h"
#include "tasks/storeReadingsTask.h"
#include "tasks/sunAlignTask.h"
#include "tasks/testTasks.h"
#include "tasks/usbChargerTask.h"
#include "tasks/writeSystemStateTask.h"
#endif

#include "tasks/espCommTask.h"

#ifdef RX
#include "tasks/displayButtonsTask.h"
#include "tasks/displayTask.h"
#include "tasks/panelAlignTask.h"
#endif

void app_main() {
    esp_log_level_set("*", ESP_LOG_NONE);
    // uncomment one of these tasks to test a specific module comment out everything else
    // xTaskCreate(HelloWorldTestTask, "Hello_World_Test_Task", 1024, NULL, tskIDLE_PRIORITY, NULL);
    // xTaskCreate(compassTestTask, "Compass_Test_Task", 1024 * 10, NULL, tskIDLE_PRIORITY, NULL);
    // xTaskCreate(gyroscopeTestTask, "Gyroscope_Test_Task", 1024 * 10, NULL, tskIDLE_PRIORITY, NULL);
    // xTaskCreate(gpsTestTask, "GPS_Test_Task", 1024 * 10, NULL, tskIDLE_PRIORITY, NULL);
    // xTaskCreate(databaseTestTask, "Database_Test_Task", 1024 * 100, NULL, tskIDLE_PRIORITY, NULL);
    // xTaskCreate(barometerTestTask, "Barometer_Test_Task", 1024 * 10, NULL, tskIDLE_PRIORITY, NULL);
    // xTaskCreate(nightShutdownTask, "Night Shutdown Task", 1024, NULL, tskIDLE_PRIORITY, NULL);
    // xTaskCreate(chargerPwmTask, "PWM Task", 10240, NULL, 2, NULL);
    // xTaskCreate(usbChargerTask, "USB Task", 10240, NULL, 1, NULL);
    // xTaskCreate(adcTask, "ADC Task", 10240, NULL, tskIDLE_PRIORITY, NULL);
    // xTaskCreate(persistSystemStateTask, "Persist System State Task", 10240, NULL, tskIDLE_PRIORITY, NULL);
    // xTaskCreate(readCoulombCounterTestTask, "stats_calc", 1024 * 4, NULL, tskIDLE_PRIORITY, NULL);
    // xTaskCreate(sunTrackingTask, "sunTrackingTask", 10240, NULL, tskIDLE_PRIORITY, NULL);
    // xTaskCreate(readCoulombCounterTestTask, "readCoulombCounterTestTask", 1024 * 100, NULL, tskIDLE_PRIORITY, NULL);

    // initialize all modules
    if (globalInit() != ESP_OK) {
        ESP_LOGE("app_main", "critical module failed to initialize. restarting....");
        esp_restart();
    }

// TODO create application tasks with xTaskCreate MAX_NAME_LENGTH 16 Bytes
#ifdef TX
    ESP_LOGI("main", "THIS IS A SENDER");
    xTaskCreate(adcReadTask, "adc_read", 1024 * 5, NULL, tskIDLE_PRIORITY, NULL);
    xTaskCreate(batteryChargerTask, "battery_charger", 1024 * 3, NULL, tskIDLE_PRIORITY, NULL);
    xTaskCreate(socCalibrationTask, "soc_calibration", 1024 * 5, NULL, tskIDLE_PRIORITY, NULL);
    xTaskCreate(storeReadingsTask, "store_readings", 1024 * 10, NULL, 1, NULL);
    xTaskCreate(sunAlignTask, "sun_align", 1024 * 5, NULL, tskIDLE_PRIORITY, NULL);
    xTaskCreate(usbChargerTask, "usb_charger", 1024 * 10, NULL, tskIDLE_PRIORITY, NULL);
    xTaskCreate(writeSystemStateTask, "write_state", 1024 * 4, NULL, tskIDLE_PRIORITY, NULL);
    xTaskCreate(statsCalcTask, "stats_calc", 1024 * 4, NULL, tskIDLE_PRIORITY, NULL);
    // xTaskCreate(compasscaliTask, "compasscaliTask", 1024 * 5, NULL, tskIDLE_PRIORITY, NULL);
    // xTaskCreate(dcf77Task, "dcf77", 1024 * 2, NULL, tskIDLE_PRIORITY, NULL);
#endif

#ifdef RX
    ESP_LOGI("main", "THIS IS A RECEIVER");
    xTaskCreate(displayTask, "display", 1024 * 10, NULL, tskIDLE_PRIORITY, &displayButtonsNotificationHandle);
    xTaskCreate(displayButtonsTask, "display_buttons", 1024 * 10, NULL, tskIDLE_PRIORITY, NULL);
    xTaskCreate(espCommReceiveTask, "esp_receive", 1024 * 10, NULL, tskIDLE_PRIORITY, NULL);
    xTaskCreate(panelAlignTask, "panelAlignTask", 1024 * 5, NULL, tskIDLE_PRIORITY, NULL);

#endif
    return;
}