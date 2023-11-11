#include "globalInit.h"

#include <nvs_flash.h>

#include "initTracker.h"
#include "state.h"

#ifdef TX
#include "adcReader.h"
#include "charger.h"
#include "dbInterface.h"
#include "dcf77.h"
#include "driver/barometer.h"
#include "driver/compass.h"
#include "driver/gps.h"
#include "driver/gyroscope.h"
#include "espComm.h"
#include "httpService.h"
#include "i2cHandler.h"
#include "initTracker.h"
#include "sdCardService.h"
#include "servo.h"
#include "spiffsService.h"
#include "state.h"
#include "wifiService.h"
#endif

#ifdef RX
#include "espComm.h"
#include "lcdButtons.h"
#include "servo.h"
#endif

esp_err_t nvsInit() {
    esp_err_t errCode = nvs_flash_init();
    if (errCode == ESP_ERR_NVS_NO_FREE_PAGES ||
        errCode == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        errCode = nvs_flash_init();
    }
    if (errCode != ESP_OK) {
        ESP_LOGE("nvs_init", "failed to initialize nvs flash");
    }
    return errCode;
}

esp_err_t globalInit() {
#ifdef TX
    if (initHighPowerCircuit() == ESP_OK) {
        highPowerIsInitialized = true;
    } else {
        ESP_LOGE("global_init", "failed to init high power disable pin");
        return ESP_FAIL;
    }

    if (initAdc() == ESP_OK) {
        adcIsInitialized = true;
    } else {
        ESP_LOGE("global_init", "failed to init adc");
        return ESP_FAIL;
    }
#endif

    if (nvsInit() == ESP_OK) {
        nvsIsInitialized = true;
    } else {
        ESP_LOGE("global_init", "failed to initialize nvs flash");
        // nvs is not critical
    }

#ifdef TX
    if (initSpiffs() == ESP_OK) {
        spiffsIsInitialized = true;
    } else {
        ESP_LOGE("global_init", "failed to initialize spiffs");
        // spiffs is not critical
    }

    // initialize i2c devices
    if (i2cInit() == ESP_OK) {
        i2cIsInitialized = true;
        // init all devices
        if (barometerInit() == ESP_OK) {
            barometerIsInitialized = true;
        } else {
            ESP_LOGE("global_init", "failed to initialize barometer");
            // barometer is not critical
        }

        if (compassModuleInit() == ESP_OK) {
            compassModuleIsInitialized = true;
        } else {
            ESP_LOGE("global_init", "failed to initialize compass module");
            // compassModule is not critical
        }

        if (gyroscopeInit() == ESP_OK) {
            gyroscopeIsInitialized = true;
        } else {
            ESP_LOGE("global_init", "failed to initialize gyroscope");
            // gyroscope is not critical
        }
    } else {
        ESP_LOGE("global_init", "failed to initialize i2c");
        // i2c is not critical
    }

    if (gpsInit() == ESP_OK) {
        gpsIsInitialized = true;
    } else {
        ESP_LOGE("global_init", "failed to initialize gps");
        // gps is not critical
    }

    if (initCharger() == ESP_OK) {
        chargerIsInitialized = true;
    } else {
        ESP_LOGE("global_init", "failed to initialize charger");
        // charger is critical
        return ESP_FAIL;
    }

    if (sdCardInit() == ESP_OK) {
        sdCardIsInitialized = true;
    } else {
        ESP_LOGE("global_init", "failed to initialize sd card");
        // sd card is not critical
    }

    if (initWifi() == ESP_OK) {
        wifiIsInitialized = true;
    } else {
        ESP_LOGE("global_init", "failed to initialize wifi");
        // wifi is not critical
    }
#endif

    if (initSystemState() == ESP_OK) {
        systemStateIsInitialized = true;
    } else {
        ESP_LOGE("global_init", "failed to initialize system state");
        // system state is not critical
    }

#ifdef TX
    if (dbInit() == ESP_OK) {
        dbIsInitialized = true;
    } else {
        ESP_LOGE("global_init", "failed to initialize database");
        // database is not critical
    }
#endif

#ifdef RX
    if (initServo() == ESP_OK) {
        servoIsInitialized = true;
    } else {
        ESP_LOGE("global_init", "failed to initialize servo");
        // servo is not critical
    }
    if (initLcdDisplay() == ESP_OK) {
        lcdDisplayInitialized = true;
    } else {
        ESP_LOGE("global_init", "failed to initialize lcd display");
        // lcd display is not critical
    }
    if (initLcdButtons() == ESP_OK) {
        lcdButtonsInitialized = true;
    } else {
        ESP_LOGE("global_init", "failed to initialize lcd display buttons");
        // lcd buttons are not critical
    }

    if (initCommUART(1) == ESP_OK) {
        uartRxIsInitialized = true;
    }
#endif

#ifdef TX
    if (initHttpServer() == ESP_OK) {
        httpServerIsInitialized = true;
    } else {
        ESP_LOGE("global_init", "failed to initialize http server");
        // http server is not critical
    }

    if (initCommUART(0) == ESP_OK) {
        uartTxIsInitialized = true;
    }

    if (dcf77Init() == ESP_OK) {
        dcf77IsInitialized = true;
    } else {
        ESP_LOGE("global_init", "failed to initialize dcf77 module");
    }
#endif

    return ESP_OK;
}
