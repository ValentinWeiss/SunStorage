#include "tasks/dcf77Task.h"

#include "dcf77.h"
#include "settings.h"

void dcf77Task() {
    while (1) {
        DCF77_SignalState = gpio_get_level(DCF77_PIN);

        if (DCF77_SignalState != DCF77_prevSignalState) {
            DCF77_prevSignalState = DCF77_SignalState;
            dcf77ReadSignal();
        }

        vTaskDelay(DCF77_DELAY);
    }
}
