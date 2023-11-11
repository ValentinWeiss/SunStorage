#include "tasks/adcReadTask.h"

#include "adcReader.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "settings.h"

/**
 * @brief this task frequently reads adc values. Use this task to optain recent values. Only use performRead, if
 * completely new values are needed (due change in the system)
 *
 */
void adcReadTask() {
    while (1) {
        performRead();
        vTaskDelay(ADC_TASK_DELAY);
    }
}
