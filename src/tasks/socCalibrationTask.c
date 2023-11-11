#include "tasks/socCalibrationTask.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "settings.h"
#include "soc.h"

/**
 * @brief The task for recalibrating the hybrid SoC
 * This task switches off the solar charger and the high power supply. Then reads the battery voltage and makes a precise
 * estimation of the SoC based on the voltage of the battery.
 * The hybrid SoC based on this measurement is frequently updated by the coulomb counter task.
 * This task should be of highest priority
 */
void socCalibrationTask() {
    while (1) {
        recalibrateSoC();
        vTaskDelay(SOC_TASK_DELAY);
    }
}
