#ifndef CHARGER_H
#define CHARGER_H

#include <esp_err.h>
#include <settings.h>
#include <stdbool.h>
#include <stdint.h>

esp_err_t initCharger();

esp_err_t initHighPowerCircuit();

void updatePwm();

void debugUpdate();

void updateUsb();

#endif
