#ifndef ADC_READER_H
#define ADC_READER_H

#include <stdint.h>

#include "esp_err.h"

esp_err_t initAdc();

/**
 * @brief triggers read. Only perform, if fresh values are actually required.
 */
void performRead();

/**
 * @brief The current, which flows from the solar panel into the battery. Values should be between 0 and 3000
 *
 * @return uint16_t the current in milliampere
 */
uint16_t getCurrentSolar();

/**
 * @brief The current, which flows from the battery to the consumers. This is the sum of the current
 * through the USB and the ESP. Values should be between 0 and 3000
 *
 * @return uint16_t the current in milliampere
 */
uint16_t getCurrentSystem();

/**
 * @brief The voltage of the lower cell of the battery. Common values should be between 3500 and 4200
 *
 * @return uint16_t the voltage in millivolt
 */
uint16_t getVoltageLowerCell();

/**
 * @brief The voltage of the lower cell of the battery. Common values should be between 3500 and 4200
 *
 * @return uint16_t the voltage in millivolt
 */
uint16_t getVoltageBattery();

/**
 * @brief The voltage of the higher cell of the battery. Common values should be between 3500 and 4200
 *
 * @return uint16_t the voltage in millivolt
 */
uint16_t getVoltageHigherCell();

/**
 * @brief Get the Charging Current. Negative values mean, that the battery discharges
 *
 * @return int16_t NOTE: this value might be negative
 */
int16_t getChargingCurrent();

/**
 * @brief The power delivered from the battery to the System (ESP and USB). Expected values are between 0 and 30000
 *
 * @return uint32_t the power in milliwatt
 */
uint32_t getPowerSystem();

/**
 * @brief The power delivered from the solar panel to the battery. Expected values are between 0 and 20000
 *
 * @return uint32_t the power in milliwatt
 */
uint16_t getPowerSolar();

/**
 * @brief The Duty-Cycle of the Coulomb counter AD-Integrator
 *
 * @return double the duty cycle in seconds
 */
double getDuty();

/**
 * @brief Resets the accumulated duty cycle of the coulomb counter
 *
 */
void resetDuty();

#endif
