#include "lcdMenu.h"

display_config_t displayConfig = {.itemEnabled = {true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true}};

display_config_t DEFAULT_DISPLAY_CONFIG = {.itemEnabled = {true, true, true, false, false,
                                                           false, false, false, false,
                                                           false, false, false, false,
                                                           false, false, false, false,
                                                           false, false, false, false,
                                                           false, false, false, false,
                                                           false, false, false, false,
                                                           false, false, false, false}};

const char* DISPLAY_NAMES[NUM_DISPLAY_ITEMS] = {
    "powerSolar", "dailyProduction", "soc", "voltageLowerCell", "voltageUpperCell",
    "solarCurrent", "chargingCurrent", "systemCurrent", "powerSystem", "powerCharging",
    "dailyConsumption", "thresholdVoltage", "maximumVoltage", "trickleThreshold", "highPowerCircuit",
    "batteryTemperature", "batteryOverheatedHysteresis", "batterySize", "currentTarget", "chargerState",
    "usbMode", "azimuth", "elevation", "manualAdjustment", "nightShutdownEnabled",
    "nightPositionAzimuth", "nightPositionElevation", "latitude", "longitude", "angleToNorth",
    "time", "httpDisableTimeout", "gpsDisableTimeout", "dcfDisableTimeout", "wifiDisableTimeout"};

bool lcdMenuDisplayState(uint8_t state, lcd_4bit_t* lcd) {
    char text[40];  // just half the screen should be enough

    if (!displayConfig.itemEnabled[state]) {
        return false;
    }

    switch (state) {
        case 1:
            sprintf(text, "%s\n\n%d Wh", DISPLAY_NAMES[state], getDailyProduction());
            break;
        case 20:
            const char* usbModeStrings[] = {"2.5 W", "5 W", "10 W"};
            sprintf(text, "%s\n\n%s", DISPLAY_NAMES[state], usbModeStrings[getUsbMode()]);
            break;
        case 19:
            const char* chargerModeStrings[] = {"Disabled", "Charging", "Full", "Balancing", "Overheated"};
            sprintf(text, "%s\n\n%s", DISPLAY_NAMES[state], chargerModeStrings[getSolarChargerState()]);
            break;
        case 18:
            sprintf(text, "%s\n\n%hu mA", DISPLAY_NAMES[state], getSolarCurrentTarget());
            break;
        case 11:
            sprintf(text, "%s\n\n%hu mV", DISPLAY_NAMES[state], getThresholdVoltage());
            break;
        case 12:
            sprintf(text, "%s\n\n%hu mV", DISPLAY_NAMES[state], getMaximumVoltage());
            break;
        case 13:
            sprintf(text, "%s\n\n%hu mV", DISPLAY_NAMES[state], getTrickleThreshold());
            break;
        case 14:
            sprintf(text, "%s\n\n%s", DISPLAY_NAMES[state], isHighPowerModeEnabled() ? "Enabled" : "Disabled");
            break;
        case 15:
            sprintf(text, "%s\n%hu \337C", DISPLAY_NAMES[state], getBatteryOverheatedTemperature());
            break;
        case 16:
            sprintf(text, "%s\n%hu", DISPLAY_NAMES[state], getBatteryOverheatedHysteresis());
            break;
        case 17:
            sprintf(text, "%s\n\n%hu mAh", DISPLAY_NAMES[state], getBatterySize());
            break;
        case 28:
            sprintf(text, "%s\n\n%hu \337", DISPLAY_NAMES[state], getLongitude());
            break;
        case 27:
            sprintf(text, "%s\n\n%hu \337", DISPLAY_NAMES[state], getLatitude());
            break;
        case 24:
            sprintf(text, "%s\n%s", DISPLAY_NAMES[state], isNightShutdownEnabled() ? "Enabled" : "Disabled");
            break;
        case 25:
            sprintf(text, "%s\n%hu \337", DISPLAY_NAMES[state], getNightShutdownAzimuth());
            break;
        case 26:
            sprintf(text, "%s\n%hu \337", DISPLAY_NAMES[state], getNightShutdownElevation());
            break;
        case 21:
            sprintf(text, "%s\n\n%hu \337", DISPLAY_NAMES[state], getPanelAzimuth());
            break;
        case 22:
            sprintf(text, "%s\n\n%hu \337", DISPLAY_NAMES[state], getPanelElevation());
            break;
        default:
            ESP_LOGE("lcdMenu_display", "got invalid state to display");
            return false;
            break;
    }

    printf("%s\n", text);
    lcdWriteString4Bit(lcd, text);

    return true;
}

void lcdMenuSleep(lcd_4bit_t* lcd) {
    lcdWriteInstruction4Bit(lcd, LCD_DISPLAY_OFF);
    lcdOff(lcd);

    uint32_t notificationValue;

    xTaskNotifyWait(0, 0, &notificationValue, portMAX_DELAY);  // wait for any button to be pressed

    lcdOn(lcd);
    lcdWriteInstruction4Bit(lcd, LCD_DISPLAY_ON);

    return;
}