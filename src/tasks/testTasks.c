#include "tasks/testTasks.h"

#include "adcReader.h"
#include "dbInterface.h"
#include "driver/barometer.h"
#include "driver/compass.h"
#include "driver/gps.h"
#include "driver/gyroscope.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "math.h"
#include "sdCardService.h"
#include "settings.h"

void HelloWorldTestTask() {
    esp_err_t ret = ESP_OK;

    // Initialize serial port
    ret = uart_driver_install(UART_NUM_0, 256, 0, 0, NULL, 0);
    if (ret != ESP_OK) {
        ESP_LOGE("app_main", "Failed to install UART driver");
        return;
    }
    while (1) {
        // Send "Hello, world!" to serial port
        printf("Hello, world!\n");
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}

void compassTestTask() {
    esp_err_t errCode = ESP_OK;

    errCode = uart_driver_install(UART_NUM_0, 256, 0, 0, NULL, 0);
    if (errCode != ESP_OK) {
        ESP_LOGE("compass_test_task", "failed to install UART driver");
        return;
    }

    errCode = i2cInit();
    if (errCode != ESP_OK) {
        ESP_LOGE("compass_test_task", "initialization of i2c failed");
        while (1) {
            vTaskDelay(5000 / portTICK_PERIOD_MS);
        }
    }

    errCode = compassModuleInit();
    if (errCode != ESP_OK) {
        ESP_LOGE("compass_test_task", "initialization of compass module failed");
        while (1) {
            vTaskDelay(5000 / portTICK_PERIOD_MS);
        }
    }

    vTaskDelay(1000 / portTICK_PERIOD_MS);

    int16_t x = 0, y = 0, z = 0, t = 0;
    while (1) {
        errCode = compassModuleChangeMode(CM_MODE_CONTINUOUS_MEASUREMENT);
        if (errCode != ESP_OK) {
            ESP_LOGE("compass_test_task", "mode change failed");
        } else {
            errCode = compassModuleReadAxis(&x, &y, &z);
            if (errCode != ESP_OK) {
                ESP_LOGE("compass_test_task", "read axis failed");
            } else {
                printf("Axis:\n\tx: %d\n\ty: %d\n\tz: %d\n\n", x, y, z);
            }

            errCode = compassModuleReadTemperature(&t);
            if (errCode != ESP_OK) {
                ESP_LOGE("compass_test_task", "read temperatur failed");
            } else {
                printf("Temperatur:\n\tt: %d°C\n\n", t);
            }
        }

        errCode = compassModuleChangeMode(CM_MODE_STANDBY);
        if (errCode != ESP_OK) {
            ESP_LOGE("compass_test_task", "mode change failed");
        }

        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}

void gyroscopeTestTask() {
    esp_err_t errCode = ESP_OK;

    errCode = uart_driver_install(UART_NUM_0, 256, 0, 0, NULL, 0);
    if (errCode != ESP_OK) {
        ESP_LOGE("gyroscope_test_task", "failed to install UART driver");
        return;
    }

    errCode = i2cInit();
    if (errCode != ESP_OK) {
        ESP_LOGE("gyroscope_test_task", "initialization of i2c failed");
        while (1) {
            vTaskDelay(5000 / portTICK_PERIOD_MS);
        }
    }

    errCode = gyroscopeTestConnection();
    if (errCode != ESP_OK) {
        ESP_LOGE("gyroscope_test_task", "connection test failed");
        while (1) {
            vTaskDelay(5000 / portTICK_PERIOD_MS);
        }
    }

    errCode = gyroscopeInit();
    if (errCode != ESP_OK) {
        ESP_LOGE("gyroscope_test_task", "initialization of gyroscope module failed");
        while (1) {
            vTaskDelay(5000 / portTICK_PERIOD_MS);
        }
    }

    gyroscopeSetAccelRange(GY_ACCEL_RANGE_8_G);
    gyroscopeSetGyroRange(GY_GYRO_RANGE_500_DEG);
    gyroscopeSetFilterBandwidth(GY_BAND_21_HZ);

    vTaskDelay(1000 / portTICK_PERIOD_MS);

    float xa = 0, ya = 0, za = 0, xg = 0, yg = 0, zg = 0, t = 0;
    while (1) {
        errCode = gyroscopeSetSleepEnable(false);
        if (errCode != ESP_OK) {
            ESP_LOGE("gyroscope_test_task", "mode change failed");
        } else {
            vTaskDelay(10 / portTICK_PERIOD_MS);
            errCode = gyroscopeGetAcceleration(&xa, &ya, &za);
            if (errCode != ESP_OK) {
                ESP_LOGE("gyroscope_test_task", "read acceleration failed");
            } else {
                printf("Acceleration:\n\tx: %f m/s^2\n\ty: %f m/s^2\n\tz: %f m/s^2\n\n", xa * GY_SENSOR_GRAVITY, ya * GY_SENSOR_GRAVITY, za * GY_SENSOR_GRAVITY);
            }

            errCode = gyroscopeGetTemperature(&t);
            if (errCode != ESP_OK) {
                ESP_LOGE("gyroscope_test_task", "read temperatur failed");
            } else {
                printf("Temperatur:\n\tt: %f°C\n\n", t);
            }

            errCode = gyroscopeGetRotation(&xg, &yg, &zg);
            if (errCode != ESP_OK) {
                ESP_LOGE("gyroscope_test_task", "read gyro failed");
            } else {
                printf("Gyroscope:\n\tx: %f rad/s\n\ty: %f rad/s\n\tz: %f rad/s\n\n", xg * GY_DEGPS_TO_RADPS, yg * GY_DEGPS_TO_RADPS, zg * GY_DEGPS_TO_RADPS);
            }
        }

        errCode = gyroscopeSetSleepEnable(true);
        if (errCode != ESP_OK) {
            ESP_LOGE("gyroscope_test_task", "mode change failed");
        }

        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}

void gpsTestTask() {
    esp_err_t errCode = ESP_OK;
    char* nmea = NULL;
    size_t nmeaLen = 0;
    nmea_data_t data;
    nmea_data_type_t dataType = NONE;

    errCode = gpsInit();
    if (errCode != ESP_OK) {
        ESP_LOGE("gps_test_task", "initialization of gps module failed");
        while (1) {
            vTaskDelay(5000 / portTICK_PERIOD_MS);
        }
    }

    while (1) {
        errCode = gpsReceiveNmea(&nmea, &nmeaLen);
        if (errCode != ESP_OK) {
            ESP_LOGE("gps_test_task", "receiving nmea from module failed");
        }

        // char nmeaBuffer[GPS_NMEA_MAX_SENTENCE_LENGTH] = {0};
        // memcpy(nmeaBuffer, nmea, nmeaLen);
        // printf("NMEA: %s LEN: %d\n", nmeaBuffer, nmeaLen);

        errCode = gpsParseNmea(nmea, nmeaLen, &data, &dataType);
        if (errCode != ESP_OK) {
            ESP_LOGE("gps_test_task", "nmea could not be parsed");
        }

        switch (dataType) {
            case GGA:
                printf("Time: %dh %dm %ds\n", data.gga.time.hours, data.gga.time.minutes, data.gga.time.seconds);
                printf("Coordinates: %f%c %f%c\n", data.gga.latitude, data.gga.northSouthIndicator, data.gga.longitude, data.gga.eastWestIndicator);
                printf("%d satellites found\n", data.gga.satellitesNumber);
                if (data.gga.fixIndicator != FIX_NOT_AVAILABLE_OR_INVALID) {
                    printf("messurement valid\n");
                } else {
                    printf("messurement invalid\n");
                }
                break;
            default:
                break;
        }

        if (nmea) {
            free(nmea);
            nmea = NULL;
        }
        nmeaLen = 0;
        dataType = NONE;

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void barometerTestTask() {
    esp_err_t errCode = ESP_OK;

    errCode = uart_driver_install(UART_NUM_0, 256, 0, 0, NULL, 0);
    if (errCode != ESP_OK) {
        ESP_LOGE("barometer_test_task", "failed to install UART driver");
        return;
    }

    errCode = i2cInit();
    if (errCode != ESP_OK) {
        ESP_LOGE("barometer_test_task", "initialization of i2c failed");
        while (1) {
            vTaskDelay(5000 / portTICK_PERIOD_MS);
        }
    }

    errCode = barometerTestConnection();
    if (errCode != ESP_OK) {
        ESP_LOGE("barometer_test_task", "connection test failed");
        while (1) {
            vTaskDelay(5000 / portTICK_PERIOD_MS);
        }
    }

    errCode = barometerInit();
    if (errCode != ESP_OK) {
        ESP_LOGE("barometer_test_task", "initialization of barometer module failed");
        while (1) {
            vTaskDelay(5000 / portTICK_PERIOD_MS);
        }
    }

    vTaskDelay(1000 / portTICK_PERIOD_MS);

    float temperature = 0.0f;
    int32_t pressure = 0;
    while (1) {
        errCode = barometerMeasure(&temperature, &pressure);
        if (errCode != ESP_OK) {
            ESP_LOGE("barometer_test_task", "measurement failed");
        } else {
            printf("Temperature: %f°C\nPressure: %ld Pa\n", temperature, pressure);
        }

        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}

void databaseTestTask() {
    esp_err_t errCode = ESP_OK;
    errCode = sdCardInit();

    if (errCode != ESP_OK) {
        ESP_LOGE("database_test_task", "failed to init sd card");
        return;
    }

    errCode = uart_driver_install(UART_NUM_0, 256, 0, 0, NULL, 0);
    if (errCode != ESP_OK) {
        ESP_LOGE("database_test_task", "failed to install UART driver");
        return;
    }

    errCode = dbInit();
    if (errCode != ESP_OK) {
        ESP_LOGE("database_test_task", "initialization of database failed");
        while (1) {
            vTaskDelay(5000 / portTICK_PERIOD_MS);
        }
    } else {
        ESP_LOGI("database_test_task", "initialization of database successful");
    }

    errCode = dbClearReadings();
    if (errCode != ESP_OK) {
        ESP_LOGE("database_test_task", "clear readings failed");
    }

    cJSON* jsonOut = NULL;
    int64_t timestamp = 20230603220000;
    readings_data_set_t readings = {0};
    readings_read_config_t readConfig = {
        .startTimestamp = 0,
        .endTimestamp = 0,
        .getTimestamp = true,
        .getGpsLat = true,
        .getGpsLon = true,
        .getBatteryLevel = false,
        .getBatteryTemperature = false,
        .getChargerState = false,
        .getCompassBearing = true,
        .getCurrentIn = false,
        .getCurrentOut = false,
        .getGpsSatellitesInView = true,
        .getGpsUsedSatellites = true,
        .getGyroscopeAccelX = false,
        .getGyroscopeAccelY = false,
        .getGyroscopeAccelZ = false,
        .getGyroscopeTemperature = false,
        .getMagneticVariation = false,
        .getVoltageHigherCell = false,
        .getVoltageLowerCell = false,
    };

    while (1) {
        readings.timestampExists = true;
        readings.timestamp = timestamp;

        errCode = dbStoreReadings(readings);
        if (errCode != ESP_OK) {
            ESP_LOGE("database_test_task", "storing readings failed");
        }
        errCode = dbGetReadings(readConfig, &jsonOut);
        if (errCode != ESP_OK) {
            ESP_LOGE("database_test_task", "receiving readings failed");
        } else {
            if (jsonOut == NULL) {
                printf("JSON OUT IS NULL\n");
            }
            char* output = cJSON_Print(jsonOut);
            if (output == NULL) {
                printf("OUTPUT IS NULL\n");
            } else {
                printf("JSON OUTPUT %s\n", output);
            }
            cJSON_Delete(jsonOut);
            jsonOut = NULL;
            cJSON_free(output);
        }
        timestamp += 5;

        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}

void readCoulombCounterTestTask() {
    static uint64_t startTime = 0;

    if (startTime == 0) startTime = esp_timer_get_time();

    while (1) {
        vTaskDelay((10000 / portTICK_PERIOD_MS));

        double time = (esp_timer_get_time() - startTime) / 1000000.0;

        double coulombShouldBe = 0.0048 * time;

        double coulombMeasured = getDuty() * (VCC / 1000.0) / 24000.0;
        coulombMeasured *= 10000.0;

        double differenze = fabs(coulombMeasured - coulombShouldBe);

        printf("Measured: %lf Should be: %lf Difference: %lf\n", coulombMeasured, coulombShouldBe, differenze);
    }
}