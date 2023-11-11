#include "wifiService.h"

EventGroupHandle_t s_wifi_event_group;
int retryAmount = 0;

static void wifiEventHandler(void* arg, esp_event_base_t event_base,
                             int32_t event_id, void* event_data) {
    wifi_mode_t mode;
    if (event_base == WIFI_EVENT) {
        switch (event_id) {
            case WIFI_EVENT_WIFI_READY:
                ESP_LOGI("WIFI::EVENT_HANDLER", "WIFI_EVENT_WIFI_READY");
                break;
            case WIFI_EVENT_SCAN_DONE:
                ESP_LOGI("WIFI::EVENT_HANDLER", "WIFI_EVENT_SCAN_DONE");
                break;
            case WIFI_EVENT_AP_START:
                ESP_LOGI("WIFI::EVENT_HANDLER", "WIFI_EVENT_AP_START");
                break;
            case WIFI_EVENT_AP_STOP:
                ESP_LOGI("WIFI::EVENT_HANDLER", "WIFI_EVENT_AP_STOP");
                break;
            case WIFI_EVENT_STA_START:
                esp_wifi_get_mode(&mode);
                if (mode == WIFI_MODE_STA) {
                    esp_wifi_connect();
                }
                break;
            case WIFI_EVENT_STA_DISCONNECTED:
                esp_wifi_get_mode(&mode);
                if (mode == WIFI_MODE_STA) {
                    if (retryAmount < WIFI_MAXIMUM_CONNECTION_RETRY) {
                        esp_wifi_connect();
                        retryAmount++;
                        ESP_LOGI("WIFI::EVENT_HANDLER", "retry to connect to the AP");
                    } else {
                        xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
                    }
                    ESP_LOGI("WIFI::EVENT_HANDLER", "connect to the AP fail");
                }
                break;
            default:
                break;
        }
    } else if (event_base == IP_EVENT) {
        switch (event_id) {
            case IP_EVENT_STA_GOT_IP:
                ip_event_got_ip_t* event = (ip_event_got_ip_t*)event_data;
                ESP_LOGI("WIFI::EVENT_HANDLER", "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
                retryAmount = 0;
                xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
                break;
            default:
                break;
        }
    } else {
        ESP_LOGI("WIFI::EVENT_HANDLER", "Unknown event");
    }
}

esp_err_t wifiStop() {
    esp_err_t res = esp_wifi_disconnect();
    if (res != ESP_OK) {
        return res;
    }
    res = esp_wifi_stop();
    if (res != ESP_OK) {
        return res;
    }

    return ESP_OK;
}

void wifiScan(cJSON* root) {
    uint16_t number = WIFI_DEFAULT_SCAN_LIST_SIZE;
    wifi_ap_record_t ap_info[WIFI_DEFAULT_SCAN_LIST_SIZE];
    uint16_t ap_count = 0;
    memset(ap_info, 0, sizeof(ap_info));

    esp_wifi_scan_start(NULL, true);

    cJSON* apArray = cJSON_CreateArray();

    esp_err_t res = esp_wifi_scan_get_ap_records(&number, ap_info);
    if (res != ESP_OK) {
        cJSON_AddItemToObject(root, "accessPoints", apArray);
        return;
    }

    res = esp_wifi_scan_get_ap_num(&ap_count);
    if (res != ESP_OK) {
        cJSON_AddItemToObject(root, "accessPoints", apArray);
        return;
    }

    ESP_LOGI("WIFI::SCAN", "Total APs scanned = %u", ap_count);

    for (int i = 0; (i < WIFI_DEFAULT_SCAN_LIST_SIZE) && (i < ap_count); i++) {
        ESP_LOGI("WIFI::SCAN", "SSID \t\t%s", ap_info[i].ssid);
        ESP_LOGI("WIFI::SCAN", "RSSI \t\t%d", ap_info[i].rssi);
        ESP_LOGI("WIFI::SCAN", "Channel \t\t%d\n", ap_info[i].primary);

        cJSON* foundAp = cJSON_CreateObject();
        cJSON_AddStringToObject(foundAp, "ssid", (char*)(ap_info[i].ssid));
        cJSON_AddNumberToObject(foundAp, "rssi", ap_info[i].rssi);
        cJSON_AddNumberToObject(foundAp, "authmode", ap_info[i].authmode);
        cJSON_AddNumberToObject(foundAp, "channel", ap_info[i].primary);
        cJSON_AddItemToArray(apArray, foundAp);
    }

    cJSON_AddItemToObject(root, "accessPoints", apArray);
}

esp_err_t initAccessPoint(void) {
    wifi_mode_t mode;
    esp_wifi_get_mode(&mode);
    // if current mode is sta mode stop wifi before switching to ap mode
    if (mode == WIFI_MODE_STA) {
        esp_wifi_disconnect();
        esp_wifi_stop();
        esp_wifi_set_mode(WIFI_MODE_NULL);
    }

    esp_netif_t* apConfig = esp_netif_create_default_wifi_ap();

    esp_err_t res = esp_netif_dhcps_stop(apConfig);
    if (res != ESP_OK) {
        ESP_LOGE("WIFI::INITAP", "Faild to stop dhcp");
        return res;
    }

    esp_netif_ip_info_t ipConfig;
    res = esp_netif_str_to_ip4(WIFI_FACTORY_GATEWAY_ADDRESS, &ipConfig.gw);
    if (res != ESP_OK) {
        ESP_LOGE("WIFI::INITAP", "Faild to read ipv4 GW");
        return res;
    }

    res = esp_netif_str_to_ip4(WIFI_FACTORY_IP_ADDRESS, &ipConfig.ip);
    if (res != ESP_OK) {
        ESP_LOGE("WIFI::INIT", "Faild to stop read ipv4");
        return res;
    }
    res = esp_netif_str_to_ip4(WIFI_FACTORY_SUBNETMASK, &ipConfig.netmask);
    if (res != ESP_OK) {
        ESP_LOGE("WIFI::INIT", "Faild to read SM");
        return res;
    }
    res = esp_netif_set_ip_info(apConfig, &ipConfig);
    if (res != ESP_OK) {
        ESP_LOGE("WIFI::INIT", "Faild to set ip info");
        return res;
    }

    res = esp_netif_dhcps_start(apConfig);
    if (res != ESP_OK) {
        ESP_LOGE("WIFI::INIT", "Faild to start dhcp");
        return res;
    }

    // use default wifi init config
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    // initialize WiFI Allocate resource for WiFi driver
    res = esp_wifi_init(&cfg);
    if (res != ESP_OK) {
        ESP_LOGE("WIFI::INIT", "Faild to init wifi");
        return res;
    }
    // create event handler to print debug messages
    res = esp_event_handler_instance_register(
        WIFI_EVENT, ESP_EVENT_ANY_ID, &wifiEventHandler, NULL, NULL);
    if (res != ESP_OK) {
        ESP_LOGE("WIFI::INIT", "Faild to set wifi event handler. Continue with wifi setup");
    }

    // set general access point config
    wifi_config_t accessPointConfig = {
        .ap =
            {
                .ssid = WIFI_FACTORY_SSID,
                .channel = WIFI_FACTORY_CHANNEL,
                .password = WIFI_FACTORY_PASSWORD,
                .max_connection = WIFI_FACTORY_MAX_STA_CONN,
                .authmode = WIFI_AUTH_WPA_WPA2_PSK,
            },
    };

    res = esp_wifi_set_mode(WIFI_MODE_APSTA);
    if (res != ESP_OK) {
        ESP_LOGE("WIFI::INIT", "Faild to set wifi ap mode");
        return res;
    }
    res = esp_wifi_set_config(WIFI_IF_AP, &accessPointConfig);
    if (res != ESP_OK) {
        ESP_LOGE("WIFI::INIT", "Faild to set wifi config");
        return res;
    }
    res = esp_wifi_start();
    if (res != ESP_OK) {
        ESP_LOGE("WIFI::INIT", "Faild to start wifi");
        return res;
    }

    ESP_LOGI("WIFI::INIT_AP",
             "Access Point initialised. SSID %s, PASSWORD: %s",
             WIFI_FACTORY_SSID, WIFI_FACTORY_PASSWORD);
    return ESP_OK;
}

esp_err_t connectToAccessPoint(char* ssid, char* password) {
    ESP_LOGI("WIFI::STA_INIT", "WiFi init STA with ssid: %s and password: %s.", ssid, password);
    esp_err_t res = ESP_OK;
    wifi_mode_t mode;
    esp_wifi_get_mode(&mode);
    // if current mode is access point stop wifi before switching to sta mode
    if (mode != WIFI_MODE_STA) {
        esp_wifi_stop();
        esp_wifi_set_mode(WIFI_MODE_NULL);
    }

    if (mode != WIFI_MODE_STA) {
        s_wifi_event_group = xEventGroupCreate();

        esp_netif_create_default_wifi_sta();

        wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
        res = esp_wifi_init(&cfg);
        if (res != ESP_OK) {
            ESP_LOGE("WIFI::CONNECTAP", "Faild to init wifi");
            return res;
        }

        esp_event_handler_instance_t instance_any_id;
        esp_event_handler_instance_t instance_got_ip;
        res = esp_event_handler_instance_register(WIFI_EVENT,
                                                  ESP_EVENT_ANY_ID,
                                                  &wifiEventHandler,
                                                  NULL,
                                                  &instance_any_id);
        if (res != ESP_OK) {
            ESP_LOGE("WIFI::CONNECTAP", "Faild to set wifi event handler");
            return res;
        }
        res = esp_event_handler_instance_register(IP_EVENT,
                                                  IP_EVENT_STA_GOT_IP,
                                                  &wifiEventHandler,
                                                  NULL,
                                                  &instance_got_ip);
        if (res != ESP_OK) {
            ESP_LOGE("WIFI::CONNECTAP", "Faild to set wifi event handler");
            return res;
        }
    }

    wifi_config_t staConfig = {
        .sta = {
            .ssid = {0},
            .password = {0},
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
            .pmf_cfg = {
                .capable = true,
                .required = false},
        },
    };

    for (uint8_t i = 0; i < 32; i++) {
        if (ssid[i] == '\0') {
            break;
        }
        staConfig.sta.ssid[i] = ssid[i];
    }

    for (uint8_t i = 0; i < 64; i++) {
        if (password[i] == '\0') {
            break;
        }
        staConfig.sta.password[i] = password[i];
    }

    res = esp_wifi_set_mode(WIFI_MODE_STA);
    if (res != ESP_OK) {
        ESP_LOGE("WIFI::CONNECTAP", "Faild to set sta mode");
        return res;
    }
    res = esp_wifi_set_config(WIFI_IF_STA, &staConfig);
    if (res != ESP_OK) {
        ESP_LOGE("WIFI::CONNECTAP", "Faild to set wifi config sta");
        return res;
    }
    res = esp_wifi_start();
    if (res != ESP_OK) {
        ESP_LOGE("WIFI::CONNECTAP", "Faild to start wifi sta");
        return res;
    }

    ESP_LOGI("WIFI::STA", "WiFi init STA finished.");

    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
                                           WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                           pdFALSE,
                                           pdFALSE,
                                           portMAX_DELAY);

    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI("WIFI::STA", "connected to ap SSID: %s password: %s",
                 staConfig.sta.ssid, staConfig.sta.password);
        return ESP_OK;
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI("WIFI::STA", "Failed to connect to SSID: %s, password: %s",
                 staConfig.sta.ssid, staConfig.sta.password);
        return ESP_FAIL;
    } else {
        ESP_LOGE("WIFI::STA", "UNEXPECTED EVENT");
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t initWifi(void) {
    wifi_mode_t mode;
    esp_wifi_get_mode(&mode);
    esp_err_t res = ESP_OK;
    if (mode != WIFI_MODE_STA && mode != WIFI_MODE_APSTA) {
        res = esp_netif_init();
        if (res != ESP_OK) {
            ESP_LOGE("WIFI::INIT", "Faild to netif init");
            return res;
        }
        res = esp_event_loop_create_default();
        if (res != ESP_OK) {
            ESP_LOGE("WIFI::INIT", "Faild to event loop create");
            return res;
        }
    }

    ESP_LOGI("WIFI::INIT", "Trying to read wifi config file");

    FILE* f = fopen(PATH_CONCAT(SDC_MOUNT_POINT, WIFI_CONFIG_FILE), "r");
    if (f == NULL) {
        ESP_LOGI("WIFI::INIT", "No wifi config file found or error reading it. Init access point");
        return initAccessPoint();
    } else {
        ESP_LOGI("WIFI::INIT",
                 "Wifi config found connecting to access point...");
        char buffer[256] = {0};
        fread(buffer, 1, sizeof(buffer), f);
        fclose(f);

        cJSON* wlanConfig = cJSON_Parse(buffer);
        if (wlanConfig == NULL) {
            const char* error = cJSON_GetErrorPtr();
            if (error != NULL) {
                ESP_LOGE("WIFI::INIT",
                         "Error in WLAN JSON %s", error);
                cJSON_Delete(wlanConfig);
                return initAccessPoint();
            }
        }

        cJSON* ssid = cJSON_GetObjectItemCaseSensitive(wlanConfig, "ssid");
        cJSON* password = cJSON_GetObjectItemCaseSensitive(wlanConfig, "password");
        esp_err_t result;
        if (cJSON_IsString(ssid) && (ssid->valuestring != NULL) && cJSON_IsString(password) && (password->valuestring != NULL)) {
            // cJSON_Delete(wlanConfig);
            ESP_LOGI("WIFI::INIT", "Connecting to %s with pw: %s", ssid->valuestring, password->valuestring);
            result = connectToAccessPoint(ssid->valuestring, password->valuestring);
            if (result == ESP_FAIL) {
                ESP_LOGI("WIFI::INIT", "Switching to AP mode as connection could not be established!");
                result = initAccessPoint();
            }
        } else {
            ESP_LOGE("WIFI::INIT", "Error parsing WLAN CONFIG JSON for ssid and password");
            result = initAccessPoint();
        }
        cJSON_Delete(wlanConfig);
        return result;
    }
}
