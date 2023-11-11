#ifndef INITTRACKER_H
#define INITTRACKER_H

#include <stdbool.h>

extern bool highPowerIsInitialized;
extern bool adcIsInitialized;
extern bool nvsIsInitialized;
extern bool spiffsIsInitialized;
extern bool i2cIsInitialized;
extern bool barometerIsInitialized;
extern bool compassModuleIsInitialized;
extern bool gyroscopeIsInitialized;
extern bool gpsIsInitialized;

extern bool chargerIsInitialized;
extern bool sdCardIsInitialized;
extern bool wifiIsInitialized;
extern bool systemStateIsInitialized;
extern bool dbIsInitialized;
extern bool servoIsInitialized;

extern bool lcdDisplayInitialized;
extern bool lcdButtonsInitialized;

extern bool httpServerIsInitialized;

extern bool dcf77IsInitialized;

extern bool uartTxIsInitialized;
extern bool uartRxIsInitialized;

#endif  // INITTRACKER_H
