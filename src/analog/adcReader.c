#include "adcReader.h"

#include <string.h>

#include "driver/gpio.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "settings.h"
#include "soc/soc_caps.h"

#define AVG_RANGE 10

adc_oneshot_unit_handle_t adcHandle = NULL;
adc_cali_handle_t caliHandleSolarCurrent = NULL;
adc_cali_handle_t caliHandleSystemCurrent = NULL;
adc_cali_handle_t caliHandleLowerCell = NULL;
adc_cali_handle_t caliHandleBattery = NULL;
adc_cali_handle_t caliHandleCoulombCounter = NULL;

int currentSystem = 0;
int voltageLowerCell = 0;
int voltageBattery = 0;
int currentSolar = 0;

uint64_t duty = 0;
uint64_t timestamp = 0;

int history[4][AVG_RANGE];
int historyIndex = -1;

#define VOLTAGE_SOLAR_0 1650
#define SOLAR_SHUNT_TO_AMPS 2.0
#define SYSTEM_SHUNT_TO_AMPS 2.0
#define VOLTAGE_DIVIDER_LOWER 2.26
#define VOLTAGE_DIVIDER_BATTERY 4.05
#define VOLTAGE_DIVIDER_SYS_CURRENT 3.65

esp_err_t initAdc() {
    esp_err_t error = ESP_OK;
    adc_oneshot_unit_init_cfg_t adcUnitConfig = {
        .unit_id = ADC_UNIT_1,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    adc_oneshot_chan_cfg_t adcChannelConfig = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_11,
    };
    error = adc_oneshot_new_unit(&adcUnitConfig, &adcHandle);
    ESP_HANDLE_ERR(error, "reader: could not set up unit");

    adc_cali_line_fitting_config_t caliConfigSolarCurrent = {
        .unit_id = ADC_UNIT_1,
        .atten = ADC_ATTEN_DB_11,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    error = adc_cali_create_scheme_line_fitting(&caliConfigSolarCurrent, &caliHandleSolarCurrent);
    ESP_HANDLE_ERR(error, "reader: could not set up line fitting for solar current");

    adc_cali_line_fitting_config_t caliConfigSystemCurrent = {
        .unit_id = ADC_UNIT_1,
        .atten = ADC_ATTEN_DB_11,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    error = adc_cali_create_scheme_line_fitting(&caliConfigSystemCurrent, &caliHandleSystemCurrent);
    ESP_HANDLE_ERR(error, "reader: could not set up line fitting for sys current");

    adc_cali_line_fitting_config_t caliConfigLowerCell = {
        .unit_id = ADC_UNIT_1,
        .atten = ADC_ATTEN_DB_11,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    error = adc_cali_create_scheme_line_fitting(&caliConfigLowerCell, &caliHandleLowerCell);
    ESP_HANDLE_ERR(error, "reader: could not set up line fitting for lower cell");

    adc_cali_line_fitting_config_t caliConfigBattery = {
        .unit_id = ADC_UNIT_1,
        .atten = ADC_ATTEN_DB_11,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };

    error = adc_cali_create_scheme_line_fitting(&caliConfigBattery, &caliHandleBattery);
    ESP_HANDLE_ERR(error, "reader: could not set up line fitting for battery");

    adc_cali_line_fitting_config_t caliConfigCoulombCounter = {
        .unit_id = ADC_UNIT_1,
        .atten = ADC_ATTEN_DB_11,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    error = adc_cali_create_scheme_line_fitting(&caliConfigCoulombCounter, &caliHandleCoulombCounter);
    ESP_HANDLE_ERR(error, "reader: could not set up line fitting for coulomb counter");

    error = adc_oneshot_config_channel(adcHandle, ADC_CH_CURRENT_SOLAR, &adcChannelConfig);
    ESP_HANDLE_ERR(error, "reader: could not set up config for solar current");

    error = adc_oneshot_config_channel(adcHandle, ADC_CH_CURRENT_SYSTEM, &adcChannelConfig);
    ESP_HANDLE_ERR(error, "reader: could not set up config for system current");

    error = adc_oneshot_config_channel(adcHandle, ADC_CH_VOLTAGE_LOWER_CELL, &adcChannelConfig);
    ESP_HANDLE_ERR(error, "reader: could not set up config for lower cell");

    error = adc_oneshot_config_channel(adcHandle, ADC_CH_VOLTAGE_BATTERY, &adcChannelConfig);
    ESP_HANDLE_ERR(error, "reader: could not set up config for battery");

    error = adc_oneshot_config_channel(adcHandle, ADC_CH_COULOMB_COUNTER, &adcChannelConfig);
    ESP_HANDLE_ERR(error, "reader: could not set up config for coulomb counter");

    // set coulomb counter discharge pin
    gpio_set_direction(COULOMB_COUNTER_DISCHARGE_PIN, GPIO_MODE_OUTPUT);
    gpio_set_pull_mode(COULOMB_COUNTER_DISCHARGE_PIN, GPIO_PULLDOWN_DISABLE);
    gpio_set_pull_mode(COULOMB_COUNTER_DISCHARGE_PIN, GPIO_PULLUP_DISABLE);
    gpio_set_level(COULOMB_COUNTER_DISCHARGE_PIN, 1);

    return ESP_OK;
}

int compareInts(const void *a, const void *b) {
    int *da = (int *)a;
    int *db = (int *)b;

    return (*da > *db) - (*da < *db);
}

void calculateAvgWithoutExtremes(int typeIndex, int *result) {
    int historyCpy[AVG_RANGE];
    for (int i = 0; i < AVG_RANGE; i++) {
        historyCpy[i] = history[typeIndex][i];
    }
    qsort(historyCpy, AVG_RANGE, sizeof(int), compareInts);
    int sum = 0;
    for (int i = 1; i < AVG_RANGE - 1; i++) {
        sum += historyCpy[i];
    }
    *result = sum / (AVG_RANGE - 2);
}

void performRead() {
    int currentSystemAdc = 0;
    int currentSolarAdc = 0;
    int voltageLowerCellAdc = 0;
    int voltageBatteryAdc = 0;
    int voltageCoulombCounter = 0;

    int readRaw = 0;
    historyIndex = (historyIndex + 1) % 10;
    adc_oneshot_read(adcHandle, ADC_CH_CURRENT_SOLAR, &readRaw);
    adc_cali_raw_to_voltage(caliHandleSolarCurrent, readRaw, &history[0][historyIndex]);
    adc_oneshot_read(adcHandle, ADC_CH_CURRENT_SYSTEM, &readRaw);
    adc_cali_raw_to_voltage(caliHandleSystemCurrent, readRaw, &history[1][historyIndex]);
    adc_oneshot_read(adcHandle, ADC_CH_VOLTAGE_LOWER_CELL, &readRaw);
    adc_cali_raw_to_voltage(caliHandleLowerCell, readRaw, &history[2][historyIndex]);
    adc_oneshot_read(adcHandle, ADC_CH_VOLTAGE_BATTERY, &readRaw);
    adc_cali_raw_to_voltage(caliHandleBattery, readRaw, &history[3][historyIndex]);
    adc_oneshot_read(adcHandle, ADC_CH_COULOMB_COUNTER, &readRaw);
    adc_cali_raw_to_voltage(caliHandleCoulombCounter, readRaw, &voltageCoulombCounter);

    calculateAvgWithoutExtremes(0, &currentSolarAdc);
    calculateAvgWithoutExtremes(1, &currentSystemAdc);
    calculateAvgWithoutExtremes(2, &voltageLowerCellAdc);
    calculateAvgWithoutExtremes(3, &voltageBatteryAdc);

    int voltageSolarShunt = VOLTAGE_SOLAR_0 - currentSolarAdc;

    voltageBattery = ((double)voltageBatteryAdc + (double)voltageSolarShunt * (double)SOLAR_SHUNT_TO_AMPS / (double)VOLTAGE_DIVIDER_BATTERY) * (double)VOLTAGE_DIVIDER_BATTERY;
    voltageBattery = voltageBattery < 0 ? 0 : voltageBattery;

    voltageLowerCell = (voltageLowerCellAdc + voltageSolarShunt * (double)SOLAR_SHUNT_TO_AMPS / (double)VOLTAGE_DIVIDER_LOWER) * (double)VOLTAGE_DIVIDER_LOWER;
    voltageLowerCell = voltageLowerCell < 0 ? 0 : voltageLowerCell;

    currentSystem = (voltageBatteryAdc * VOLTAGE_DIVIDER_BATTERY - currentSystemAdc * VOLTAGE_DIVIDER_SYS_CURRENT) * SYSTEM_SHUNT_TO_AMPS;
    currentSystem = currentSystem < 0 ? 0 : currentSystem;

    currentSolar = currentSystem + voltageSolarShunt * SOLAR_SHUNT_TO_AMPS;

    if (historyIndex == 9) {
        ESP_LOGI("adc", "battery adc %d\n", voltageBatteryAdc);
        ESP_LOGI("adc", "lower adc %d\n", voltageLowerCellAdc);
        ESP_LOGI("adc", "system adc %d\n", currentSystemAdc);
        ESP_LOGI("adc", "solar adc %d\n", currentSolarAdc);

        ESP_LOGI("adc", "battery %d\n", voltageBattery);
        ESP_LOGI("adc", "lower %d\n", voltageLowerCell);
        ESP_LOGI("adc", "system  %d\n", currentSystem);
        ESP_LOGI("adc", "solar  %d\n", currentSolar);
    }

    // coulomb counter
    if (voltageCoulombCounter > VCC / 2) {
        if (timestamp == 0) timestamp = esp_timer_get_time();
        gpio_set_level(COULOMB_COUNTER_DISCHARGE_PIN, 1);
    } else {
        gpio_set_level(COULOMB_COUNTER_DISCHARGE_PIN, 0);

        if (timestamp != 0) {
            duty += (esp_timer_get_time() - timestamp) / 1000;  // time passed in milliseconds
            timestamp = 0;                                      // reset timer
        }
    }
}

uint16_t getCurrentSolar() {
    return currentSolar;
}

uint16_t getCurrentSystem() {
    return currentSystem;
}

uint16_t getVoltageLowerCell() {
    return voltageLowerCell;
}

uint16_t getVoltageBattery() {
    return voltageBattery;
}

uint16_t readVoltageBattery() {
    performRead();
    return history[3][historyIndex];
}

int16_t getChargingCurrent() {
    return currentSolar - currentSystem;
}

uint16_t getVoltageHigherCell() {
    return voltageBattery - voltageLowerCell;
}

uint32_t getPowerSystem() {
    return voltageBattery * currentSystem * 0.001;
}

uint16_t getPowerSolar() {
    return voltageBattery * currentSolar * 0.001;
}

double getDuty() {
    return duty / 1000.0;
}

void resetDuty() {
    duty = 0;
}
