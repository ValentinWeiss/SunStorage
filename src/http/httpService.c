#include "httpService.h"

httpd_handle_t server = NULL;
httpd_config_t config = HTTPD_DEFAULT_CONFIG();
bool httpEnabled = true;
bool wifiEnabled = true;
bool gpsEnabled = true;
bool dcf77Enabled = true;
int httpStopDuration = 0;
int gpsStopDuration = 0;
int wifiStopDuration = 0;
int dcf77StopDuration = 0;

bool getGpsEnabled() {
    return gpsEnabled;
}

bool getDcf77Enabled() {
    return dcf77Enabled;
}

// stop server task for x seconds
void stopHttp() {
    httpd_stop(server);
    httpEnabled = false;
    ESP_LOGI("HTTP::STOP", "Stopping server for %d seconds", httpStopDuration);

    vTaskDelay(pdMS_TO_TICKS(httpStopDuration * 1000));

    ESP_LOGI("HTTP::STOP", "Starting server after %d seconds", httpStopDuration);
    initHttpServer();
    httpEnabled = true;
    httpStopDuration = 0;

    vTaskDelete(NULL);
}

// stop wifi task for x seconds
void stopWifi() {
    esp_err_t res = wifiStop();
    if (res != ESP_OK) {
        ESP_LOGI("WIFI::STOP", "Could not stop wifi!");
        wifiEnabled = true;
        wifiStopDuration = 0;
        return;
    }
    wifiEnabled = false;
    ESP_LOGI("WIFI::STOP", "Stopping wifi for %d seconds", wifiStopDuration);

    vTaskDelay(pdMS_TO_TICKS(wifiStopDuration * 1000));

    ESP_LOGI("WIFI::STOP", "Starting wifi after %d seconds", wifiStopDuration);

    esp_wifi_start();

    wifiEnabled = true;
    wifiStopDuration = 0;

    vTaskDelete(NULL);
}
// stop wifi task for x seconds
void stopDCF77() {
    // stopDCF77();
    dcf77Enabled = false;
    ESP_LOGI("WIFI::STOP", "Stopping dcf77 for %d seconds", dcf77StopDuration);

    vTaskDelay(pdMS_TO_TICKS(dcf77StopDuration * 1000));

    ESP_LOGI("WIFI::STOP", "Starting dcf77 after %d seconds", dcf77StopDuration);

    dcf77Enabled = true;
    dcf77StopDuration = 0;

    vTaskDelete(NULL);
}
// stop wifi task for x seconds
void stopGPS() {
    // stopGPS();
    gpsEnabled = false;
    ESP_LOGI("WIFI::STOP", "Stopping gps for %d seconds", gpsStopDuration);

    vTaskDelay(pdMS_TO_TICKS(gpsStopDuration * 1000));

    ESP_LOGI("WIFI::STOP", "Starting gps after %d seconds", gpsStopDuration);

    gpsEnabled = true;
    gpsStopDuration = 0;

    vTaskDelete(NULL);
}

// set http content type based on file endings; is used for static file serve
esp_err_t setContenttypeBasedOnFile(httpd_req_t *req, const char *filepath) {
    const char *type = "text/plain";
    const char *gzipEncoding = "Content-Encoding";
    const char *gzipEncodingValue = "gzip";
    if (HTTP_CHECK_FILE_EXTENSION(filepath, ".html")) {
        type = "text/html";
    } else if (HTTP_CHECK_FILE_EXTENSION(filepath, ".js")) {
        type = "application/javascript";
        httpd_resp_set_hdr(req, gzipEncoding, gzipEncodingValue);
    } else if (HTTP_CHECK_FILE_EXTENSION(filepath, ".css")) {
        type = "text/css";
        // httpd_resp_set_hdr(req, gzipEncoding, gzipEncodingValue);
    } else if (HTTP_CHECK_FILE_EXTENSION(filepath, ".png")) {
        type = "image/png";
    } else if (HTTP_CHECK_FILE_EXTENSION(filepath, ".ico")) {
        type = "image/x-icon";
    } else if (HTTP_CHECK_FILE_EXTENSION(filepath, ".svg")) {
        type = "text/xml";
    } else if (HTTP_CHECK_FILE_EXTENSION(filepath, ".json")) {
        type = "application/json";
    }
    return httpd_resp_set_type(req, type);
}

esp_err_t checkIfIsServableFile(const char *filepath) {
    if (HTTP_CHECK_FILE_EXTENSION(filepath, ".html")) {
        return ESP_OK;
    } else if (HTTP_CHECK_FILE_EXTENSION(filepath, ".js")) {
        return ESP_OK;
    } else if (HTTP_CHECK_FILE_EXTENSION(filepath, ".css")) {
        return ESP_OK;
    } else if (HTTP_CHECK_FILE_EXTENSION(filepath, ".png")) {
        return ESP_OK;
    } else if (HTTP_CHECK_FILE_EXTENSION(filepath, ".ico")) {
        return ESP_OK;
    } else if (HTTP_CHECK_FILE_EXTENSION(filepath, ".svg")) {
        return ESP_OK;
    } else if (HTTP_CHECK_FILE_EXTENSION(filepath, ".json")) {
        return ESP_OK;
    }
    return ESP_FAIL;
}

// system information handler
esp_err_t systemStatus(httpd_req_t *req) {
    httpd_resp_set_type(req, "application/json");
    cJSON *jsonRoot = cJSON_CreateObject();
    esp_chip_info_t chipInfo;
    esp_chip_info(&chipInfo);
    uint8_t baseMacAdr[6] = {0};
    esp_read_mac(baseMacAdr, ESP_MAC_WIFI_STA);
    cJSON_AddStringToObject(jsonRoot, "version", IDF_VER);
    cJSON_AddNumberToObject(jsonRoot, "cores", chipInfo.cores);
    cJSON_AddNumberToObject(jsonRoot, "model", chipInfo.model);
    cJSON_AddNumberToObject(jsonRoot, "features", chipInfo.features);
    cJSON_AddNumberToObject(jsonRoot, "availableHeap", esp_get_free_heap_size());
    cJSON_AddNumberToObject(jsonRoot, "availableInternalHeap", esp_get_free_internal_heap_size());
    cJSON_AddNumberToObject(jsonRoot, "minimumHeap", esp_get_minimum_free_heap_size());
    cJSON_AddNumberToObject(jsonRoot, "mac", *baseMacAdr);

    const char *sysInfo = cJSON_Print(jsonRoot);
    httpd_resp_sendstr(req, sysInfo);
    free((void *)sysInfo);
    cJSON_Delete(jsonRoot);
    return ESP_OK;
}

// system information handler
esp_err_t wifiAPListHandler(httpd_req_t *req) {
    httpd_resp_set_type(req, "application/json");
    cJSON *jsonRoot = cJSON_CreateObject();
    wifiScan(jsonRoot);

    const char *apList = cJSON_Print(jsonRoot);
    httpd_resp_sendstr(req, apList);
    free((void *)apList);
    cJSON_Delete(jsonRoot);
    return ESP_OK;
}

esp_err_t parseHttpBody(cJSON **json_post, httpd_req_t *req) {
    char *body = malloc(req->content_len + 1);
    if (body == NULL) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Request body got no length");
        return ESP_FAIL;
    }

    // parse provided body
    if (httpd_req_recv(req, body, req->content_len) <= 0) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Bad Request");
        free(body);
        return ESP_FAIL;
    }
    body[req->content_len] = '\0';  // add end of string

    // body mit cjson parsen
    *json_post = cJSON_Parse(body);
    if (*json_post == NULL) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid JSON");
        free(body);
        return ESP_FAIL;
    }
    free(body);
    return ESP_OK;
}

esp_err_t wifiConfigureHandler(httpd_req_t *req) {
    char *body = malloc(req->content_len + 1);
    if (body == NULL) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Internal Server Error");
        return ESP_FAIL;
    }

    // parse provided body
    if (httpd_req_recv(req, body, req->content_len) <= 0) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Bad Request");
        free((void *)body);
        return ESP_FAIL;
    }
    body[req->content_len] = '\0';  // add end of string

    // body mit cjson parsen
    cJSON *json = cJSON_Parse(body);
    if (json == NULL) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid JSON");
        free((void *)body);
        return ESP_FAIL;
    }

    cJSON *ssidJson = cJSON_GetObjectItemCaseSensitive(json, "ssid");
    cJSON *passwordJson = cJSON_GetObjectItemCaseSensitive(json, "password");
    if (!cJSON_IsString(ssidJson) || !cJSON_IsString(passwordJson)) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid JSON Format");
        cJSON_Delete(json);
        free(body);
        return ESP_FAIL;
    }

    FILE *file = fopen(PATH_CONCAT(SDC_MOUNT_POINT, WIFI_CONFIG_FILE), "w");
    if (file == NULL) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to open wifi settings file");
        cJSON_Delete(json);
        free(body);
        return ESP_OK;
    }
    char *wifi_txt = cJSON_Print(json);
    fputs(wifi_txt, file);
    fclose(file);
    // send success to client
    httpd_resp_send(req, NULL, 0);

    free((void *)wifi_txt);

    ESP_LOGI("HTTP::WifiConfigure", "Updated Wifi config file. Try to connect to AP");

    initWifi();

    cJSON_Delete(json);
    free((void *)body);

    return ESP_OK;
}

esp_err_t stateSetterHandler(httpd_req_t *req) {
    cJSON *postedJson = NULL;
    esp_err_t res = parseHttpBody(&postedJson, req);

    if (res != ESP_OK) {
        return ESP_FAIL;
    }

    cJSON *usbMode = cJSON_GetObjectItem(postedJson, "usbMode");
    cJSON *currentTarget = cJSON_GetObjectItem(postedJson, "currentTarget");
    cJSON *thresholdVoltage = cJSON_GetObjectItem(postedJson, "thresholdVoltage");
    cJSON *maximumVoltage = cJSON_GetObjectItem(postedJson, "maximumVoltage");
    cJSON *trickleThreshold = cJSON_GetObjectItem(postedJson, "trickleThreshold");
    cJSON *batterySize = cJSON_GetObjectItem(postedJson, "batterySize");
    cJSON *overheatedTemperature = cJSON_GetObjectItem(postedJson, "overheatedTemperature");
    cJSON *highPowerEnabled = cJSON_GetObjectItem(postedJson, "highPowerEnabled");
    cJSON *overheatedHysteresis = cJSON_GetObjectItem(postedJson, "overheatedHysteresis");

    if (usbMode != NULL) {
        setUsbMode(usbMode->valuedouble);
    }
    if (currentTarget != NULL) {
        setSolarCurrentTarget(currentTarget->valuedouble);
    }
    if (thresholdVoltage != NULL) {
        setThresholdVoltage(thresholdVoltage->valuedouble);
    }
    if (maximumVoltage != NULL) {
        setMaximumVoltage(maximumVoltage->valuedouble);
    }
    if (trickleThreshold != NULL) {
        setTrickleThreshold(trickleThreshold->valuedouble);
    }
    if (batterySize != NULL) {
        setBatterySize(batterySize->valuedouble);
    }
    if (overheatedTemperature != NULL) {
        setBatteryOverheatedTemperature(overheatedTemperature->valuedouble);
    }
    if (highPowerEnabled != NULL) {
        setHighPowerCircuitEnabled(highPowerEnabled->valuedouble);
    }
    if (overheatedHysteresis != NULL) {
        setBatteryOverheatedHysteresis(overheatedHysteresis->valuedouble);
    }

    cJSON *latitude = cJSON_GetObjectItem(postedJson, "latitude");
    cJSON *logitude = cJSON_GetObjectItem(postedJson, "longitude");
    cJSON *azimuth = cJSON_GetObjectItem(postedJson, "azimuth");
    cJSON *elevation = cJSON_GetObjectItem(postedJson, "elevation");
    cJSON *angleToNorth = cJSON_GetObjectItem(postedJson, "angleToNorth");
    if (latitude != NULL) {
        setLatitude(latitude->valuedouble);
    }
    if (logitude != NULL) {
        setLongitude(logitude->valuedouble);
    }
    if (azimuth != NULL) {
        setPanelAzimuth(azimuth->valuedouble);
    }
    if (elevation != NULL) {
        setPanelElevation(elevation->valuedouble);
    }
    if (angleToNorth != NULL) {
        setAngleToNorth(angleToNorth->valuedouble);
    }

    ESP_LOGI("HTTP::STATEHANDLER", "Setting Battery values");

    cJSON *manMode = cJSON_GetObjectItem(postedJson, "manualMode");
    if (manMode != NULL) {
        setAutoAdjustPanel(cJSON_IsTrue(manMode));
    }
    // cJSON_Delete(manMode);

    display_config_t displayConfig = {.itemEnabled = {false, false, false, false, false,
                                                      false, false, false, false,
                                                      false, false, false, false,
                                                      false, false, false, false,
                                                      false, false, false, false,
                                                      false, false, false, false,
                                                      false, false, false, false,
                                                      false, false, false, false,
                                                      false}};
    getDisplayConfig(&displayConfig);
    for (uint8_t i = 0; i < NUM_DISPLAY_ITEMS; i++) {
        const char *toEval = DISPLAY_NAMES[i];
        char combinedString[strlen(toEval) + 3 + 1];
        sprintf(combinedString, "d_%s", toEval);
        if (cJSON_GetObjectItem(postedJson, combinedString) != NULL) {
            displayConfig.itemEnabled[i] = cJSON_IsTrue(cJSON_GetObjectItem(postedJson, combinedString));
        }
    }

    setDisplayConfig(&displayConfig);

    // night shutdown
    cJSON *nightShutdownEnabled = cJSON_GetObjectItem(postedJson, "nsEnabled");
    cJSON *nightShutdownAzimuth = cJSON_GetObjectItem(postedJson, "nsAzimuth");
    cJSON *nightShutdownElevation = cJSON_GetObjectItem(postedJson, "nsElevation");

    if (nightShutdownEnabled != NULL) {
        setNightShutdownEnabled(cJSON_IsTrue(nightShutdownEnabled));
    }
    if (nightShutdownAzimuth != NULL) {
        setNightShutdownAzimuth(nightShutdownAzimuth->valuedouble);
    }
    if (nightShutdownElevation != NULL) {
        setNightShutdownElevation(nightShutdownElevation->valuedouble);
    }

    // send ok response to client with updated json
    httpd_resp_send(req, NULL, 0);

    // free all allocated memory
    cJSON_Delete(postedJson);

    return ESP_OK;
}

void changeKey(cJSON *object, const char *newKey, const char *oldKey) {
    cJSON *item = cJSON_GetObjectItem(object, oldKey);
    if (item != NULL) {
        cJSON_AddItemToObject(object, newKey, cJSON_Duplicate(item, 1));
        cJSON_DeleteItemFromObject(object, oldKey);
    }
}

esp_err_t stateGetterHandler(httpd_req_t *req) {
    cJSON *root = cJSON_CreateObject();

    getState(&root);

    if (root == NULL) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to parse state");
        cJSON_Delete(root);
        return ESP_FAIL;
    }

    // set ns before each string in state for frontend
    cJSON *nightShutdown = cJSON_GetObjectItem(root, "nightShutdown");
    if (nightShutdown != NULL && nightShutdown->type == cJSON_Object) {
        changeKey(nightShutdown, "nsEnabled", "enabled");
        changeKey(nightShutdown, "nsAzimuth", "azimuth");
        changeKey(nightShutdown, "nsElevation", "elevation");
    }

    // set d_ before each string for frontend
    cJSON *display = cJSON_GetObjectItem(root, "display");
    if (display != NULL && display->type == cJSON_Object) {
        changeKey(display, "d_powerSolar", "powerSolar");
        changeKey(display, "d_dailyProduction", "dailyProduction");
        changeKey(display, "d_soc", "soc");
        changeKey(display, "d_voltageLowerCell", "voltageLowerCell");
        changeKey(display, "d_voltageUpperCell", "voltageUpperCell");
        changeKey(display, "d_solarCurrent", "solarCurrent");
        changeKey(display, "d_chargingCurrent", "chargingCurrent");
        changeKey(display, "d_systemCurrent", "systemCurrent");
        changeKey(display, "d_powerSystem", "powerSystem");
        changeKey(display, "d_powerCharging", "powerCharging");
        changeKey(display, "d_dailyConsumption", "dailyConsumption");
        changeKey(display, "d_thresholdVoltage", "thresholdVoltage");
        changeKey(display, "d_maximumVoltage", "maximumVoltage");
        changeKey(display, "d_trickleThreshold", "trickleThreshold");
        changeKey(display, "d_highPowerCircuit", "highPowerCircuit");
        changeKey(display, "d_batteryTemperature", "batteryTemperature");
        changeKey(display, "d_batteryOverheatedHysteresis", "batteryOverheatedHysteresis");
        changeKey(display, "d_batterySize", "batterySize");
        changeKey(display, "d_currentTarget", "currentTarget");
        changeKey(display, "d_chargerState", "chargerState");
        changeKey(display, "d_usbMode", "usbMode");
        changeKey(display, "d_azimuth", "azimuth");
        changeKey(display, "d_elevation", "elevation");
        changeKey(display, "d_manualAdjustment", "manualAdjustment");
        changeKey(display, "d_nightShutdownEnabled", "nightShutdownEnabled");
        changeKey(display, "d_nightPositionAzimuth", "nightPositionAzimuth");
        changeKey(display, "d_nightPositionElevation", "nightPositionElevation");
        changeKey(display, "d_latitude", "latitude");
        changeKey(display, "d_longitude", "longitude");
        changeKey(display, "d_angleToNorth", "angleToNorth");
        changeKey(display, "d_time", "time");
        changeKey(display, "d_httpDisableTimeout", "httpDisableTimeout");
        changeKey(display, "d_gpsDisableTimeout", "gpsDisableTimeout");
        changeKey(display, "d_dcfDisableTimeout", "dcfDisableTimeout");
        changeKey(display, "d_wifiDisableTimeout", "wifiDisableTimeout");
    }

    // send state to frontend
    char *responseText = cJSON_Print(root);
    httpd_resp_sendstr(req, responseText);
    free((void *)responseText);
    cJSON_Delete(root);
    return ESP_OK;
}

esp_err_t recalibrateBatteryHandler(httpd_req_t *req) {
    recalibrateSoC();
    httpd_resp_send(req, NULL, 0);
    return ESP_OK;
}

esp_err_t databaseGetHandler(httpd_req_t *req) {
    cJSON *postedJson = NULL;
    esp_err_t res = parseHttpBody(&postedJson, req);

    if (res != ESP_OK) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid JSON");
        return ESP_FAIL;
    }

    cJSON *jsonOut = NULL;
    readings_read_config_t readConfig = {
        .startTimestamp = 0,
        .endTimestamp = 0,
        .getTimestamp = false,
        .getGpsLat = false,
        .getGpsLon = false,
        .getBatteryLevel = false,
        .getBatteryTemperature = false,
        .getChargerState = false,
        .getCompassBearing = false,
        .getCurrentIn = false,
        .getCurrentOut = false,
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

    if (cJSON_GetObjectItem(postedJson, "getTimestamp") != NULL) {
        readConfig.getTimestamp = true;
    }
    if (cJSON_GetObjectItem(postedJson, "getGpsLat") != NULL) {
        readConfig.getGpsLat = true;
    }
    if (cJSON_GetObjectItem(postedJson, "getGpsLon") != NULL) {
        readConfig.getGpsLon = true;
    }
    if (cJSON_GetObjectItem(postedJson, "getBatteryLevel") != NULL) {
        readConfig.getBatteryLevel = true;
    }
    if (cJSON_GetObjectItem(postedJson, "getBatteryTemperature") != NULL) {
        readConfig.getBatteryTemperature = true;
    }
    if (cJSON_GetObjectItem(postedJson, "getChargerState") != NULL) {
        readConfig.getChargerState = true;
    }
    if (cJSON_GetObjectItem(postedJson, "getCompassBearing") != NULL) {
        readConfig.getCompassBearing = true;
    }
    if (cJSON_GetObjectItem(postedJson, "getCurrentIn") != NULL) {
        readConfig.getCurrentIn = true;
    }
    if (cJSON_GetObjectItem(postedJson, "getCurrentOut") != NULL) {
        readConfig.getCurrentOut = true;
    }
    if (cJSON_GetObjectItem(postedJson, "getGpsSatellitesInView") != NULL) {
        readConfig.getGpsSatellitesInView = true;
    }
    if (cJSON_GetObjectItem(postedJson, "getGpsUsedSatellites") != NULL) {
        readConfig.getGpsUsedSatellites = true;
    }
    if (cJSON_GetObjectItem(postedJson, "getGyroscopeAccelX") != NULL) {
        readConfig.getGyroscopeAccelX = true;
    }
    if (cJSON_GetObjectItem(postedJson, "getGyroscopeAccelY") != NULL) {
        readConfig.getGyroscopeAccelY = true;
    }
    if (cJSON_GetObjectItem(postedJson, "getGyroscopeAccelZ") != NULL) {
        readConfig.getGyroscopeAccelZ = true;
    }
    if (cJSON_GetObjectItem(postedJson, "getGyroscopeTemperature") != NULL) {
        readConfig.getGyroscopeTemperature = true;
    }
    if (cJSON_GetObjectItem(postedJson, "getMagneticVariation") != NULL) {
        readConfig.getMagneticVariation = true;
    }
    if (cJSON_GetObjectItem(postedJson, "getVoltageHigherCell") != NULL) {
        readConfig.getVoltageHigherCell = true;
    }
    if (cJSON_GetObjectItem(postedJson, "getVoltageLowerCell") != NULL) {
        readConfig.getVoltageLowerCell = true;
    }

    cJSON *jsonStartTimestamp = cJSON_GetObjectItem(postedJson, "startTimestamp");
    cJSON *jsonEndTimestamp = cJSON_GetObjectItem(postedJson, "endTimestamp");
    if (jsonStartTimestamp == NULL) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid JSON. No timestamp given");
        return ESP_FAIL;
    }

    readConfig.startTimestamp = (int64_t)atoll(jsonStartTimestamp->valuestring);
    if (jsonEndTimestamp != NULL) {
        readConfig.endTimestamp = (int64_t)atoll(jsonEndTimestamp->valuestring);
    } else {
        readConfig.endTimestamp = 0;
    }

    esp_err_t errCode = dbGetReadings(readConfig, &jsonOut);
    if (errCode != ESP_OK) {
        ESP_LOGE("database_test_task", "receiving readings failed");
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "receiving readings failed");
        return ESP_FAIL;
    }

    char *output = cJSON_Print(jsonOut);
    httpd_resp_sendstr(req, output);
    // cJSON_Delete(jsonEndTimestamp);
    // cJSON_Delete(jsonEndTimestamp);
    cJSON_Delete(jsonOut);
    jsonOut = NULL;
    cJSON_free(output);
    cJSON_Delete(postedJson);
    return ESP_OK;
}

// serve static files
esp_err_t commonGetHandler(httpd_req_t *req) {
    char filepath[HTTP_FILE_PATH_MAX];
    restServerContext_t *restContext = (restServerContext_t *)req->user_ctx;
    strlcpy(filepath, restContext->base_path, sizeof(filepath));

    if (req->uri[strlen(req->uri) - 1] == '/') {
        strlcat(filepath, "/index.html", sizeof(filepath));

    } else {
        strlcat(filepath, req->uri, sizeof(filepath));
    }

    esp_err_t isServable = checkIfIsServableFile(filepath);

    if (isServable == ESP_FAIL) {
        strlcpy(filepath, restContext->base_path, sizeof(filepath));
        strlcat(filepath, "/index.html", sizeof(filepath));
    }

    // handle error if path not exists
    int fd = open(filepath, O_RDONLY, 0);
    if (fd == -1) {
        ESP_LOGE("HTTP::commonGetHandler", "Failed to open file : %s", filepath);
        /* Respond with 500 Internal Server Error */
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to read existing file");
        return ESP_FAIL;
    }

    setContenttypeBasedOnFile(req, filepath);

    char *chunk = restContext->scratch;
    ssize_t read_bytes;
    do {
        /* Read file in chunks into the scratch buffer */
        read_bytes = read(fd, chunk, HTTP_SCRATCH_BUFSIZE);
        if (read_bytes == -1) {
            ESP_LOGE("HTTP::commonGetHandler", "Failed to read file : %s", filepath);
        } else if (read_bytes > 0) {
            /* Send the buffer contents as HTTP response chunk */
            if (httpd_resp_send_chunk(req, chunk, read_bytes) != ESP_OK) {
                close(fd);
                ESP_LOGE("HTTP::commonGetHandler", "File sending failed!");
                /* Abort sending file */
                httpd_resp_sendstr_chunk(req, NULL);
                /* Respond with 500 Internal Server Error */
                httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to send file");
                return ESP_FAIL;
            }
        }
    } while (read_bytes > 0);
    /* Close file after sending complete */
    close(fd);
    ESP_LOGI("HTTP::commonGetHandler", "File sending complete");
    /* Respond with an empty chunk to signal HTTP response completion */
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

esp_err_t shutdownHandler(httpd_req_t *req) {
    cJSON *postedJson = NULL;
    esp_err_t res = parseHttpBody(&postedJson, req);

    if (res != ESP_OK) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid JSON");
        return ESP_FAIL;
    }

    cJSON *http = cJSON_GetObjectItem(postedJson, "http");
    cJSON *gps = cJSON_GetObjectItem(postedJson, "gps");
    cJSON *wifi = cJSON_GetObjectItem(postedJson, "wifi");
    cJSON *dcf77 = cJSON_GetObjectItem(postedJson, "dcf77");
    if (http != NULL) {
        httpStopDuration = http->valueint;
        if (httpEnabled && httpStopDuration > 0)
            xTaskCreate(&stopHttp, "stop_http_task", 4096, NULL, 5, NULL);
    }

    if (gps != NULL) {
        gpsStopDuration = gps->valueint;
        if (gpsEnabled && gpsStopDuration > 0)
            xTaskCreate(&stopGPS, "stop_gps_task", 4096, NULL, 5, NULL);
    }
    if (wifi != NULL) {
        wifiStopDuration = wifi->valueint;
        if (wifiEnabled && wifiStopDuration > 0)
            xTaskCreate(&stopWifi, "stop_wifi_task", 4096, NULL, 5, NULL);
    }

    if (dcf77 != NULL) {
        dcf77StopDuration = dcf77->valueint;
        if (dcf77Enabled && dcf77StopDuration > 0)
            xTaskCreate(&stopDCF77, "stop_dcf77_task", 4096, NULL, 5, NULL);
    }

    cJSON_Delete(postedJson);
    httpd_resp_send_chunk(req, NULL, 0);

    return ESP_OK;
}

// init http server
esp_err_t initHttpServer(void) {
    restServerContext_t *restContext = calloc(1, sizeof(restServerContext_t));
    strlcpy(restContext->base_path, SPIFFS_BASE_PATH, sizeof(restContext->base_path));

    config.max_uri_handlers = HTTP_MAX_URI_HANDLERS;
    config.uri_match_fn = httpd_uri_match_wildcard;
    config.max_uri_handlers = 20;
    config.stack_size = 1024 * 10;

    httpd_uri_t uri_system_information_get = {
        .uri = "/api/v1/system",
        .method = HTTP_GET,
        .handler = systemStatus,
        .user_ctx = restContext};

    httpd_uri_t uri_get_wifi_list = {
        .uri = "/api/v1/wifi/list",
        .method = HTTP_GET,
        .handler = wifiAPListHandler,
        .user_ctx = restContext};

    httpd_uri_t set_wifi_settings = {
        .uri = "/api/v1/wifi/configure",
        .method = HTTP_POST,
        .handler = wifiConfigureHandler,
        .user_ctx = restContext};

    httpd_uri_t set_state_uri = {
        .uri = "/api/v1/state",
        .method = HTTP_POST,
        .handler = stateSetterHandler,
        .user_ctx = restContext};

    httpd_uri_t get_state_uri = {
        .uri = "/api/v1/state",
        .method = HTTP_GET,
        .handler = stateGetterHandler,
        .user_ctx = restContext};

    httpd_uri_t get_battery_recalibrate = {
        .uri = "/api/v1/battery/calibrate",
        .method = HTTP_GET,
        .handler = recalibrateBatteryHandler,
        .user_ctx = restContext};

    httpd_uri_t get_database_values = {
        .uri = "/api/v1/database",
        .method = HTTP_POST,
        .handler = databaseGetHandler,
        .user_ctx = restContext};

    httpd_uri_t shutdown_uri = {
        .uri = "/api/v1/shutdown",
        .method = HTTP_POST,
        .handler = shutdownHandler,
        .user_ctx = restContext};

    httpd_uri_t uri_get = {
        .uri = "/*",
        .method = HTTP_GET,
        .handler = commonGetHandler,
        .user_ctx = restContext};

    if (httpd_start(&server, &config) == ESP_OK) {
        ESP_LOGI("HTTP::ENDPOINT_INIT", "Initializing HTTP Endpoints");
        httpd_register_uri_handler(server, &shutdown_uri);
        // ===== SYSTEM INFORMATION HANDLER ===== //
        httpd_register_uri_handler(server, &uri_system_information_get);
        // ===== WIFI HANDLERS ===== //
        httpd_register_uri_handler(server, &uri_get_wifi_list);
        httpd_register_uri_handler(server, &set_wifi_settings);
        // ===== STATE HANDLER ===== //
        httpd_register_uri_handler(server, &set_state_uri);
        httpd_register_uri_handler(server, &get_state_uri);
        // ===== DATABASE HANDLER ===== //
        httpd_register_uri_handler(server, &get_database_values);
        httpd_register_uri_handler(server, &get_battery_recalibrate);
        // ===== STATIC FILE SERVER HANDLER ===== //
        httpd_register_uri_handler(server, &uri_get);
    } else {
        ESP_LOGE("HTTP:INIT", "HTTP Server failed to start!!!");
        return ESP_FAIL;
    }
    return ESP_OK;
}
