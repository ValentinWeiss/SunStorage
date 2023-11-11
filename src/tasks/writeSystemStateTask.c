#include "tasks/writeSystemStateTask.h"

#include "FreeRTOSConfig.h"
#include "freertos/FreeRTOS.h"
#include "settings.h"
#include "state.h"

void writeSystemStateTask() {
    while (1) {
        writeUpdatedJsonToState();
        vTaskDelay(STATE_WRITE_DELAY);
    }
}
