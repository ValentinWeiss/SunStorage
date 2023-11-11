
#include "soc.h"

#include <stdint.h>
#include <stdio.h>

#include "adcReader.h"
#include "charger.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "rom/ets_sys.h"
#include "state.h"

#define VOLTAGE_100 8400
#define VOLTAGE_0 7400

const uint8_t SOC_TABLE[] = {
    10, 15, 16, 18, 19, 20, 21, 23, 24, 25,
    26, 28, 29, 30, 31, 33, 34, 35, 38, 40,
    41, 43, 44, 45, 46, 48, 49, 50, 53, 55,
    56, 58, 59, 60, 61, 61, 62, 62, 63, 63,
    64, 65, 66, 66, 67, 68, 68, 69, 69, 70,
    71, 72, 72, 73, 74, 75, 76, 76, 77, 78,
    78, 79, 79, 80, 80, 81, 81, 82, 82, 83,
    83, 83, 84, 84, 85, 85, 86, 86, 87, 87,
    88, 90, 91, 91, 92, 93, 93, 94, 95, 95,
    96, 96, 97, 97, 98, 98, 99, 99, 100, 100};

uint8_t hybridSoC = 0;

uint8_t getSateOfChargeViaVoltage(uint16_t voltage) {
    if (voltage > VOLTAGE_100) {
        return 100;
    }
    if (voltage < VOLTAGE_0) {
        return 0;
    }
    return SOC_TABLE[(voltage - VOLTAGE_0) / 10];
}

uint8_t getHybridStateOfCharge() {
    double coulombMeasured = getDuty() * (VCC / 1000.0) / RESISTOR;
    coulombMeasured *= 10000.0;

    double ampereHoursLost = coulombMeasured / COULOMB_TO_AMPERE_HOURS;

    double lastSyncedAmpHours = (hybridSoC / 100) * STATE_CHARGER_BATTERY_SIZE_DEFAULT;

    return (uint8_t)(((lastSyncedAmpHours - ampereHoursLost) / STATE_CHARGER_BATTERY_SIZE_DEFAULT) * 100);
}

void recalibrateSoC() {
    // this whole method should be a critical section, since an external change could be lost.
    // However, critical sections do not work and
    // An alternative would be using semaphors for highPowerCircuitEnabled and chargerMode
    // The more simple approach is to give this task the highest priority and it should not be interrupted

    // prepare
    CHARGER_MODE previousMode = getSolarChargerState();
    bool highPowerModePreviouslyEnabled = isHighPowerModeEnabled();
    setSolarChargerState(DISABLED);
    setHighPowerCircuitEnabled(false);
    ets_delay_us(1000);

    // check currents
    performRead();
    int currentSolar = getCurrentSolar();
    int currentSystem = getCurrentSystem();
    if (currentSolar > 100 || currentSystem > 100) {
        ESP_LOGW("soc", "current larger than 100 mA. SoC estimation might be inprecise.\n\n");
    }
    ESP_LOGI("soc", "Current Solar: %d\n\n", currentSolar);
    ESP_LOGI("soc", "Current System: %d\n\n", currentSystem);

    // calibrate SoC
    int voltage = getVoltageBattery();
    ESP_LOGI("soc", "Old Hybrid Soc: %u\n\n", hybridSoC);
    hybridSoC = getSateOfChargeViaVoltage(voltage);
    ESP_LOGI("soc", "New Hybrid Soc: %u\n\n", hybridSoC);

    resetDuty();

    // restore state
    setSolarChargerState(previousMode);
    setHighPowerCircuitEnabled(highPowerModePreviouslyEnabled);
}
