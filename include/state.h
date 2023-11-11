#ifndef STATE_H
#define STATE_H

#include <esp_err.h>
#include <string.h>

#include "cJSON.h"
#include "espComm.h"
#include "esp_err.h"
#include "sdCardService.h"
#include "settings.h"
#include "stringConcat.h"

esp_err_t initSystemState();

void writeUpdatedJsonToState();

// /////////////////////////////////////////////////////////
// decrlare getters to all system relevant variables here:

/**
 * @brief Set the Usb Mode. This sets the charging power of the USB. Not all devices may support all the modes.
 *
 * @param usbState the state of the usb circuit, which determines the available power.
 */
void setUsbMode(USB_MODE usbState);

/**
 * @brief Set the Solar Charger State. Only state changes from DISABLED to any other state or from
 * any state to DISABLED are sensible, since the other mode changes are triggered automatically and
 * the user input will probably be overriden at the next invokation of chargerPwmTask
 *
 * @param chargerMode the new mode
 */
void setSolarChargerState(CHARGER_MODE chargerMode);

/**
 * @brief Set the Solar Current Target. This maximum current, which is allowed to fill the battery.
 * normally this will be equal to the amount of Ah of the battery (in our case 2000) for LiPo and
 * around 1 / 10 Ah for NiMH (around 250 for the batteries in the lab)
 *
 * @param milliAmps
 */
void setSolarCurrentTarget(uint16_t milliAmps);

/**
 * @brief Set the Threshold Voltage, which is used as a hysteresis to delay the activation of the task
 * If the battery is considered full at 4200 and the threshold is set to 50, the charger will start charging
 * at 4150.
 *
 * @param millivolts hysteresis in millivolts
 */
void setThresholdVoltage(uint16_t millivolts);

/**
 * @brief Set the maximum voltage for batteries. If the voltage is higher, the battey is considered FULL
 *
 * @param millivolts the target voltage
 */
void setMaximumVoltage(uint16_t millivolts);

/**
 * @brief The threshold, from which the charger should begin to decrease the current.
 * if the maximum voltage is set to 4200 and the trickeThreshold is set to 4000, the charger will supply
 * full power, until 4000 mV are reached, and then linearly decrease the current. At 4100, for instance, only half the
 * current will be supplied.
 *
 * @param millivolts the threshold voltage
 */
void setTrickleThreshold(uint16_t millivolts);

/**
 * @brief The high power circuit is a 5V circuit used for usb-charging and servos.
 * This method allows to swtich it on or off.
 * @param enabled the enablement status
 */
void setHighPowerCircuitEnabled(bool enabled);

void setBatteryOverheatedTemperature(uint8_t temperature);

void setBatteryOverheatedHysteresis(uint8_t temperature);

void setBatterySize(uint16_t mah);

void setAutoAdjustPanel(bool isManual);

void setAngleToNorth(int16_t angleToNorth);

void setPanelAzimuth(int16_t degree);

void setPanelElevation(int16_t degree);

void setTimestamp(uint16_t timestamp);

void setState(cJSON* state);

void setLatitude(int16_t lat);

void setLongitude(int16_t longitude);

void setDailyProduction(int16_t dailyProduction);

void setAngleToNorth(int16_t angle);

void setDisplayConfig(display_config_t* config);

void setNightShutdownEnabled(bool enabled);

void setNightShutdownAzimuth(int16_t longitude);

void setNightShutdownElevation(int16_t elevation);

void setEsp2Azimuth(float azimuth);

void setEsp2Elevation(float elevation);

void setEsp2xGyro(float xGyro);

void setEsp2yGyro(float yGyro);

void setEsp2zGyro(float zGyro);

void setEsp2Heading(float heading);

// /////////////////////////////////////////////////////////
// decalare setters to all system relevant variables here:

// charger
USB_MODE getUsbMode();

CHARGER_MODE getSolarChargerState();

uint16_t getSolarCurrentTarget();

uint16_t getThresholdVoltage();

uint16_t getMaximumVoltage();

uint16_t getTrickleThreshold();

bool isHighPowerModeEnabled();

uint8_t getBatteryOverheatedTemperature();

uint8_t getBatteryOverheatedHysteresis();

uint16_t getBatterySize();

bool getAutoAdjustPanel();

int16_t getAngleToNorth();

int16_t getPanelAzimuth();

int16_t getPanelElevation();

uint16_t getTimestamp();

void getState(cJSON** json);

int16_t getLatitude();

int16_t getLongitude();

int16_t getDailyProduction();

void getDisplayConfig(display_config_t* lcdMenuItemsConfig);

bool isNightShutdownEnabled();

int16_t getNightShutdownAzimuth();

int16_t getNightShutdownElevation();

float getEsp2Azimuth();

float getEsp2Elevation();

float getEsp2xGyro();

float getEsp2yGyro();

float getEsp2zGyro();

float getEsp2Heading();

#endif  // STATE_H
