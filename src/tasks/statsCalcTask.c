#include "tasks/statsCalcTask.h"

#include "cJSON.h"
#include "dbInterface.h"
#include "driver/gps.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "settings.h"

void statsCalcTask() {
    esp_err_t errCode = ESP_OK;

    cJSON* jsonOut = NULL;
    int64_t timestamp = 0;
    readings_read_config_t readConfig = {
        .startTimestamp = 0,
        .endTimestamp = 0,
        .getTimestamp = true,
        .getGpsLat = false,
        .getGpsLon = false,
        .getBatteryLevel = false,
        .getBatteryTemperature = false,
        .getChargerState = false,
        .getCompassBearing = false,
        .getCurrentIn = true,
        .getCurrentOut = true,
        .getGpsSatellitesInView = false,
        .getGpsUsedSatellites = false,
        .getGyroscopeAccelX = false,
        .getGyroscopeAccelY = false,
        .getGyroscopeAccelZ = false,
        .getGyroscopeTemperature = false,
        .getMagneticVariation = false,
        .getVoltageHigherCell = false,
        .getVoltageLowerCell = false,
    };

    char* nmea = NULL;
    size_t nmeaLen = 0;
    nmea_data_t data;
    nmea_data_type_t dataType = NONE;

    while (1) {
        errCode = gpsReceiveNmea(&nmea, &nmeaLen);
        if (errCode != ESP_OK) {
            ESP_LOGE("gps_test_task", "receiving nmea from module failed");
        }

        errCode = gpsParseNmea(nmea, nmeaLen, &data, &dataType);
        if (errCode != ESP_OK) {
            ESP_LOGE("gps_test_task", "nmea could not be parsed");
        }

        switch (dataType) {
            case RMC:
                timestamp = data.rmc.date.year * 10000000000 + data.rmc.date.month * 100000000 + data.rmc.date.day * 1000000;
            default:
                break;
        }

        if (nmea) {
            free(nmea);
            nmea = NULL;
        }
        nmeaLen = 0;
        dataType = NONE;

        uint16_t currentIn = 0;
        uint16_t currentOut = 0;

        int16_t dailyProduction = 0;

        if (timestamp) {
            readConfig.startTimestamp = timestamp;

            timestamp = 0;  // reset timestamp

            errCode = dbGetReadings(readConfig, &jsonOut);
            if (errCode != ESP_OK) {
                ESP_LOGE("statsCalcTask", "receiving readings failed");
            } else {
                if (jsonOut == NULL) {
                    ESP_LOGE("statsCalcTask", "JSON OUT IS NULL");
                }

                for (int i = 0; i < cJSON_GetArraySize(jsonOut); i++) {
                    cJSON* json = cJSON_GetArrayItem(jsonOut, i);
                    if (json == NULL) {
                        ESP_LOGE("statsCalcTask", "json is null");
                    } else {
                        cJSON* item = cJSON_GetObjectItem(json, "currentIn");
                        if (item != NULL) {
                            currentIn = (uint16_t)cJSON_GetNumberValue(item);
                        }

                        item = cJSON_GetObjectItem(json, "currentOut");

                        if (item != NULL) {
                            currentOut = (uint16_t)cJSON_GetNumberValue(item);
                        }

                        dailyProduction += currentIn;
                        dailyProduction -= currentOut;
                    }
                }

                cJSON_Delete(jsonOut);
                jsonOut = NULL;
            }

            vTaskDelay(STATS_CALC_DELAY);
        }
    }
}