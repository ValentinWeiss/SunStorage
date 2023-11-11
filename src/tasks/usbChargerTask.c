#include "tasks/usbChargerTask.h"

#include "FreeRTOSConfig.h"
#include "charger.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
/**
 * @brief Task which controls the dacs for the USB-output mode
 *
 */
void usbChargerTask() {
    while (1) {
        updateUsb();
        vTaskDelay(USB_CHARGER_DELAY);
    }
}
