
#include "charger.h"

#include <math.h>
#include <stdio.h>

#include "adcReader.h"
#include "driver/barometer.h"
#include "driver/dac.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "initTracker.h"
#include "settings.h"
#include "state.h"

#define BALANICNG_THRESHOLD 300
#define PWM_REGULATION_FACTOR 1
#define CELL_COUNT 2

#define DAC_2_VOLT 160  // should be lower, but for some reason the dac output voltage is a bit too low
#define DAC_2_8_VOLT 230

void setDutyCycle(ledc_channel_t channel, int dutyCycle) {
    ledc_set_duty(CHARGER_LEDC_MODE, channel, dutyCycle);
    ledc_update_duty(CHARGER_LEDC_MODE, channel);
}

bool balancingRequired() {
    return abs(getVoltageHigherCell() - getVoltageLowerCell()) > BALANICNG_THRESHOLD;
}

bool temperatureBelow(uint8_t value) {
    float temperature;
    barometerMeasure(&temperature, NULL);
    return temperature < value;
}

void handleBalancing() {
    uint16_t voltageLowerCell = getVoltageLowerCell();
    uint16_t voltageHigherCell = getVoltageHigherCell();
    if (voltageLowerCell - voltageHigherCell > BALANICNG_THRESHOLD) {
        gpio_set_level(CHARGER_GPIO_LOWER_CELL_BALANCING, 0);
    } else if (voltageHigherCell - voltageLowerCell > BALANICNG_THRESHOLD) {
        gpio_set_level(CHARGER_GPIO_HIGHER_CELL_BALANCING, 0);
    } else {
        gpio_set_level(CHARGER_GPIO_LOWER_CELL_BALANCING, 1);
        gpio_set_level(CHARGER_GPIO_HIGHER_CELL_BALANCING, 1);
        setSolarChargerState(CHARGING);
    }
}

void handleCharging() {
    int32_t dutyCycle = (int32_t)ledc_get_duty(CHARGER_LEDC_MODE, LEDC_CHANNEL_0);
    uint16_t currentVoltage = getVoltageBattery();
    int16_t targetAmps = 0;

    if (!temperatureBelow(getBatteryOverheatedTemperature())) {
        setSolarChargerState(OVERHEATED);
    }

    if (balancingRequired()) {
        setSolarChargerState(BALANCING);
    }

    if (currentVoltage > getMaximumVoltage() * CELL_COUNT) {
        setSolarChargerState(FULL);
    }

    uint16_t trickleThresholdVoltage = getTrickleThreshold();
    if (currentVoltage > trickleThresholdVoltage * CELL_COUNT) {
        uint16_t solarTargetMilliAmps = getSolarCurrentTarget();
        uint16_t maxVoltage = getMaximumVoltage();
        int trickleOffset = (currentVoltage - trickleThresholdVoltage * CELL_COUNT) / CELL_COUNT;
        targetAmps = solarTargetMilliAmps + solarTargetMilliAmps * (trickleOffset / (trickleThresholdVoltage - maxVoltage));
        ESP_LOGI("charger", "Trickle Charging: target %d mA", targetAmps);
    }

    int16_t current = getChargingCurrent();

    if (current < targetAmps) {
        dutyCycle = dutyCycle + (targetAmps - current) * PWM_REGULATION_FACTOR;
    }
    if (dutyCycle < 0) {
        dutyCycle = 0;
    }
    if (dutyCycle >= 1 << LEDC_TIMER_13_BIT) {
        dutyCycle = (1 << LEDC_TIMER_13_BIT) - 1;
    }

    ESP_LOGI("charger", "new cycle %ld", dutyCycle);
    setDutyCycle(LEDC_CHANNEL_0, dutyCycle);
}

void handleFull() {
    uint16_t currentVoltage = getVoltageBattery();
    uint16_t trickleThresholdVoltage = getTrickleThreshold();
    uint16_t fullThreshold = getThresholdVoltage();
    if (currentVoltage - fullThreshold > trickleThresholdVoltage * CELL_COUNT) {
        setSolarChargerState(CHARGING);
    }
    if (balancingRequired()) {
        setSolarChargerState(BALANCING);
    }
}

void handleOverheated() {
    if (temperatureBelow(getBatteryOverheatedTemperature() - getBatteryOverheatedHysteresis())) {
        setSolarChargerState(CHARGING);
    }
    setDutyCycle(LEDC_CHANNEL_0, 0);
}

void debugUpdate() {
    ESP_LOGI("charger", "Solar Current: %d\n\n", getChargingCurrent());
    ESP_LOGI("charger", "System Current: %d\n\n", getCurrentSystem());
    ESP_LOGI("charger", "Voltage Cell 0: %hu\n\n", getVoltageLowerCell());
    ESP_LOGI("charger", "Voltage Battery: %hu\n\n", getVoltageBattery());
    ESP_LOGI("charger", "Solar State: %d\n\n", getSolarChargerState());
    ESP_LOGI("charger", "DutyCycle Solar: %d\n\n", (int)ledc_get_duty(CHARGER_LEDC_MODE, LEDC_CHANNEL_0));
}

void updatePwm() {
    gpio_set_level(CHARGER_GPIO_HIGH_POWER_DISABLE, !isHighPowerModeEnabled());
    ESP_LOGI("charger", "high power enabled %d", isHighPowerModeEnabled());
    CHARGER_MODE solarState = getSolarChargerState();
    switch (solarState) {
        case DISABLED:
            ESP_LOGI("charger", "Disabled");
            setDutyCycle(LEDC_CHANNEL_0, 0);
            break;

        case CHARGING:
            ESP_LOGI("charger", "Charging");
            handleCharging();
            break;

        case BALANCING:
            ESP_LOGI("charger", "Balancing");
            handleBalancing();
            break;

        case FULL:
            ESP_LOGI("charger", "Full");
            handleFull();
            break;

        case OVERHEATED:
            ESP_LOGW("charger", "Battery too hot");
            handleOverheated();
            break;

        default:
            ESP_LOGE("charger", "invalid charger mode");
    }
}

esp_err_t initCharger() {
    esp_err_t error = ESP_OK;

    // barometer
    if (!barometerIsInitialized) {
        ESP_LOGE("charger", "barometer/temperature sensor is not yet initialized");
        return ESP_FAIL;
    }

    // PWM
    ledc_timer_config_t ledcTimer = {
        .speed_mode = CHARGER_LEDC_MODE,
        .timer_num = CHARGER_LEDC_TIMER_SOLAR,
        .duty_resolution = CHARGER_LEDC_DUTY_RES,
        .freq_hz = CHARGER_LEDC_FREQUENCY,
        .clk_cfg = LEDC_AUTO_CLK};

    error = ledc_timer_config(&ledcTimer);
    ESP_HANDLE_ERR(error, "charger: could not create ledc config");

    ledc_channel_config_t ledcChannel = {
        .speed_mode = CHARGER_LEDC_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = CHARGER_LEDC_TIMER_SOLAR,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = CHARGER_LEDC_OUTPUT_SOLAR,
        .duty = 0,
        .hpoint = 0};

    error = ledc_channel_config(&ledcChannel);
    ESP_HANDLE_ERR(error, "charger: could set up ledc channel");

    gpio_config_t outputConfig;
    outputConfig.mode = GPIO_MODE_OUTPUT;
    outputConfig.pull_up_en = GPIO_PULLUP_DISABLE;
    outputConfig.pull_down_en = GPIO_PULLDOWN_DISABLE;
    outputConfig.intr_type = GPIO_INTR_DISABLE;

    outputConfig.pin_bit_mask = (1ULL << CHARGER_GPIO_HIGH_POWER_DISABLE);

    ESP_HANDLE_ERR(gpio_config(&outputConfig), "failed to set up high power disable pin");

    outputConfig.pin_bit_mask = (1ULL << CHARGER_GPIO_HIGHER_CELL_BALANCING);
    ESP_HANDLE_ERR(gpio_config(&outputConfig), "failed to set up lower balancing pin");

    outputConfig.pin_bit_mask = (1ULL << CHARGER_GPIO_LOWER_CELL_BALANCING);
    ESP_HANDLE_ERR(gpio_config(&outputConfig), "failed to set up higher balancing pin");

    error = gpio_set_level(CHARGER_GPIO_LOWER_CELL_BALANCING, 1);
    ESP_HANDLE_ERR(error, "charger: could not set cell balancing lower pin high");

    error = gpio_set_level(CHARGER_GPIO_HIGHER_CELL_BALANCING, 1);
    ESP_HANDLE_ERR(error, "charger: could not set cell balancing upper pin high");

    // Usb Charger
    error = dac_output_enable(DAC_CHANNEL_1);
    ESP_HANDLE_ERR(error, "charger: could not enable DAC 1");

    error = dac_output_enable(DAC_CHANNEL_2);
    ESP_HANDLE_ERR(error, "charger: could not enable DAC 2");

    return ESP_OK;
}

esp_err_t initHighPowerCircuit() {
    esp_err_t error = ESP_OK;

    error = gpio_set_direction(CHARGER_GPIO_HIGH_POWER_DISABLE, GPIO_MODE_OUTPUT);
    ESP_HANDLE_ERR(error, "charger: could not set up high power disable pin");

    error = gpio_set_level(CHARGER_GPIO_HIGH_POWER_DISABLE, 1);
    ESP_HANDLE_ERR(error, "charger: could not set high power disable pin high");
    return ESP_OK;
}

void updateUsb() {
    int usbState = getUsbMode();
    ESP_LOGI("charger", "USB STATE: %d\n\n", usbState);
    ESP_LOGI("charger", "High Power circuit enabled %d\n\n", isHighPowerModeEnabled());

    switch (usbState) {
        case _2_POINT_5_WATT:
            dac_output_voltage(DAC_CHANNEL_1, DAC_2_VOLT);
            dac_output_voltage(DAC_CHANNEL_2, DAC_2_VOLT);
            break;
        case _5_WATT:
            dac_output_voltage(DAC_CHANNEL_1, DAC_2_VOLT);
            dac_output_voltage(DAC_CHANNEL_2, DAC_2_8_VOLT);
            break;
        case _10_WATT:
            dac_output_voltage(DAC_CHANNEL_1, DAC_2_8_VOLT);
            dac_output_voltage(DAC_CHANNEL_2, DAC_2_VOLT);
            break;
        default:
            ESP_LOGE("charger", "invalid usb mode");
    }
}
