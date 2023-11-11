#include "tasks/batteryChargerTask.h"

#include "charger.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "settings.h"

/**
 * @brief task for controlling the PWM and the statemachine of the battery charger
 *
 */
void batteryChargerTask() {
    while (1) {
        debugUpdate();
        updatePwm();
        vTaskDelay(BATTERY_CHARGER_TASK_DELAY);
    }
}
