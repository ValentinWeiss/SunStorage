#ifndef DISPLAYTASK_H
#define DISPLAYTASK_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern TaskHandle_t displayButtonsNotificationHandle;

void displayTask();

#endif  // DISPLAYTASK_H
