#include "dbInterface.h"

#include <dirent.h>
#include <errno.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <string.h>
#include <sys/stat.h>

#include "settings.h"
#include "stringConcat.h"

SemaphoreHandle_t currentDateMutex = NULL;
SemaphoreHandle_t currentDateFileMutex = NULL;
SemaphoreHandle_t readAllFileMutex = NULL;
int32_t currentDate = 0;

esp_err_t dbInit() {
    // check if DB_FOLDER exists or create folder
    DIR* dbDir = opendir(PATH_CONCAT(SDC_MOUNT_POINT, DB_FOLDER));
    if (dbDir) {
        closedir(dbDir);
    } else {
        if (errno == ENOENT) {
            int rc = mkdir(PATH_CONCAT(SDC_MOUNT_POINT, DB_FOLDER), 0666);
            if (rc < 0) {
                ESP_LOGE("db_init", "failed to create database directory. Error: %d", errno);
                return ESP_FAIL;
            }
        } else {
            ESP_LOGE("db_init", "failed to open database directory. Error: %d", errno);
            return ESP_FAIL;
        }
    }

    currentDateMutex = xSemaphoreCreateMutex();
    if (!currentDateMutex) {
        return ESP_FAIL;
    }
    currentDateFileMutex = xSemaphoreCreateMutex();
    if (!currentDateFileMutex) {
        vSemaphoreDelete(currentDateMutex);
        currentDateMutex = NULL;
        return ESP_FAIL;
    }
    readAllFileMutex = xSemaphoreCreateMutex();
    if (!readAllFileMutex) {
        vSemaphoreDelete(currentDateMutex);
        currentDateMutex = NULL;
        vSemaphoreDelete(currentDateFileMutex);
        currentDateFileMutex = NULL;
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t dbDeinit() {
    vSemaphoreDelete(currentDateMutex);
    currentDateMutex = NULL;
    vSemaphoreDelete(currentDateFileMutex);
    currentDateFileMutex = NULL;
    vSemaphoreDelete(readAllFileMutex);
    readAllFileMutex = NULL;

    return ESP_OK;
}

esp_err_t dbStoreReadings(readings_data_set_t readings) {
    if (!readings.timestampExists) {
        ESP_LOGE("db_store_readings", "failed because timestamp is missing");
        return ESP_FAIL;
    }

    esp_err_t errCode = ESP_OK;

    // create file path
    int32_t date = (int32_t)(readings.timestamp / DB_TIMESTAMP_DATE_DEVIDER);
    char pathBuffer[50] = {0};
    char timestampBuffer[20] = {0};
    sprintf(timestampBuffer, "%ld", date);
    strcpy(pathBuffer, PATH_CONCAT(SDC_MOUNT_POINT, DB_FOLDER));
    strcat(pathBuffer, "/");
    strcat(pathBuffer, timestampBuffer);
    strcat(pathBuffer, ".csv");

    // set currentDate
    if (date != currentDate) {
        // obtain mutex
        if (xSemaphorePoll(currentDateMutex) == pdTRUE) {
            currentDate = date;
            xSemaphoreGive(currentDateMutex);
        } else {
            ESP_LOGE("db_store_readings", "failed to take currentDateMutex");
            return ESP_FAIL;
        }
    }

    // obtain mutex
    if (xSemaphorePoll(currentDateFileMutex) != pdTRUE) {
        ESP_LOGE("db_store_readings", "failed to take currentDateFileMutex");
        return ESP_FAIL;
    }

    // open file corresponding to the timestamp
    FILE* currentFile = fopen(pathBuffer, "a");
    if (!currentFile) {
        ESP_LOGE("db_store_readings", "failed to create or open file '%s'", pathBuffer);
        xSemaphoreGive(currentDateFileMutex);
        return ESP_FAIL;
    }

    // TimestampID [lld],ChargerState [u],CurrentIn [u],CurrentOut [u],BatteryLevel [u],VoltageLowerCell [u],VoltageHigherCell [u],BatteryTemperature [f],GyroscopeAccelX [f],GyroscopeAccelY [f],GyroscopeAccelZ [f],GyroscopeTemperature [f],GpsLat [f],GpsLon [f],GpsUsedSatellites [u],GpsSatellitesInView [u],CompassBearing [f],MagneticVariation [f]

    // store readings

    fprintf(currentFile, "%lld,", readings.timestamp);

    if (readings.chargerStateExists) {
        fprintf(currentFile, "%u,", readings.chargerState);
    } else {
        fprintf(currentFile, ",");
    }

    if (readings.currentInExists) {
        fprintf(currentFile, "%u,", readings.currentIn);
    } else {
        fprintf(currentFile, ",");
    }

    if (readings.currentOutExists) {
        fprintf(currentFile, "%u,", readings.currentOut);
    } else {
        fprintf(currentFile, ",");
    }

    if (readings.batteryLevelExists) {
        fprintf(currentFile, "%u,", readings.batteryLevel);
    } else {
        fprintf(currentFile, ",");
    }

    if (readings.voltageLowerCellExists) {
        fprintf(currentFile, "%u,", readings.voltageLowerCell);
    } else {
        fprintf(currentFile, ",");
    }

    if (readings.voltageHigherCellExists) {
        fprintf(currentFile, "%u,", readings.voltageHigherCell);
    } else {
        fprintf(currentFile, ",");
    }

    if (readings.batteryTemperatureExists) {
        fprintf(currentFile, "%f,", readings.batteryTemperature);
    } else {
        fprintf(currentFile, ",");
    }

    if (readings.gyroscopeAccelXExists) {
        fprintf(currentFile, "%f,", readings.gyroscopeAccelX);
    } else {
        fprintf(currentFile, ",");
    }

    if (readings.gyroscopeAccelYExists) {
        fprintf(currentFile, "%f,", readings.gyroscopeAccelY);
    } else {
        fprintf(currentFile, ",");
    }

    if (readings.gyroscopeAccelZExists) {
        fprintf(currentFile, "%f,", readings.gyroscopeAccelZ);
    } else {
        fprintf(currentFile, ",");
    }

    if (readings.gyroscopeTemperatureExists) {
        fprintf(currentFile, "%f,", readings.gyroscopeTemperature);
    } else {
        fprintf(currentFile, ",");
    }

    if (readings.gpsLatExists) {
        fprintf(currentFile, "%f,", readings.gpsLat);
    } else {
        fprintf(currentFile, ",");
    }

    if (readings.gpsLonExists) {
        fprintf(currentFile, "%f,", readings.gpsLon);
    } else {
        fprintf(currentFile, ",");
    }

    if (readings.gpsUsedSatellitesExists) {
        fprintf(currentFile, "%u,", readings.gpsUsedSatellites);
    } else {
        fprintf(currentFile, ",");
    }

    if (readings.gpsSatellitesInViewExists) {
        fprintf(currentFile, "%u,", readings.gpsSatellitesInView);
    } else {
        fprintf(currentFile, ",");
    }

    if (readings.compassBearingExists) {
        fprintf(currentFile, "%f,", readings.compassBearing);
    } else {
        fprintf(currentFile, ",");
    }

    if (readings.magneticVariationExists) {
        fprintf(currentFile, "%f\n", readings.magneticVariation);
    } else {
        fprintf(currentFile, "\n");
    }

    fclose(currentFile);
    xSemaphoreGive(currentDateFileMutex);

    return errCode;
}

esp_err_t dbGetReadings(readings_read_config_t config, cJSON** jsonOut) {
    if (*jsonOut) {
        ESP_LOGE("getdbreadings", "json not null");
        return ESP_FAIL;
    }
    // obtain read all mutex
    if (xSemaphorePoll(readAllFileMutex) != pdTRUE) {
        ESP_LOGE("db_get_readings", "failed to take readAllFileMutex");
        return ESP_FAIL;
    }

    int32_t startDate = 0;
    int32_t endDate = 0;
    int32_t localStartDate = INT32_MAX;
    int32_t localEndDate = INT32_MIN;

    // find first and last file
    DIR* dbDir = opendir(PATH_CONCAT(SDC_MOUNT_POINT, DB_FOLDER));

    if (dbDir) {
        struct dirent* nextFile;

        while ((nextFile = readdir(dbDir)) != NULL) {
            char* dateEndPtr = strchr(nextFile->d_name, '.');
            int32_t fileDate = 0;
            if (dateEndPtr) {
                // terminate string
                *dateEndPtr = '\0';
                fileDate = strtol(nextFile->d_name, NULL, 10);
                // set back to '.'
                *dateEndPtr = '.';
            }
            if (fileDate < localStartDate) {
                localStartDate = fileDate;
            }
            if (fileDate > localEndDate) {
                localEndDate = fileDate;
            }
        }

        closedir(dbDir);
    } else {
        ESP_LOGE("db_clear_readings", "failed to open database directory");
        xSemaphoreGive(readAllFileMutex);
        return ESP_FAIL;
    }

    if (!config.startTimestamp) {
        startDate = localStartDate;
        config.startTimestamp = INT64_MIN;
    } else {
        startDate = (int32_t)(config.startTimestamp / DB_TIMESTAMP_DATE_DEVIDER);
    }
    if (!config.endTimestamp) {
        endDate = localEndDate;
        config.endTimestamp = INT64_MAX;
    } else {
        endDate = (int32_t)(config.endTimestamp / DB_TIMESTAMP_DATE_DEVIDER);
    }

    *jsonOut = cJSON_CreateArray();
    if (*jsonOut == NULL) {
        ESP_LOGE("db_get_readings", "failed to create json root array");
        xSemaphoreGive(readAllFileMutex);
        return ESP_FAIL;
    }
    // if no columns are chosen return an empty JSON array
    if (!config.getBatteryLevel && !config.getBatteryTemperature && !config.getChargerState && !config.getCompassBearing && !config.getCurrentIn && !config.getCurrentOut && !config.getGpsLat && !config.getGpsLon && !config.getGpsSatellitesInView && !config.getGpsUsedSatellites && !config.getGyroscopeAccelX && !config.getGyroscopeAccelY && !config.getGyroscopeAccelZ && !config.getGyroscopeTemperature && !config.getMagneticVariation && !config.getTimestamp && !config.getVoltageHigherCell && !config.getVoltageLowerCell) {
        ESP_LOGI("db_get_readings", "no column selected");
        xSemaphoreGive(readAllFileMutex);
        return ESP_OK;
    }

    if ((startDate > localEndDate) || (endDate < localStartDate)) {
        ESP_LOGI("db_get_readings", "no readings for this section: startTimestamp: %d; endTimestamp: %d, localStart %d, localEnd %d", (int)startDate, (int)endDate, (int)localStartDate, (int)localEndDate);
        xSemaphoreGive(readAllFileMutex);
        return ESP_OK;
    }

    for (int32_t t = startDate; t <= endDate; t++) {
        // create file path
        char pathBuffer[50] = {0};
        char timestampBuffer[20] = {0};
        sprintf(timestampBuffer, "%ld", t);
        strcpy(pathBuffer, PATH_CONCAT(SDC_MOUNT_POINT, DB_FOLDER));
        strcat(pathBuffer, "/");
        strcat(pathBuffer, timestampBuffer);
        strcat(pathBuffer, ".csv");

        // obtain mutex if necessary
        bool fileMutexObtained = false;
        if (xSemaphorePoll(currentDateMutex) == pdTRUE) {
            int32_t localCurrentDate = currentDate;
            xSemaphoreGive(currentDateMutex);
            if (t == localCurrentDate) {
                if (xSemaphorePoll(currentDateFileMutex) == pdTRUE) {
                    fileMutexObtained = true;
                } else {
                    ESP_LOGE("db_get_readings", "failed to take currentDateFileMutex");
                    continue;
                }
            }
        } else {
            ESP_LOGE("db_get_readings", "failed to take currentDateMutex");
            continue;
        }
        // try to open file
        FILE* file = fopen(pathBuffer, "r");

        if (file) {
            char lineBuffer[DB_MAX_LINE_LENGTH] = {0};

            while (fgets(lineBuffer, DB_MAX_LINE_LENGTH, file)) {
                // get count of values
                uint16_t counter = 0;
                for (uint16_t i = 0; i < DB_MAX_LINE_LENGTH; i++) {
                    if (lineBuffer[i] == ',') {
                        counter++;
                    } else if (lineBuffer[i] == '\n') {
                        break;
                    }
                }

                if (counter != (DB_COL_COUNT - 1)) {
                    continue;
                }
                // parse line into json
                uint8_t wordSize = 0;
                char* wordPtr = lineBuffer;
                uint8_t readingsNumber = 0;
                cJSON* item = cJSON_CreateObject();
                bool isValidTimestamp = true;
                for (uint16_t i = 0; i < DB_MAX_LINE_LENGTH; i++) {
                    if ((lineBuffer[i] == ',') || (lineBuffer[i] == '\n')) {
                        wordSize = (uint8_t)(&lineBuffer[i] - wordPtr);

                        // set string terminator
                        lineBuffer[i] = '\0';

                        if ((readingsNumber == 0) && ((t == startDate) || (t == endDate))) {
                            // parse timestamp and check if it is in bounds else continue
                            int64_t timestamp = strtoll(wordPtr, NULL, 10);
                            if (((t == startDate) && (timestamp < config.startTimestamp)) || ((t == endDate) && (timestamp > config.endTimestamp))) {
                                isValidTimestamp = false;
                                break;
                            }
                        }

                        cJSON* innerItem = NULL;

                        if (wordSize) {
                            innerItem = cJSON_CreateString(wordPtr);
                        } else {
                            innerItem = cJSON_CreateNull();
                        }

                        cJSON_bool success = cJSON_True;
                        switch (readingsNumber) {
                            case 0:
                                // TimestampID
                                if (config.getTimestamp) {
                                    success = cJSON_AddItemToObject(item, DB_READINGS_COL_TIMESTAMP, innerItem);
                                } else {
                                    cJSON_Delete(innerItem);
                                    innerItem = NULL;
                                }
                                break;
                            case 1:
                                // ChargerState
                                if (config.getChargerState) {
                                    success = cJSON_AddItemToObject(item, DB_READINGS_COL_CHARGER_STATE, innerItem);
                                } else {
                                    cJSON_Delete(innerItem);
                                    innerItem = NULL;
                                }
                                break;
                            case 2:
                                // CurrentIn
                                if (config.getCurrentIn) {
                                    success = cJSON_AddItemToObject(item, DB_READINGS_COL_CURRENT_IN, innerItem);
                                } else {
                                    cJSON_Delete(innerItem);
                                    innerItem = NULL;
                                }
                                break;
                            case 3:
                                // CurrentOut
                                if (config.getCurrentOut) {
                                    success = cJSON_AddItemToObject(item, DB_READINGS_COL_CURRENT_OUT, innerItem);
                                } else {
                                    cJSON_Delete(innerItem);
                                    innerItem = NULL;
                                }
                                break;
                            case 4:
                                // BatteryLevel
                                if (config.getBatteryLevel) {
                                    success = cJSON_AddItemToObject(item, DB_READINGS_COL_BATTERY_LEVEL, innerItem);
                                } else {
                                    cJSON_Delete(innerItem);
                                    innerItem = NULL;
                                }
                                break;
                            case 5:
                                // VoltageLowerCell
                                if (config.getVoltageLowerCell) {
                                    success = cJSON_AddItemToObject(item, DB_READINGS_COL_VOLTAGE_LOWER_CELL, innerItem);
                                } else {
                                    cJSON_Delete(innerItem);
                                    innerItem = NULL;
                                }
                                break;
                            case 6:
                                // VoltageHigherCell
                                if (config.getVoltageHigherCell) {
                                    success = cJSON_AddItemToObject(item, DB_READINGS_COL_VOLTAGE_HIGHER_CELL, innerItem);
                                } else {
                                    cJSON_Delete(innerItem);
                                    innerItem = NULL;
                                }
                                break;
                            case 7:
                                // BatteryTemperature
                                if (config.getBatteryTemperature) {
                                    success = cJSON_AddItemToObject(item, DB_READINGS_COL_BATTERY_TEMPERATURE, innerItem);
                                } else {
                                    cJSON_Delete(innerItem);
                                    innerItem = NULL;
                                }
                                break;
                            case 8:
                                // GyroscopeAccelX
                                if (config.getGyroscopeAccelX) {
                                    success = cJSON_AddItemToObject(item, DB_READINGS_COL_GYROSCOPE_ACCEL_X, innerItem);
                                } else {
                                    cJSON_Delete(innerItem);
                                    innerItem = NULL;
                                }
                                break;
                            case 9:
                                // GyroscopeAccelY
                                if (config.getGyroscopeAccelY) {
                                    success = cJSON_AddItemToObject(item, DB_READINGS_COL_GYROSCOPE_ACCEL_Y, innerItem);
                                } else {
                                    cJSON_Delete(innerItem);
                                    innerItem = NULL;
                                }
                                break;
                            case 10:
                                // GyroscopeAccelZ
                                if (config.getGyroscopeAccelZ) {
                                    success = cJSON_AddItemToObject(item, DB_READINGS_COL_GYROSCOPE_ACCEL_Z, innerItem);
                                } else {
                                    cJSON_Delete(innerItem);
                                    innerItem = NULL;
                                }
                                break;
                            case 11:
                                // GyroscopeTemperature
                                if (config.getGyroscopeTemperature) {
                                    success = cJSON_AddItemToObject(item, DB_READINGS_COL_GYROSCOPE_TEMPERATURE, innerItem);
                                } else {
                                    cJSON_Delete(innerItem);
                                    innerItem = NULL;
                                }
                                break;
                            case 12:
                                // GpsLat
                                if (config.getGpsLat) {
                                    success = cJSON_AddItemToObject(item, DB_READINGS_COL_GPS_LAT, innerItem);
                                } else {
                                    cJSON_Delete(innerItem);
                                    innerItem = NULL;
                                }
                                break;
                            case 13:
                                // GpsLon
                                if (config.getGpsLon) {
                                    success = cJSON_AddItemToObject(item, DB_READINGS_COL_GPS_LON, innerItem);
                                } else {
                                    cJSON_Delete(innerItem);
                                    innerItem = NULL;
                                }
                                break;
                            case 14:
                                // GpsUsedSatellites
                                if (config.getGpsUsedSatellites) {
                                    success = cJSON_AddItemToObject(item, DB_READINGS_COL_GPS_USED_SATELLITES, innerItem);
                                } else {
                                    cJSON_Delete(innerItem);
                                    innerItem = NULL;
                                }
                                break;
                            case 15:
                                // GpsSatellitesInView
                                if (config.getGpsSatellitesInView) {
                                    success = cJSON_AddItemToObject(item, DB_READINGS_COL_GPS_SATELLITES_IN_VIEW, innerItem);
                                } else {
                                    cJSON_Delete(innerItem);
                                    innerItem = NULL;
                                }
                                break;
                            case 16:
                                // CompassBearing
                                if (config.getCompassBearing) {
                                    success = cJSON_AddItemToObject(item, DB_READINGS_COL_COMPASS_BEARING, innerItem);
                                } else {
                                    cJSON_Delete(innerItem);
                                    innerItem = NULL;
                                }
                                break;
                            case 17:
                                // MagneticVariation
                                if (config.getMagneticVariation) {
                                    success = cJSON_AddItemToObject(item, DB_READINGS_COL_MAGNETIC_VARIATION, innerItem);
                                } else {
                                    cJSON_Delete(innerItem);
                                    innerItem = NULL;
                                }
                                break;
                            default:
                                ESP_LOGE("db_get_readings", "failed to add JSON item to JSON object %d", (int)readingsNumber);
                                break;
                        }

                        if (!success) {
                            ESP_LOGE("db_get_readings", "failed to add JSON item to JSON object %d", (int)readingsNumber);
                            cJSON_Delete(innerItem);
                        }

                        if (readingsNumber >= (DB_COL_COUNT - 1)) {
                            break;
                        }

                        if ((i + 1) < DB_MAX_LINE_LENGTH) {
                            wordPtr = &lineBuffer[i + 1];
                            wordSize = 0;
                            readingsNumber++;
                        }
                    }
                }
                if (isValidTimestamp) {
                    if (!cJSON_AddItemToArray(*jsonOut, item)) {
                        ESP_LOGE("db_get_readings", "failed to add JSON object to JSON array");
                        cJSON_Delete(item);
                    }
                } else {
                    cJSON_Delete(item);
                }
            }

            fclose(file);
        }

        if (fileMutexObtained) {
            xSemaphoreGive(currentDateFileMutex);
        }
    }

    xSemaphoreGive(readAllFileMutex);
    return ESP_OK;
}

esp_err_t dbClearReadings() {
    // obtain read all mutex
    if (xSemaphorePoll(readAllFileMutex) != pdTRUE) {
        ESP_LOGE("db_clear_readings", "failed to take readAllFileMutex");
        return ESP_FAIL;
    }

    // obtain write mutex
    if (xSemaphorePoll(currentDateFileMutex) != pdTRUE) {
        ESP_LOGE("db_clear_readings", "failed to take currentDateFileMutex");
        xSemaphoreGive(readAllFileMutex);
        return ESP_FAIL;
    }

    // delete all files in folder
    esp_err_t errCode = ESP_OK;
    DIR* dbDir = opendir(PATH_CONCAT(SDC_MOUNT_POINT, DB_FOLDER));

    if (dbDir) {
        struct dirent* nextFile;
        char pathBuffer[275] = {0};

        while ((nextFile = readdir(dbDir)) != NULL) {
            sprintf(pathBuffer, "%s/%s", PATH_CONCAT(SDC_MOUNT_POINT, DB_FOLDER), nextFile->d_name);
            remove(pathBuffer);
        }

        closedir(dbDir);
    } else {
        ESP_LOGE("db_clear_readings", "failed to open database directory");
        errCode = ESP_FAIL;
    }

    xSemaphoreGive(currentDateFileMutex);
    xSemaphoreGive(readAllFileMutex);

    return errCode;
}

esp_err_t dbTrimBelowReadings(int32_t date) {
    // obtain read all mutex
    if (xSemaphorePoll(readAllFileMutex) != pdTRUE) {
        ESP_LOGE("db_clear_readings", "failed to take readAllFileMutex");
        return ESP_FAIL;
    }

    // obtain write mutex
    if (xSemaphorePoll(currentDateFileMutex) != pdTRUE) {
        ESP_LOGE("db_clear_readings", "failed to take currentDateFileMutex");
        xSemaphoreGive(readAllFileMutex);
        return ESP_FAIL;
    }

    // delete all files with name < date in folder
    esp_err_t errCode = ESP_OK;
    DIR* dbDir = opendir(PATH_CONCAT(SDC_MOUNT_POINT, DB_FOLDER));

    if (dbDir) {
        struct dirent* nextFile;
        char pathBuffer[275] = {0};

        while ((nextFile = readdir(dbDir)) != NULL) {
            char* dateEndPtr = strchr(nextFile->d_name, '.');
            int32_t fileDate = 0;
            if (dateEndPtr) {
                // terminate string
                *dateEndPtr = '\0';
                fileDate = strtol(nextFile->d_name, NULL, 10);
                // set back to '.'
                *dateEndPtr = '.';
            }

            if (fileDate <= date) {
                sprintf(pathBuffer, "%s/%s", PATH_CONCAT(SDC_MOUNT_POINT, DB_FOLDER), nextFile->d_name);
                remove(pathBuffer);
            }
        }

        closedir(dbDir);
    } else {
        ESP_LOGE("db_clear_readings", "failed to open database directory");
        errCode = ESP_FAIL;
    }

    xSemaphoreGive(currentDateFileMutex);
    xSemaphoreGive(readAllFileMutex);

    return errCode;
}
