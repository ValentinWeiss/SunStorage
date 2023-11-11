#include "tasks/storeReadingsTask.h"

#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "adcReader.h"
#include "dbInterface.h"
#include "dcf77.h"
#include "driver/barometer.h"
#include "driver/gps.h"
#include "driver/gyroscope.h"
#include "initTracker.h"
#include "settings.h"
#include "soc.h"
#include "state.h"
#include "sunUtils.h"

void storeReadingsTask() {
    if (!dbIsInitialized) {
        ESP_LOGE("store_readings_task", "database is not initialized");
        while (1) {
            vTaskDelay(60000 / portTICK_PERIOD_MS);
        }
    }

    esp_err_t errCode = ESP_OK;

    char* nmea = NULL;
    size_t nmeaLen = 0;
    nmea_data_t nmeaData;
    nmea_data_t rmcNmeaData, ggaNmeaData, gsvNmeaData;
    bool rmcRead = false, ggaRead = false, gsvRead = false;
    nmea_data_type_t nmeaDataType = NONE;
    bool gpsTimestampFound = true;

    float temperatureBattery;
    float temperatureGyroscope;

    float accelX, accelY, accelZ;

    while (1) {
        readings_data_set_t readingsDataSet = {0};

        // read all relevant readings if initialised successfully

        if (gpsIsInitialized) {
            // reset nmeaDatas
            rmcRead = false;
            ggaRead = false;
            gsvRead = false;

            uint8_t i = 0;

            do {
                nmeaDataType = NONE;

                errCode = gpsReceiveNmea(&nmea, &nmeaLen);
                if (errCode != ESP_OK) {
                    ESP_LOGE("store_readings_task", "failed to receive nmea");
                    break;
                }

                errCode = gpsParseNmea(nmea, nmeaLen, &nmeaData, &nmeaDataType);
                if (errCode != ESP_OK) {
                    ESP_LOGI("store_readings_task", "failed to parse nmea");
                    if (nmea) {
                        free(nmea);
                        nmea = NULL;
                    }
                    nmeaLen = 0;
                    continue;
                }

                if (nmea) {
                    free(nmea);
                    nmea = NULL;
                }
                nmeaLen = 0;

                switch (nmeaDataType) {
                    case RMC:
                        rmcNmeaData = nmeaData;
                        rmcRead = true;
                        break;
                    case GGA:
                        ggaNmeaData = nmeaData;
                        ggaRead = true;
                        break;
                    case GSV:
                        gsvNmeaData = nmeaData;
                        gsvRead = true;
                        break;
                    default:
                        break;
                }

                i++;
            } while ((!rmcRead || !ggaRead || !gsvRead) && (i < STORE_READINGS_TASK_MAX_GPS_READS));

            if (rmcRead && rmcNmeaData.rmc.date.day) {
                // set timestamp from gps
                gpsTimestampFound = true;
                readingsDataSet.timestampExists = true;
                readingsDataSet.timestamp = (((int64_t)rmcNmeaData.rmc.date.year + 2000) * 10000000000) + ((int64_t)rmcNmeaData.rmc.date.month * 100000000) + ((int64_t)rmcNmeaData.rmc.date.day * 1000000) + ((int64_t)rmcNmeaData.rmc.time.hours * 10000) + ((int64_t)rmcNmeaData.rmc.time.minutes * 100) + ((int64_t)rmcNmeaData.rmc.time.seconds);

                // if valid
                if (rmcNmeaData.rmc.status == ACTIVE_STATUS) {
                    // gpsLat
                    readingsDataSet.gpsLatExists = true;
                    if (rmcNmeaData.rmc.northSouthIndicator == 'N') {
                        readingsDataSet.gpsLat = rmcNmeaData.rmc.latitude;
                    } else if (rmcNmeaData.rmc.northSouthIndicator == 'S') {
                        readingsDataSet.gpsLat = -1 * rmcNmeaData.rmc.latitude;
                    } else {
                        readingsDataSet.gpsLatExists = false;
                    }

                    // gpsLon
                    readingsDataSet.gpsLonExists = true;
                    if (rmcNmeaData.rmc.eastWestIndicator == 'E') {
                        readingsDataSet.gpsLon = rmcNmeaData.rmc.longitude;
                    } else if (rmcNmeaData.rmc.eastWestIndicator == 'W') {
                        readingsDataSet.gpsLon = -1 * rmcNmeaData.rmc.longitude;
                    } else {
                        readingsDataSet.gpsLonExists = false;
                    }

                    // magneticVariation
                    if (rmcNmeaData.rmc.eastwestMagneticVariation == 'E') {
                        readingsDataSet.magneticVariationExists = true;
                        readingsDataSet.magneticVariation = rmcNmeaData.rmc.magneticVariation;
                    } else if (rmcNmeaData.rmc.eastwestMagneticVariation == 'W') {
                        readingsDataSet.magneticVariationExists = true;
                        readingsDataSet.magneticVariation = -1 * rmcNmeaData.rmc.magneticVariation;
                    }
                }

                // gpsUsedSatellites from GGA
                if (ggaRead) {
                    readingsDataSet.gpsUsedSatellitesExists = true;
                    readingsDataSet.gpsUsedSatellites = ggaNmeaData.gga.satellitesNumber;
                }

                // gpsSatellitesInView from GSV
                if (gsvRead) {
                    readingsDataSet.gpsSatellitesInViewExists = true;
                    readingsDataSet.gpsSatellitesInView = gsvNmeaData.gsv.numberOfSatellitesInView;
                }
            } else {
                gpsTimestampFound = false;
            }
        } else {
            gpsTimestampFound = false;
        }

        // use manual input if timestamp is not available
        if (!gpsTimestampFound) {
            ESP_LOGW("store_readings_task", "no timestamp from gps module");

            // set timestamp from dcf77 if available
            bool timeStampIsSet = false;
            if (dcf77IsInitialized) {
                dcf77_date_t date = {0};
                if (dcf77ReadTime(&date)) {
                    readingsDataSet.timestampExists = true;
                    readingsDataSet.timestamp = ((int64_t)date.year * 10000000000) + ((int64_t)date.month * 100000000) + ((int64_t)date.day * 1000000) + ((int64_t)date.hour * 10000) + ((int64_t)date.minute * 100);  // + ((int64_t)date.second);
                    timeStampIsSet = true;
                }
            }
            if (!timeStampIsSet) {
                ESP_LOGW("store_readings_task", "no timestamp available");
                vTaskDelay(5000 / portTICK_PERIOD_MS);
                continue;
            }
        }

        if (systemStateIsInitialized) {
            // chargerState
            readingsDataSet.chargerStateExists = true;
            readingsDataSet.chargerState = (uint8_t)getSolarChargerState();
        }

        if (adcIsInitialized) {
            // currentIn

            readingsDataSet.currentInExists = true;
            readingsDataSet.currentIn = getCurrentSolar();

            // currentOut

            readingsDataSet.currentOutExists = true;
            readingsDataSet.currentOut = getCurrentSystem();

            // batteryLevel

            readingsDataSet.batteryLevelExists = true;
            readingsDataSet.batteryLevel = getHybridStateOfCharge();

            // voltageLowerCell

            readingsDataSet.voltageLowerCellExists = true;
            readingsDataSet.voltageLowerCell = getVoltageLowerCell();

            // voltageHigherCell

            readingsDataSet.voltageHigherCellExists = true;
            readingsDataSet.voltageHigherCell = getVoltageHigherCell();
        }

        if (barometerIsInitialized) {
            // batteryTemperature
            errCode = barometerMeasure(&temperatureBattery, NULL);
            if (errCode == ESP_OK) {
                readingsDataSet.batteryTemperatureExists = true;
                readingsDataSet.batteryTemperature = temperatureBattery;
            } else {
                ESP_LOGE("store_readings_task", "failed to read battery temperature");
            }
        }

        if (gyroscopeIsInitialized) {
            errCode = gyroscopeSetSleepEnable(false);
            if (errCode != ESP_OK) {
                ESP_LOGE("store_readings_task", "failed to change gyroscope mode to active");
            } else {
                errCode = gyroscopeGetAcceleration(&accelX, &accelY, &accelZ);
                if (errCode == ESP_OK) {
                    // gyroscopeAccelX
                    readingsDataSet.gyroscopeAccelXExists = true;
                    readingsDataSet.gyroscopeAccelX = accelX * GY_SENSOR_GRAVITY;

                    // gyroscopeAccelY
                    readingsDataSet.gyroscopeAccelYExists = true;
                    readingsDataSet.gyroscopeAccelY = accelY * GY_SENSOR_GRAVITY;

                    // gyroscopeAccelZ
                    readingsDataSet.gyroscopeAccelZExists = true;
                    readingsDataSet.gyroscopeAccelZ = accelZ * GY_SENSOR_GRAVITY;
                } else {
                    ESP_LOGE("store_readings_task", "failed to read gyroscope acceleration");
                }

                errCode = gyroscopeGetTemperature(&temperatureGyroscope);
                if (errCode == ESP_OK) {
                    // gyroscopeTemperature
                    readingsDataSet.gyroscopeTemperatureExists = true;
                    readingsDataSet.gyroscopeTemperature = temperatureGyroscope;
                } else {
                    ESP_LOGE("store_readings_task", "failed to read gyroscope temperature");
                }

                errCode = gyroscopeSetSleepEnable(true);
                if (errCode != ESP_OK) {
                    ESP_LOGE("store_readings_task", "failed to change gyroscope mode to sleep");
                }
            }
        }

        // compassBearing
        readingsDataSet.compassBearingExists = true;
        readingsDataSet.compassBearing = getPanelOrientation();

        // store readings in db
        if (dbStoreReadings(readingsDataSet) != ESP_OK) {
            ESP_LOGE("store_readings_task", "failed to store readings");
        } else {
            ESP_LOGI("store_readings_task", "write succeded");
        }

        vTaskDelay(STORE_READINGS_TASK_DELAY);
    }
}
