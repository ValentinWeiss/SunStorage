#include "state.h"

cJSON *state = NULL;

bool toNotifyChange = false;

SemaphoreHandle_t writeStateMutex;

esp_err_t parseStateFileToJson(cJSON **root) {
    FILE *file = fopen(PATH_CONCAT(SDC_MOUNT_POINT, STATE_FILE), "r");
    if (file == NULL) {
        return ESP_FAIL;
    }
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *fileContent = (char *)malloc(file_size + 1);
    fread(fileContent, 1, file_size, file);
    fileContent[file_size] = '\0';
    fclose(file);

    *root = cJSON_Parse(fileContent);

    if (root == NULL) {
        free(fileContent);
        return ESP_FAIL;
    }
    free((void *)fileContent);

    return ESP_OK;
}

void writeUpdatedJsonToState() {
    if (toNotifyChange == true) {
        esp_err_t espTransmit = espCommTransmit();
        bool saveOk = false;
        if (xSemaphorePoll(writeStateMutex) == pdTRUE) {
            char *updatedJson = cJSON_Print(state);
            FILE *file = fopen(PATH_CONCAT(SDC_MOUNT_POINT, STATE_FILE), "w");
            if (file == NULL) {
                fclose(file);
            }

            fwrite(updatedJson, 1, strlen(updatedJson), file);
            fclose(file);
            xSemaphoreGive(writeStateMutex);
            ESP_LOGI("state", "state saved to sd card successfully");
            saveOk = true;
        } else {
            ESP_LOGE("state", "could not aquire write semaphore");
        }
        if ((espTransmit == ESP_OK) && (saveOk == true)) {
            toNotifyChange = false;
        }
    }
}

esp_err_t initSystemState() {
    writeStateMutex = xSemaphoreCreateMutex();
    if (xSemaphorePoll(writeStateMutex) == pdTRUE) {
        parseStateFileToJson(&state);
        if (state == NULL) {
            ESP_LOGI("state", "Unable to recreate state from JSON.\n\n");
            state = cJSON_CreateObject();
            cJSON *charger = cJSON_CreateObject();

            // charger
            cJSON_AddItemToObject(state, STATE_OBJ_CHARGER, charger);
            cJSON_AddNumberToObject(charger, STATE_CHARGER_USBMODE, (double)STATE_CHARGER_USBMODE_DEFAULT);
            cJSON_AddNumberToObject(charger, STATE_CHARGER_CHARGER_MODE, (double)STATE_CHARGER_CHARGER_MODE_DEFAULT);
            cJSON_AddNumberToObject(charger, STATE_CHARGER_CURRENT_TARGET, (double)STATE_CHARGER_CURRENT_TARGET_DEFAULT);
            cJSON_AddNumberToObject(charger, STATE_CHARGER_THRESHOLD_VOLTAGE, (double)STATE_CHARGER_THRESHOLD_VOLTAGE_DEFAULT);
            cJSON_AddNumberToObject(charger, STATE_CHARGER_MAXIMUM_VOLTAGE, (double)STATE_CHARGER_MAXIMUM_VOLTAGE_DEFAULT);
            cJSON_AddNumberToObject(charger, STATE_CHARGER_TRICKLE_THRESHOLD, (double)STATE_CHARGER_TRICKLE_THRESHOLD_DEFAULT);
            cJSON_AddNumberToObject(charger, STATE_CHARGER_BATTERY_SIZE, (double)STATE_CHARGER_BATTERY_SIZE_DEFAULT);
            cJSON_AddNumberToObject(charger, STATE_CHARGER_OVERHEATED_TEMPERATURE, (double)STATE_CHARGER_OVERHEATED_TEMPERATURE_DEFAULT);
            cJSON_AddNumberToObject(charger, STATE_CHARGER_HIGH_POWER_ENABLED, (double)STATE_CHARGER_HIGH_POWER_ENABLED_DEFAULT);
            cJSON_AddNumberToObject(charger, STATE_CHARGER_OVERHEATED_HYSTERESESIS, (double)STATE_CHARGER_OVERHEATED_HYSTERESESIS_DEFAULT);

            // current location by HTTP API
            cJSON *positioning = cJSON_CreateObject();
            cJSON_AddNumberToObject(positioning, "latitude", 0);
            cJSON_AddNumberToObject(positioning, "longitude", 0);
            cJSON_AddNumberToObject(positioning, "azimuth", 0);
            cJSON_AddNumberToObject(positioning, "elevation", 0);
            cJSON_AddNumberToObject(positioning, "angleToNorth", 0);
            cJSON_AddNumberToObject(positioning, "manualMode", false);
            cJSON_AddItemToObject(state, "positioning", positioning);

            // displayConfig
            cJSON *display = cJSON_CreateObject();
            for (int i = 0; i < NUM_DISPLAY_ITEMS; i++) {
                cJSON_AddNumberToObject(display, DISPLAY_NAMES[i], DEFAULT_DISPLAY_CONFIG.itemEnabled[i]);
            }
            cJSON_AddItemToObject(state, "display", display);

            // night shutdown
            cJSON *nightShutdown = cJSON_CreateObject();
            cJSON_AddNumberToObject(nightShutdown, "enabled", true);
            cJSON_AddNumberToObject(nightShutdown, "azimuth", 0);
            cJSON_AddNumberToObject(nightShutdown, "elevation", 0);
            cJSON_AddItemToObject(state, "nightShutdown", nightShutdown);

            // esp2 data
            cJSON *esp2 = cJSON_CreateObject();
            cJSON_AddNumberToObject(esp2, "azimuth", 0);
            cJSON_AddNumberToObject(esp2, "elevation", 0);
            cJSON_AddNumberToObject(esp2, "xGyro", 0);
            cJSON_AddNumberToObject(esp2, "yGyro", 0);
            cJSON_AddNumberToObject(esp2, "zGyro", 0);
            cJSON_AddNumberToObject(esp2, "heading", 0);
            cJSON_AddItemToObject(state, "esp2", esp2);

            // stats data
            cJSON *stats = cJSON_CreateObject();
            cJSON_AddNumberToObject(stats, "dailyProduction", 0);
            cJSON_AddItemToObject(state, "stats", stats);
        }
        xSemaphoreGive(writeStateMutex);
    } else {
        ESP_LOGI("state", "Unable to aquire semaphore for state creation\n\n");
        return ESP_FAIL;
    }

    return ESP_OK;
}

double getDoubleOfSubChildNullSave(const char *name, const char *subChildName, double defaultValue) {
    if (xSemaphorePoll(writeStateMutex)) {
        cJSON *child = cJSON_GetObjectItem(state, name);
        if (child != NULL) {
            cJSON *subChild = cJSON_GetObjectItem(child, subChildName);
            if (subChild != NULL) {
                xSemaphoreGive(writeStateMutex);
                return cJSON_GetNumberValue(subChild);
            } else {
                ESP_LOGI("state", "Subchild \"%s\" does not exist", subChildName);
                xSemaphoreGive(writeStateMutex);
                return defaultValue;
            }
        }
        xSemaphoreGive(writeStateMutex);
        ESP_LOGI("state", "Child \"%s\" does not exist", name);
    }
    return defaultValue;
}

double getSignelChildNullSave(char *name, double defaultValue) {
    if (xSemaphorePoll(writeStateMutex)) {
        cJSON *child = cJSON_GetObjectItem(state, name);
        if (child != NULL) {
            xSemaphoreGive(writeStateMutex);
            return cJSON_GetNumberValue(child);
        }
        xSemaphoreGive(writeStateMutex);
        ESP_LOGI("state", "Child \"%s\" does not exist", name);
    }
    return defaultValue;
}

void modifyDouble(const char *objName, const char *subChildName, double value) {
    if (xSemaphorePoll(writeStateMutex)) {
        cJSON *child = cJSON_GetObjectItem(state, objName);
        if (child != NULL) {
            cJSON *newItem = cJSON_CreateNumber(value);
            cJSON_ReplaceItemInObjectCaseSensitive(child, subChildName, newItem);
            toNotifyChange = true;
        } else {
            ESP_LOGI("state", "Child \"%s\" does not exist\n\n", objName);
        }
        xSemaphoreGive(writeStateMutex);
    } else {
        ESP_LOGI("state", "state: Could not aquire write semaphore\n\n");
    }
}

void modifySingle(char *objName, double value) {
    if (xSemaphorePoll(writeStateMutex)) {
        cJSON *child = cJSON_GetObjectItem(state, objName);
        if (child != NULL) {
            cJSON_SetNumberValue(child, value);
            toNotifyChange = true;
        } else {
            ESP_LOGI("state", "Child \"%s\" does not exist\n\n", objName);
        }
        xSemaphoreGive(writeStateMutex);
    } else {
        ESP_LOGI("state", "state: Could not aquire write semaphore\n\n");
    }
}

// /////////////////////////////////////////////////////////
// decalare SETTERS to all system relevant variables here:

void setUsbMode(USB_MODE usbState) {
    modifyDouble(STATE_OBJ_CHARGER, STATE_CHARGER_USBMODE, (double)usbState);
}

void setSolarChargerState(CHARGER_MODE chargerMode) {
    modifyDouble(STATE_OBJ_CHARGER, STATE_CHARGER_CHARGER_MODE, (double)chargerMode);
}
void setSolarCurrentTarget(uint16_t milliamps) {
    modifyDouble(STATE_OBJ_CHARGER, STATE_CHARGER_CURRENT_TARGET, (double)milliamps);
}

void setThresholdVoltage(uint16_t millivolts) {
    modifyDouble(STATE_OBJ_CHARGER, STATE_CHARGER_THRESHOLD_VOLTAGE, (double)millivolts);
}

void setMaximumVoltage(uint16_t millivolts) {
    modifyDouble(STATE_OBJ_CHARGER, STATE_CHARGER_MAXIMUM_VOLTAGE, (double)millivolts);
}

void setTrickleThreshold(uint16_t millivolts) {
    modifyDouble(STATE_OBJ_CHARGER, STATE_CHARGER_TRICKLE_THRESHOLD, (double)millivolts);
}

void setHighPowerCircuitEnabled(bool enabled) {
    modifyDouble(STATE_OBJ_CHARGER, STATE_CHARGER_HIGH_POWER_ENABLED, (double)enabled);
}

void setBatteryOverheatedTemperature(uint8_t temperature) {
    modifyDouble(STATE_OBJ_CHARGER, STATE_CHARGER_OVERHEATED_TEMPERATURE, (double)temperature);
}

void setBatteryOverheatedHysteresis(uint8_t temperature) {
    modifyDouble(STATE_OBJ_CHARGER, STATE_CHARGER_OVERHEATED_HYSTERESESIS, (double)temperature);
}

void setBatterySize(uint16_t mah) {
    modifyDouble(STATE_OBJ_CHARGER, STATE_CHARGER_BATTERY_SIZE, mah);
}

void setAutoAdjustPanel(bool isManual) {
    modifyDouble("positioning", "manualMode", isManual);
}

void setAngleToNorth(int16_t angle) {
    modifyDouble("positioning", "angleToNorth", angle);
}

void setPanelAzimuth(int16_t degree) {
    modifyDouble("positioning", "azimuth", degree);
}

void setPanelElevation(int16_t degree) {
    modifyDouble("positioning", "elevation", degree);
}

void setLatitude(int16_t lat) {
    modifyDouble("positioning", "latitude", lat);
}

void setLongitude(int16_t longitude) {
    modifyDouble("positioning", "longitude", longitude);
}

void setDailyProduction(int16_t dailyProduction) {
    modifyDouble("stats", "dailyProduction", dailyProduction);
}

void setDisplayConfig(display_config_t *displayConifg) {
    for (int i = 0; i < NUM_DISPLAY_ITEMS; i++) {
        modifyDouble("display", DISPLAY_NAMES[i], displayConifg->itemEnabled[i]);
    }
}

void setNightShutdownEnabled(bool enabled) {
    modifyDouble("nightShutdown", "enabled", enabled);
}

void setNightShutdownAzimuth(int16_t longitude) {
    modifyDouble("nightShutdown", "azimuth", longitude);
}

void setNightShutdownElevation(int16_t elevation) {
    modifyDouble("nightShutdown", "elevation", elevation);
}

void setEsp2Azimuth(float azimuth) {
    modifyDouble("esp2", "azimuth", azimuth);
}

void setEsp2Elevation(float elevation) {
    modifyDouble("esp2", "elevation", elevation);
}

void setEsp2xGyro(float xGyro) {
    modifyDouble("esp2", "xGyro", xGyro);
}

void setEsp2yGyro(float yGyro) {
    modifyDouble("esp2", "yGyro", yGyro);
}

void setEsp2zGyro(float zGyro) {
    modifyDouble("esp2", "zGyro", zGyro);
}

void setEsp2Heading(float heading) {
    modifyDouble("esp2", "heading", heading);
}

void setState(cJSON *json) {
    if (xSemaphorePoll(writeStateMutex) == pdTRUE) {
        cJSON_Delete(state);
        state = cJSON_Duplicate(json, 1);
        xSemaphoreGive(writeStateMutex);
    }
}

// /////////////////////////////////////////////////////////
// decalare GETTERS to all system relevant variables here:

// charger
USB_MODE getUsbMode() {
    return (USB_MODE)getDoubleOfSubChildNullSave(STATE_OBJ_CHARGER, STATE_CHARGER_USBMODE, STATE_CHARGER_USBMODE_DEFAULT);
}

CHARGER_MODE getSolarChargerState() {
    return (CHARGER_MODE)getDoubleOfSubChildNullSave(STATE_OBJ_CHARGER, STATE_CHARGER_CHARGER_MODE, STATE_CHARGER_CHARGER_MODE_DEFAULT);
}

uint16_t getSolarCurrentTarget() {
    return (uint16_t)getDoubleOfSubChildNullSave(STATE_OBJ_CHARGER, STATE_CHARGER_CURRENT_TARGET, STATE_CHARGER_CURRENT_TARGET_DEFAULT);
}

uint16_t getThresholdVoltage() {
    return (uint16_t)getDoubleOfSubChildNullSave(STATE_OBJ_CHARGER, STATE_CHARGER_THRESHOLD_VOLTAGE, STATE_CHARGER_THRESHOLD_VOLTAGE_DEFAULT);
}

uint16_t getMaximumVoltage() {
    return (uint16_t)getDoubleOfSubChildNullSave(STATE_OBJ_CHARGER, STATE_CHARGER_MAXIMUM_VOLTAGE, STATE_CHARGER_MAXIMUM_VOLTAGE_DEFAULT);
}

uint16_t getTrickleThreshold() {
    return (uint16_t)getDoubleOfSubChildNullSave(STATE_OBJ_CHARGER, STATE_CHARGER_TRICKLE_THRESHOLD, STATE_CHARGER_TRICKLE_THRESHOLD_DEFAULT);
}

bool isHighPowerModeEnabled() {
    return (bool)getDoubleOfSubChildNullSave(STATE_OBJ_CHARGER, STATE_CHARGER_HIGH_POWER_ENABLED, STATE_CHARGER_HIGH_POWER_ENABLED_DEFAULT);
}

uint8_t getBatteryOverheatedTemperature() {
    return (uint8_t)getDoubleOfSubChildNullSave(STATE_OBJ_CHARGER, STATE_CHARGER_OVERHEATED_TEMPERATURE, STATE_CHARGER_OVERHEATED_TEMPERATURE_DEFAULT);
}

uint8_t getBatteryOverheatedHysteresis() {
    return (uint8_t)getDoubleOfSubChildNullSave(STATE_OBJ_CHARGER, STATE_CHARGER_OVERHEATED_HYSTERESESIS, STATE_CHARGER_OVERHEATED_HYSTERESESIS_DEFAULT);
}

uint16_t getBatterySize() {
    return (uint16_t)getDoubleOfSubChildNullSave(STATE_OBJ_CHARGER, STATE_CHARGER_BATTERY_SIZE, STATE_CHARGER_BATTERY_SIZE_DEFAULT);
}

bool getAutoAdjustPanel() {
    return (bool)getDoubleOfSubChildNullSave("positioning", "manualMode", true);
}

int16_t getPanelElevation() {
    return (int16_t)getDoubleOfSubChildNullSave("positioning", "elevation", 0);
}

int16_t getPanelAzimuth() {
    return (int16_t)getDoubleOfSubChildNullSave("positioning", "azimuth", 0);
}

int16_t getAngleToNorth() {
    return (int16_t)getDoubleOfSubChildNullSave("positioning", "angleToNorth", 0);
}

void getState(cJSON **json) {
    if (xSemaphorePoll(writeStateMutex) == pdTRUE) {
        *json = cJSON_Duplicate(state, 1);
        xSemaphoreGive(writeStateMutex);
    }
}

int16_t getLatitude() {
    return (int16_t)getDoubleOfSubChildNullSave("positioning", "latitude", 0);
}

int16_t getLongitude() {
    return (int16_t)getDoubleOfSubChildNullSave("positioning", "longitude", 0);
}

int16_t getDailyProduction() {
    return (int16_t)getDoubleOfSubChildNullSave("stats", "dailyProduction", 0);
}

int16_t getElevation() {
    return (int16_t)getDoubleOfSubChildNullSave("positioning", "elevation", 0);
}

bool isNightShutdownEnabled() {
    return (bool)getDoubleOfSubChildNullSave("nightShutdown", "enabled", true);
}

int16_t getNightShutdownAzimuth() {
    return (int16_t)getDoubleOfSubChildNullSave("nightShutdown", "azimuth", 0);
}

int16_t getNightShutdownElevation() {
    return (int16_t)getDoubleOfSubChildNullSave("nightShutdown", "elevation", 0);
}

float getEsp2Azimuth() {
    return (float)getDoubleOfSubChildNullSave("esp2", "azimuth", 0);
}

float getEsp2Elevation() {
    return (float)getDoubleOfSubChildNullSave("esp2", "elevation", 0);
}

float getEsp2xGyro() {
    return (float)getDoubleOfSubChildNullSave("esp2", "xGyro", 0);
}
float getEsp2yGyro() {
    return (float)getDoubleOfSubChildNullSave("esp2", "yGyro", 0);
}
float getEsp2zGyro() {
    return (float)getDoubleOfSubChildNullSave("esp2", "zGyro", 0);
}

float getEsp2Heading() {
    return (float)getDoubleOfSubChildNullSave("esp2", "heading", 0);
}

// preallocate the result struct, ideally statically
void getDisplayConfig(display_config_t *result) {
    if (result == NULL) {
        ESP_LOGW("state", "The display struct must be preallocated");
    }

    for (int i = 0; i < NUM_DISPLAY_ITEMS; i++) {
        result->itemEnabled[i] = getDoubleOfSubChildNullSave("display", DISPLAY_NAMES[i], DEFAULT_DISPLAY_CONFIG.itemEnabled[i]);
    }
}
