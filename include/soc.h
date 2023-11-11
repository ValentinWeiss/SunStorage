#ifndef SOC_H
#define SOC_H

#include <stdint.h>

/**
 * @brief calculates the state of charge for a LiPo battery using the voltage.
 *
 * @return percentage of the State of charge
 */
uint8_t getSateOfChargeViaVoltage(uint16_t voltage);

uint8_t getHybridStateOfCharge();

void recalibrateSoC();

#endif  // SOC_H
