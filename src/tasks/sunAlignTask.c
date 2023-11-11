#include "tasks/sunAlignTask.h"

#include "dcf77.h"
#include "nightShutdown.h"
#define DEBUG

void sunAlignTask() {
    esp_err_t errCode = ESP_OK;

    nightShutdownEvents = xEventGroupCreate();

    // compass variables
    int16_t xCompass;
    int16_t yCompass;
    int16_t zCompass;
    // int16_t calibrationData[3][2] = {{1642, -8082}, {5622, -6640}, {5245, -6122}}; old data
    int16_t calibrationData[3][2] = {{7395, -4587}, {4377, -7770}, {6570, -4815}};
    int16_t xCorr = ((calibrationData[0][0] + calibrationData[0][1]) / 2);
    int16_t yCorr = ((calibrationData[1][0] + calibrationData[1][1]) / 2);
    int16_t zCorr = ((calibrationData[2][0] + calibrationData[2][1]) / 2);

    // gyro variables
    float xGyro;
    float yGyro;
    float zGyro;

    // dcf77 valiables
    dcf77_date_t dcf77_date;

    // gps variables
    char* nmea = NULL;
    size_t nmeaLen;
    nmea_data_t nmeaData;
    nmea_data_t rmcNmeaData;
    nmea_data_type_t nmeaDataType = NONE;
    location_t location = {0, 0};
    float magneticVariation = 0;
    bool rmcRead = false;

    uint8_t second = 0;
    uint8_t minute = 0;
    uint8_t hour = 0;
    uint8_t day = 0;
    uint8_t month = 0;
    uint32_t year = 0;

    sun_direction_t sunDirection = {0, 0};

    // servo data
    // float angleRotate;
    // float angleTilt;
    uint8_t gpsPolls = 0;
    float heading = 0;
    float nightShutdownTimeLeft = 0.0;

    while (1) {
        if (isNightShutdownEnabled() && isNightShutdown(&nightShutdownTimeLeft, location.latitude, location.longitude, month, day, hour, minute)) {
            setEsp2Azimuth((float)getNightShutdownAzimuth());
            setEsp2Elevation((float)getNightShutdownElevation());
            vTaskDelay(nightShutdownTimeLeft * 3600 * 1000 / portTICK_PERIOD_MS);
        }
        // get data
        if (compassModuleIsInitialized) {
            // compass data
            errCode = compassModuleChangeMode(CM_MODE_CONTINUOUS_MEASUREMENT);
            if (errCode != ESP_OK) {
                ESP_LOGE("sunAligntask", "mode change failed");
            } else {
                errCode = compassModuleReadAxis(&xCompass, &yCompass, &zCompass);
                if (errCode != ESP_OK) {
                    ESP_LOGE("sunAlign_task", "read axis failed");
                }
            }
            errCode = compassModuleChangeMode(CM_MODE_STANDBY);
            if (errCode != ESP_OK) {
                ESP_LOGE("sunAlign_task", "mode change failed");
            }

            heading = getHeading(xCompass, xCorr, yCompass, yCorr, zCompass, zCorr, xGyro, yGyro, zGyro) - 100;
        }
        if (!compassModuleIsInitialized || errCode != ESP_OK) {
            ESP_LOGE("sunAlignTask", "no live compass data");
            xCompass = 0;
            yCompass = 0;
            zCompass = 0;
            heading = 0;
            errCode = ESP_OK;
        }

        if (gyroscopeIsInitialized) {
            // gyro data
            errCode = gyroscopeSetSleepEnable(false);
            if (errCode != ESP_OK) {
                ESP_LOGE("sunAlign_task", "mode change failed");
            } else {
                errCode = gyroscopeGetAcceleration(&xGyro, &yGyro, &zGyro);
                if (errCode != ESP_OK) {
                    ESP_LOGE("sunAlign_task", "read acceleration failed");
                }
            }
            errCode = gyroscopeSetSleepEnable(true);
            if (errCode != ESP_OK) {
                ESP_LOGE("sunAlign_task", "mode change failed");
            }
        }
        if (!gyroscopeIsInitialized || errCode != ESP_OK) {
            ESP_LOGE("sunAlignTask", "no live gyro data");
            xGyro = 0;
            yGyro = 0;
            zGyro = 1;
            errCode = ESP_OK;
        }

        if (gpsIsInitialized) {
            // gps data
            do {
                nmeaDataType = NONE;

                errCode = gpsReceiveNmea(&nmea, &nmeaLen);
                if (errCode != ESP_OK) {
                    ESP_LOGE("sunAlign_task", "failed to receive nmea");
                    continue;
                }

                errCode = gpsParseNmea(nmea, nmeaLen, &nmeaData, &nmeaDataType);
                if (errCode != ESP_OK) {
                    ESP_LOGE("sunAlign_task", "failed to parse nmea");
                    continue;
                }

                if (nmea) {
                    free(nmea);
                    nmea = NULL;
                }
                nmeaLen = 0;

                if (nmeaDataType == RMC) {
                    rmcNmeaData = nmeaData;
                    rmcRead = true;
                }
                gpsPolls++;
            } while ((!rmcRead) && (gpsPolls < 10));
            gpsPolls = 0;
            if (!rmcRead) {
                // no rmcRead data
            } else {
                if (rmcNmeaData.rmc.status == ACTIVE_STATUS) {
                    // latitude
                    if (rmcNmeaData.rmc.northSouthIndicator == 'N') {
                        location.latitude = rmcNmeaData.rmc.latitude;
                    }
                    if (rmcNmeaData.rmc.northSouthIndicator == 'S') {
                        location.latitude = -1 * rmcNmeaData.rmc.latitude;
                    }
                    // longitude
                    if (rmcNmeaData.rmc.eastWestIndicator == 'E') {
                        location.longitude = rmcNmeaData.rmc.longitude;
                    }
                    if (rmcNmeaData.rmc.eastWestIndicator == 'W') {
                        location.longitude = -1 * rmcNmeaData.rmc.longitude;
                    }

                    if (rmcNmeaData.rmc.eastwestMagneticVariation == 'E') {
                        magneticVariation = rmcNmeaData.rmc.magneticVariation;
                    } else if (rmcNmeaData.rmc.eastwestMagneticVariation == 'W') {
                        magneticVariation = -1 * rmcNmeaData.rmc.magneticVariation;
                    }
                    heading += magneticVariation;
                }

                // // time and date
                year = rmcNmeaData.rmc.date.year;
                month = rmcNmeaData.rmc.date.month;
                day = rmcNmeaData.rmc.date.day;
                hour = rmcNmeaData.rmc.time.hours;
                minute = rmcNmeaData.rmc.time.minutes;
                second = rmcNmeaData.rmc.time.seconds;
            }
        }
        if (!gpsIsInitialized || errCode != ESP_OK) {
            ESP_LOGE("sunAlign_task", "no live gps data");
            location.latitude = 40;
            location.longitude = 12;
            year = 2023;
            month = 6;
            day = 20;
            hour = 15;
            minute = 0;
            second = 0;
            errCode = ESP_OK;
            // dcf77 data
            if (dcf77IsInitialized) {
                bool valid = dcf77ReadTime(&dcf77_date);
                if (!valid) {
                    ESP_LOGE("sunAlignTask", "dcf77 date not valid");
                }
                year = dcf77_date.year;
                month = dcf77_date.month;
                day = dcf77_date.day;
                hour = dcf77_date.hour;
                minute = dcf77_date.minute;
                second = 0;
            } else {
                ESP_LOGE("sunAlignTask", "no live dcf77 data");
                dcf77_date.minute = 0;
                dcf77_date.hour = 0;
                dcf77_date.day = 0;
                dcf77_date.weekDay = 0;
                dcf77_date.month = 0;
                dcf77_date.year = 0;
                dcf77_date.dst = false;
                dcf77_date.leapYear = false;
            }
        }
        int16_t timezoneOffset = 1;
        if (((month == 2 && day < 26) || month < 2) || ((month == 9 && day > 29) || month > 9)) {
            timezoneOffset = 1;
        } else {
            timezoneOffset = 2;
        }
#ifdef DEBUG
        ESP_LOGI("", "sensordata:\n");
        ESP_LOGI("", "lat: %lf lon: %lf\n", location.latitude, location.longitude);
        ESP_LOGI("", "year: %lu month: %u day: %u hour: %u minute: %u second: %u\n", year, month, day, hour, minute, second);
        ESP_LOGI("", "cx: %i cy: %i cz: %i heading: %f\n", xCompass, yCompass, zCompass, heading);
        ESP_LOGI("", "gx: %f gy: %f gz: %f\n", xGyro, yGyro, zGyro);
#endif
        if (!getAutoAdjustPanel()) {
            sunDirection = calculateSunDirection(location, year + 2000, month, day, hour, minute, second, timezoneOffset);
            sunDirection.azimuthAngle += heading;
            if (sunDirection.azimuthAngle < 0) {
                sunDirection.azimuthAngle += 360;
            }
#ifdef DEBUG
            ESP_LOGI("", "azi: %lf ele: %lf\n", sunDirection.azimuthAngle, sunDirection.elevationAngle);
#endif
            setEsp2Azimuth(sunDirection.azimuthAngle);
            setEsp2Elevation(sunDirection.elevationAngle);
        } else {
            setEsp2Azimuth((float)getPanelAzimuth() + (float)getAngleToNorth());
            setEsp2Elevation((float)getPanelElevation());
            ESP_LOGI("", "set %f %f\n", getEsp2Azimuth(), getEsp2Elevation());
        }
        setEsp2xGyro(xGyro);
        setEsp2yGyro(yGyro);
        setEsp2zGyro(zGyro);
        ESP_LOGI("COMPASS", "%f variation %f", heading, magneticVariation);
        setEsp2Heading(heading);

        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
}
