#include "driver/gps.h"

#include <freertos/semphr.h>
#include <rom/ets_sys.h>
#include <string.h>

SemaphoreHandle_t gpsMutex = NULL;

char c[GPS_UART_RX_BUFFER_SIZE] = {0};
size_t bufferedDataLen = 0;
uint16_t idx = 0;

typedef struct NmeaWord {
    size_t len;
    char* pWord;
} nmea_word_t;

esp_err_t gpsInit() {
    esp_err_t errCode = ESP_OK;
    gpsMutex = xSemaphoreCreateMutex();
    if (!gpsMutex) {
        return ESP_FAIL;
    }
    // initialize uart connection
    uart_config_t uartConf = {
        .baud_rate = GPS_UART_BAUD_RATE,
        .data_bits = GPS_UART_DATA_BITS,
        .parity = GPS_UART_PARITY,
        .stop_bits = GPS_UART_STOP_BITS,
        .flow_ctrl = GPS_UART_FLOW_CTRL,
        .rx_flow_ctrl_thresh = GPS_UART_RX_FLOW_CTRL_THRESH,
        .source_clk = UART_SCLK_DEFAULT,
    };

    errCode = uart_param_config(GPS_UART_NUM, &uartConf);
    if (errCode != ESP_OK) {
        return errCode;
    }

    // set communication pins without RTS and CTS
    errCode = uart_set_pin(GPS_UART_NUM, GPS_GPIO_TX, GPS_GPIO_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    if (errCode != ESP_OK) {
        return errCode;
    }

    // install uart driver
    errCode = uart_driver_install(GPS_UART_NUM, GPS_UART_RX_BUFFER_SIZE, GPS_UART_TX_BUFFER_SIZE, 0, NULL, 0);
    if (errCode != ESP_OK) {
        return errCode;
    }

    vTaskDelay(1000 / portTICK_PERIOD_MS);

    size_t testLen = 0;
    errCode = uart_get_buffered_data_len(GPS_UART_NUM, &testLen);
    if (errCode != ESP_OK) {
        return errCode;
    }
    if (!testLen) {
        errCode = ESP_FAIL;
    }

    return errCode;
}

esp_err_t gpsClearLocalUartBuffer() {
    if (xSemaphorePoll(gpsMutex) != pdTRUE) {
        return ESP_FAIL;
    }
    bufferedDataLen = 0;
    idx = 0;

    xSemaphoreGive(gpsMutex);

    return ESP_OK;
}

esp_err_t gpsTransmitData(const char* data, size_t len) {
    if (xSemaphorePoll(gpsMutex) != pdTRUE) {
        return ESP_FAIL;
    }
    int byteCount = uart_write_bytes(GPS_UART_NUM, data, len);
    if (byteCount == len) {
        return ESP_OK;
    }
    if (byteCount >= 0) {
        return ESP_ERR_NOT_FINISHED;
    }
    xSemaphoreGive(gpsMutex);
    return ESP_FAIL;
}

esp_err_t gpsReceiveNmea(char** nmea, size_t* len) {
    if (*nmea || !len) {
        return ESP_FAIL;
    }

    if (xSemaphorePoll(gpsMutex) != pdTRUE) {
        return ESP_FAIL;
    }

    esp_err_t errCode = ESP_OK;
    bool newDataSet = false;

    char nmeaBuffer[GPS_NMEA_MAX_SENTENCE_LENGTH] = {0};
    size_t lenCounter = 0;

    bool sentenceStarted = false;

    while (1) {
        if (bufferedDataLen < GPS_NMEA_MIN_LEN) {
            newDataSet = true;
            errCode = uart_get_buffered_data_len(GPS_UART_NUM, &bufferedDataLen);
            if (errCode != ESP_OK) {
                xSemaphoreGive(gpsMutex);
                return errCode;
            }
        }
        if (bufferedDataLen < GPS_NMEA_MIN_LEN) {
            vTaskDelay(10 / portTICK_PERIOD_MS);
            continue;
        }

        if (newDataSet) {
            idx = 0;
            // copy bytes in local buffer
            bufferedDataLen = uart_read_bytes(GPS_UART_NUM, c, bufferedDataLen, 100);
        }

        while (bufferedDataLen > 0) {
            if (c[idx] == '$') {
                sentenceStarted = true;
                lenCounter = 0;
            }
            if (sentenceStarted) {
                nmeaBuffer[lenCounter] = c[idx];
                lenCounter++;

                if (c[idx] == '\n') {
                    *nmea = malloc(lenCounter);
                    memcpy(*nmea, nmeaBuffer, lenCounter);
                    *len = lenCounter;

                    xSemaphoreGive(gpsMutex);
                    return errCode;
                }
                if (lenCounter > GPS_NMEA_MAX_SENTENCE_LENGTH) {
                    xSemaphoreGive(gpsMutex);
                    return ESP_ERR_INVALID_SIZE;
                }
            }
            bufferedDataLen--;
            idx++;
        }
    }
}

esp_err_t parseTime(nmea_word_t timeWord, gps_time_t* time) {
    if (!timeWord.pWord || !time) {
        return ESP_FAIL;
    }

    if (timeWord.len == 0) {
        time->hours = 0;
        time->minutes = 0;
        time->seconds = 0;
        time->milliseconds = 0;

        return ESP_OK;
    } else if (timeWord.len < 6) {
        return ESP_FAIL;
    }

    // HHMMSS.ss
    time->hours = (timeWord.pWord[0] - '0') * 10 + (timeWord.pWord[1] - '0');
    time->minutes = (timeWord.pWord[2] - '0') * 10 + (timeWord.pWord[3] - '0');
    time->seconds = (timeWord.pWord[4] - '0') * 10 + (timeWord.pWord[5] - '0');
    if (timeWord.len >= 9) {
        time->milliseconds = (timeWord.pWord[7] - '0') * 100 + (timeWord.pWord[8] - '0') * 10;
    } else {
        time->milliseconds = 0;
    }

    return ESP_OK;
}

esp_err_t parseCoordinate(nmea_word_t coordinateWord, float* gpsCoord) {
    if (!coordinateWord.pWord || !gpsCoord) {
        return ESP_FAIL;
    }

    if (coordinateWord.len == 0) {
        *gpsCoord = 0.0f;
        return ESP_OK;
    }

    char wordBuffer[20] = {0};  // Ensure string is terminated after memcpy
    if (coordinateWord.len > 19) {
        return ESP_FAIL;
    }
    memcpy(wordBuffer, coordinateWord.pWord, coordinateWord.len);
    char* fullstop = strchr(wordBuffer, '.');
    if (fullstop == NULL || fullstop - wordBuffer > 6) {
        return ESP_FAIL;
    }
    char degreeBuffer[6] = {0};
    strncpy(degreeBuffer, wordBuffer, fullstop - wordBuffer);  // get DDDMM
    long dddmm = atol(degreeBuffer);
    long degrees = (dddmm / 100);              // truncate the minutes
    long fullMinutes = dddmm - degrees * 100;  // remove the degrees

    float minutes = atof(fullstop);
    minutes += (float)fullMinutes;

    *gpsCoord = (float)degrees + (minutes / 60);

    return ESP_OK;
}

esp_err_t parseDirectionIndicator(nmea_word_t directionIndicatorWord, char* indicationChar) {
    if (!directionIndicatorWord.pWord || (directionIndicatorWord.len > 1) || !indicationChar) {
        return ESP_FAIL;
    }

    if (directionIndicatorWord.len == 0) {
        *indicationChar = 0;
    } else {
        *indicationChar = directionIndicatorWord.pWord[0];
    }

    return ESP_OK;
}

esp_err_t parseFixIndicator(nmea_word_t fixIndicatorWord, gps_position_fix_indicator_t* fixIndication) {
    if (!fixIndicatorWord.pWord || (fixIndicatorWord.len > 1) || !fixIndication) {
        return ESP_FAIL;
    }

    if (fixIndicatorWord.len == 0) {
        *fixIndication = FIX_NOT_AVAILABLE_OR_INVALID;
    } else {
        switch (fixIndicatorWord.pWord[0] - '0') {
            case 0:
                *fixIndication = FIX_NOT_AVAILABLE_OR_INVALID;
                break;
            case 1:
                *fixIndication = GNSS_FIX;
                break;
            case 2:
                *fixIndication = DGPS_FIX;
                break;
            case 3:
                *fixIndication = PPS_FIX;
                break;
            case 4:
                *fixIndication = REAL_TIME_KINEMATIC_FIX;
                break;
            case 5:
                *fixIndication = DEAD_RECKONING_FIX;
                break;
            case 6:
                *fixIndication = MANUAL_INPUT_MODE_FIX;
                break;
            case 7:
                *fixIndication = SIMULATION_MODE_FIX;
                break;
            default:
                *fixIndication = FIX_NOT_AVAILABLE_OR_INVALID;
                break;
        }
    }

    return ESP_OK;
}

esp_err_t parseUnit(nmea_word_t unitWord, gps_unit_t* unit) {
    if (!unitWord.pWord || !unit) {
        return ESP_FAIL;
    }

    if (unitWord.len == 0) {
        *unit = METER_OR_DEGREES_MAGNETIC;
    } else {
        switch (unitWord.pWord[0]) {
            case 'M':
                *unit = METER_OR_DEGREES_MAGNETIC;
                break;
            case 'T':
                *unit = DEGREES_TRUE;
                break;
            case 'N':
                *unit = KNOTS;
                break;
            case 'K':
                *unit = KILOMETERS_PER_HOUR;
                break;
            default:
                return ESP_FAIL;
        }
    }

    return ESP_OK;
}

esp_err_t parseUint8(nmea_word_t word, uint8_t* uint8) {
    if (!word.pWord || !uint8) {
        return ESP_FAIL;
    }

    if (word.len == 0) {
        *uint8 = 0;

        return ESP_OK;
    } else if (word.len > 10) {
        return ESP_FAIL;
    }

    char buffer[11] = {0};

    memcpy(buffer, word.pWord, word.len);

    *uint8 = (uint8_t)atol(buffer);

    return ESP_OK;
}

esp_err_t parseUint16(nmea_word_t word, uint16_t* uint16) {
    if (!word.pWord || !uint16) {
        return ESP_FAIL;
    }

    if (word.len == 0) {
        *uint16 = 0;

        return ESP_OK;
    } else if (word.len > 10) {
        return ESP_FAIL;
    }

    char buffer[11] = {0};

    memcpy(buffer, word.pWord, word.len);

    *uint16 = (uint16_t)atol(buffer);

    return ESP_OK;
}

esp_err_t parseFloat(nmea_word_t word, float* f) {
    if (!word.pWord || !f) {
        return ESP_FAIL;
    }

    if (word.len == 0) {
        *f = 0;

        return ESP_OK;
    } else if (word.len > 10) {
        return ESP_FAIL;
    }

    char buffer[11] = {0};

    memcpy(buffer, word.pWord, word.len);

    *f = (float)atof(buffer);

    return ESP_OK;
}

esp_err_t parseStatus(nmea_word_t statusWord, gps_status_t* status) {
    if (!statusWord.pWord || !status) {
        return ESP_FAIL;
    }

    if (statusWord.len == 0) {
        *status = VOID_STATUS;
    } else {
        switch (statusWord.pWord[0]) {
            case 'A':
                *status = ACTIVE_STATUS;
                break;
            case 'V':
                *status = VOID_STATUS;
                break;
            default:
                *status = VOID_STATUS;
                break;
        }
    }

    return ESP_OK;
}

esp_err_t parseMode(nmea_word_t modeWord, gps_mode_t* mode) {
    if (!modeWord.pWord || !mode) {
        return ESP_FAIL;
    }

    if (modeWord.len == 0) {
        *mode = NOT_VALID_MODE;
    } else {
        switch (modeWord.pWord[0]) {
            case 'A':
                *mode = AUTONOMOUS_MODE;
                break;
            case 'D':
                *mode = DIFFERENTIAL_MODE;
                break;
            case 'E':
                *mode = ESTIMATED_MODE;
                break;
            case 'F':
                *mode = FLOAT_RTK_MODE;
                break;
            case 'M':
                *mode = MANUAL_INPUT_MODE;
                break;
            case 'N':
                *mode = NOT_VALID_MODE;
                break;
            case 'P':
                *mode = PRECISE_MODE;
                break;
            case 'R':
                *mode = REAL_TIME_KINEMATIC_MODE;
                break;
            case 'S':
                *mode = SIMULATOR_MODE;
                break;
            default:
                *mode = NOT_VALID_MODE;
                break;
        }
    }

    return ESP_OK;
}

esp_err_t parseDate(nmea_word_t dateWord, gps_date_t* date) {
    if (!dateWord.pWord || !date) {
        return ESP_FAIL;
    }

    if (dateWord.len == 0) {
        date->day = 0;
        date->month = 0;
        date->year = 0;

        return ESP_OK;
    } else if (dateWord.len > 10) {
        return ESP_FAIL;
    }

    char buffer[11] = {0};

    memcpy(buffer, dateWord.pWord, dateWord.len);

    long ddmmyy = atol(buffer);
    date->day = ddmmyy / 10000;
    date->month = (ddmmyy % 10000) / 100;
    date->year = (ddmmyy % 100);

    return ESP_OK;
}

esp_err_t parseGsaMode1(nmea_word_t modeWord, gps_gsa_mode_1_t* mode) {
    if (!modeWord.pWord || !mode) {
        return ESP_FAIL;
    }

    if (modeWord.len == 0) {
        *mode = AUTOMATIC_GSA_MODE;
    } else {
        switch (modeWord.pWord[0]) {
            case 'M':
                *mode = MANUAL_GSA_MODE;
                break;
            case 'A':
                *mode = AUTOMATIC_GSA_MODE;
                break;
            default:
                *mode = AUTOMATIC_GSA_MODE;
                break;
        }
    }

    return ESP_OK;
}

esp_err_t parseGsaMode2(nmea_word_t modeWord, gps_gsa_mode_2_t* mode) {
    if (!modeWord.pWord || !mode) {
        return ESP_FAIL;
    }

    if (modeWord.len == 0) {
        *mode = FIX_NOT_AVAILABLE_GSA_MODE;
    } else {
        switch (modeWord.pWord[0]) {
            case '1':
                *mode = FIX_NOT_AVAILABLE_GSA_MODE;
                break;
            case '2':
                *mode = TWO_DIMENSION_GSA_MODE;
                break;
            case '3':
                *mode = THREE_DIMENSION_GSA_MODE;
                break;
            default:
                *mode = FIX_NOT_AVAILABLE_GSA_MODE;
                break;
        }
    }

    return ESP_OK;
}

esp_err_t gpsParseNmea(char* nmea, size_t len, nmea_data_t* nmeaData, nmea_data_type_t* nmeaDataType) {
    if ((len <= 1) || (nmea[0] != '$') || !nmeaData || !nmeaDataType) {
        return ESP_FAIL;
    }

    if (xSemaphorePoll(gpsMutex) != pdTRUE) {
        return ESP_FAIL;
    }

    esp_err_t errCode = ESP_OK;

    size_t nmeaStart = 1, nmeaEnd = 0;

    // check checksum
    uint8_t checksum = 0;
    uint8_t nmeaChecksum = 0;
    bool checksumStarted = false;
    bool checkSucceded = false;

    for (size_t i = 1; i < len; i++) {
        if (nmea[i] == '*') {
            checksumStarted = true;
            nmeaEnd = i;
        } else if (checksumStarted) {
            if ((i + 1 >= len)) {
                break;
            }
            // convert nmea checksum hex string to uint8_t
            char hexBuffer[3] = {nmea[i], nmea[i + 1], '\0'};
            nmeaChecksum = (uint8_t)strtoul(hexBuffer, NULL, 16);

            checkSucceded = (nmeaChecksum == checksum) ? true : false;
            break;
        } else {
            checksum ^= nmea[i];
        }
    }

    if (!checkSucceded) {
        xSemaphoreGive(gpsMutex);
        return ESP_ERR_INVALID_CRC;
    }

    // parse

    // separate sentence into words
    nmea_word_t wordPtr[GPS_NMEA_MAX_WORD_COUNT] = {0};
    size_t wordCounter = 0;
    size_t wordLenCounter = 0;

    // we can expact nmea sentence to have at least 1 word (sentence len > 1)
    wordPtr[wordCounter].pWord = &nmea[nmeaStart];

    while ((nmeaStart < nmeaEnd) && (wordCounter + 1 < GPS_NMEA_MAX_WORD_COUNT)) {
        if (nmea[nmeaStart] == ',') {
            wordPtr[wordCounter].len = wordLenCounter;
            wordLenCounter = 0;

            wordCounter++;
            if (nmeaStart + 1 <= nmeaEnd) {
                wordPtr[wordCounter].pWord = &nmea[nmeaStart + 1];
            }
        } else {
            wordLenCounter++;
        }
        nmeaStart++;
    }

    // set length of last word
    wordPtr[wordCounter].len = wordLenCounter;

    if ((wordCounter == 0) || (wordCounter >= GPS_NMEA_MAX_WORD_COUNT)) {
        xSemaphoreGive(gpsMutex);
        return ESP_ERR_INVALID_SIZE;
    }

    if (wordPtr[0].len != 5) {
        xSemaphoreGive(gpsMutex);
        return ESP_FAIL;
    }

    if (!strncmp(wordPtr[0].pWord, "GPRMC", 5)) {
        rmc_t rmc = {0};
        // parse Time
        errCode = parseTime(wordPtr[1], &rmc.time);
        if (errCode != ESP_OK) {
            xSemaphoreGive(gpsMutex);
            return errCode;
        }

        // parse status
        errCode = parseStatus(wordPtr[2], &rmc.status);
        if (errCode != ESP_OK) {
            xSemaphoreGive(gpsMutex);
            return errCode;
        }

        // parse latitude
        errCode = parseCoordinate(wordPtr[3], &rmc.latitude);
        if (errCode != ESP_OK) {
            xSemaphoreGive(gpsMutex);
            return errCode;
        }

        // parse N/S indicator
        errCode = parseDirectionIndicator(wordPtr[4], &rmc.northSouthIndicator);
        if (errCode != ESP_OK) {
            xSemaphoreGive(gpsMutex);
            return errCode;
        }

        // parse longitude
        errCode = parseCoordinate(wordPtr[5], &rmc.longitude);
        if (errCode != ESP_OK) {
            xSemaphoreGive(gpsMutex);
            return errCode;
        }

        // parse E/W indicator
        errCode = parseDirectionIndicator(wordPtr[6], &rmc.eastWestIndicator);
        if (errCode != ESP_OK) {
            xSemaphoreGive(gpsMutex);
            return errCode;
        }

        // parse speed over ground
        errCode = parseFloat(wordPtr[7], &rmc.speedOverGround);
        if (errCode != ESP_OK) {
            xSemaphoreGive(gpsMutex);
            return errCode;
        }

        // parse track made good
        errCode = parseFloat(wordPtr[8], &rmc.trackMadeGood);
        if (errCode != ESP_OK) {
            xSemaphoreGive(gpsMutex);
            return errCode;
        }

        // parse date
        errCode = parseDate(wordPtr[9], &rmc.date);
        if (errCode != ESP_OK) {
            xSemaphoreGive(gpsMutex);
            return errCode;
        }

        // parse magnetic veriation
        errCode = parseFloat(wordPtr[10], &rmc.magneticVariation);
        if (errCode != ESP_OK) {
            xSemaphoreGive(gpsMutex);
            return errCode;
        }

        // parse mode
        errCode = parseDirectionIndicator(wordPtr[11], &rmc.eastwestMagneticVariation);
        if (errCode != ESP_OK) {
            xSemaphoreGive(gpsMutex);
            return errCode;
        }

        *nmeaDataType = RMC;
        nmeaData->rmc = rmc;
    } else if (!strncmp(wordPtr[0].pWord, "GPGSA", 5)) {
        gsa_t gsa = {0};
        // parse mode
        errCode = parseGsaMode1(wordPtr[1], &gsa.mode1);
        if (errCode != ESP_OK) {
            xSemaphoreGive(gpsMutex);
            return errCode;
        }

        // parse mode
        errCode = parseGsaMode2(wordPtr[2], &gsa.mode2);
        if (errCode != ESP_OK) {
            xSemaphoreGive(gpsMutex);
            return errCode;
        }

        // parse satellite used on channel
        errCode = parseUint8(wordPtr[3], &gsa.satelliteUsedOnChannel1);
        if (errCode != ESP_OK) {
            xSemaphoreGive(gpsMutex);
            return errCode;
        }

        // parse satellite used on channel
        errCode = parseUint8(wordPtr[4], &gsa.satelliteUsedOnChannel2);
        if (errCode != ESP_OK) {
            xSemaphoreGive(gpsMutex);
            return errCode;
        }

        // parse satellite used on channel
        errCode = parseUint8(wordPtr[5], &gsa.satelliteUsedOnChannel3);
        if (errCode != ESP_OK) {
            xSemaphoreGive(gpsMutex);
            return errCode;
        }

        // parse satellite used on channel
        errCode = parseUint8(wordPtr[6], &gsa.satelliteUsedOnChannel4);
        if (errCode != ESP_OK) {
            xSemaphoreGive(gpsMutex);
            return errCode;
        }

        // parse satellite used on channel
        errCode = parseUint8(wordPtr[7], &gsa.satelliteUsedOnChannel5);
        if (errCode != ESP_OK) {
            xSemaphoreGive(gpsMutex);
            return errCode;
        }

        // parse satellite used on channel
        errCode = parseUint8(wordPtr[8], &gsa.satelliteUsedOnChannel6);
        if (errCode != ESP_OK) {
            xSemaphoreGive(gpsMutex);
            return errCode;
        }

        // parse satellite used on channel
        errCode = parseUint8(wordPtr[9], &gsa.satelliteUsedOnChannel7);
        if (errCode != ESP_OK) {
            xSemaphoreGive(gpsMutex);
            return errCode;
        }

        // parse satellite used on channel
        errCode = parseUint8(wordPtr[10], &gsa.satelliteUsedOnChannel8);
        if (errCode != ESP_OK) {
            xSemaphoreGive(gpsMutex);
            return errCode;
        }

        // parse satellite used on channel
        errCode = parseUint8(wordPtr[11], &gsa.satelliteUsedOnChannel9);
        if (errCode != ESP_OK) {
            xSemaphoreGive(gpsMutex);
            return errCode;
        }

        // parse satellite used on channel
        errCode = parseUint8(wordPtr[12], &gsa.satelliteUsedOnChannel10);
        if (errCode != ESP_OK) {
            xSemaphoreGive(gpsMutex);
            return errCode;
        }

        // parse satellite used on channel
        errCode = parseUint8(wordPtr[13], &gsa.satelliteUsedOnChannel11);
        if (errCode != ESP_OK) {
            xSemaphoreGive(gpsMutex);
            return errCode;
        }

        // parse satellite used on channel
        errCode = parseUint8(wordPtr[14], &gsa.satelliteUsedOnChannel12);
        if (errCode != ESP_OK) {
            xSemaphoreGive(gpsMutex);
            return errCode;
        }

        // parse pdop
        errCode = parseFloat(wordPtr[15], &gsa.pdop);
        if (errCode != ESP_OK) {
            xSemaphoreGive(gpsMutex);
            return errCode;
        }

        // parse hdop
        errCode = parseFloat(wordPtr[16], &gsa.hdop);
        if (errCode != ESP_OK) {
            xSemaphoreGive(gpsMutex);
            return errCode;
        }

        // parse vdop
        errCode = parseFloat(wordPtr[17], &gsa.vdop);
        if (errCode != ESP_OK) {
            xSemaphoreGive(gpsMutex);
            return errCode;
        }

        *nmeaDataType = GSA;
        nmeaData->gsa = gsa;
    } else if (!strncmp(wordPtr[0].pWord, "GPGGA", 5)) {
        gga_t gga = {0};
        // parse Time
        errCode = parseTime(wordPtr[1], &gga.time);
        if (errCode != ESP_OK) {
            xSemaphoreGive(gpsMutex);
            return errCode;
        }

        // parse latitude
        errCode = parseCoordinate(wordPtr[2], &gga.latitude);
        if (errCode != ESP_OK) {
            xSemaphoreGive(gpsMutex);
            return errCode;
        }

        // parse N/S indicator
        errCode = parseDirectionIndicator(wordPtr[3], &gga.northSouthIndicator);
        if (errCode != ESP_OK) {
            xSemaphoreGive(gpsMutex);
            return errCode;
        }

        // parse longitude
        errCode = parseCoordinate(wordPtr[4], &gga.longitude);
        if (errCode != ESP_OK) {
            xSemaphoreGive(gpsMutex);
            return errCode;
        }

        // parse E/W indicator
        errCode = parseDirectionIndicator(wordPtr[5], &gga.eastWestIndicator);
        if (errCode != ESP_OK) {
            xSemaphoreGive(gpsMutex);
            return errCode;
        }

        // parse fix indicator
        errCode = parseFixIndicator(wordPtr[6], &gga.fixIndicator);
        if (errCode != ESP_OK) {
            xSemaphoreGive(gpsMutex);
            return errCode;
        }

        // parse satellite number
        errCode = parseUint8(wordPtr[7], &gga.satellitesNumber);
        if (errCode != ESP_OK) {
            xSemaphoreGive(gpsMutex);
            return errCode;
        }

        // parse hdop
        errCode = parseFloat(wordPtr[8], &gga.hdop);
        if (errCode != ESP_OK) {
            xSemaphoreGive(gpsMutex);
            return errCode;
        }

        // parse mls altitude
        errCode = parseFloat(wordPtr[9], &gga.mlsAltitude);
        if (errCode != ESP_OK) {
            xSemaphoreGive(gpsMutex);
            return errCode;
        }

        // parse mls altitude unit
        errCode = parseUnit(wordPtr[10], &gga.mlsAltitudeUnit);
        if (errCode != ESP_OK) {
            xSemaphoreGive(gpsMutex);
            return errCode;
        }

        // parse geoid height
        errCode = parseFloat(wordPtr[11], &gga.geoidHeight);
        if (errCode != ESP_OK) {
            xSemaphoreGive(gpsMutex);
            return errCode;
        }

        // parse geoid height unit
        errCode = parseUnit(wordPtr[12], &gga.geoidHeightUnit);
        if (errCode != ESP_OK) {
            xSemaphoreGive(gpsMutex);
            return errCode;
        }

        // parse age of differential correction
        errCode = parseUint8(wordPtr[13], &gga.ageOfDiffCorr);
        if (errCode != ESP_OK) {
            xSemaphoreGive(gpsMutex);
            return errCode;
        }

        // parse station ID
        errCode = parseUint16(wordPtr[13], &gga.diffRefStationId);
        if (errCode != ESP_OK) {
            xSemaphoreGive(gpsMutex);
            return errCode;
        }

        *nmeaDataType = GGA;
        nmeaData->gga = gga;
    } else if (!strncmp(wordPtr[0].pWord, "GPGLL", 5)) {
        gll_t gll = {0};
        // parse latitude
        errCode = parseCoordinate(wordPtr[1], &gll.latitude);
        if (errCode != ESP_OK) {
            xSemaphoreGive(gpsMutex);
            return errCode;
        }

        // parse N/S indicator
        errCode = parseDirectionIndicator(wordPtr[2], &gll.northSouthIndicator);
        if (errCode != ESP_OK) {
            xSemaphoreGive(gpsMutex);
            return errCode;
        }

        // parse longitude
        errCode = parseCoordinate(wordPtr[3], &gll.longitude);
        if (errCode != ESP_OK) {
            xSemaphoreGive(gpsMutex);
            return errCode;
        }

        // parse E/W indicator
        errCode = parseDirectionIndicator(wordPtr[4], &gll.eastWestIndicator);
        if (errCode != ESP_OK) {
            xSemaphoreGive(gpsMutex);
            return errCode;
        }

        // parse time
        errCode = parseTime(wordPtr[5], &gll.time);
        if (errCode != ESP_OK) {
            xSemaphoreGive(gpsMutex);
            return errCode;
        }

        // parse status
        errCode = parseStatus(wordPtr[6], &gll.status);
        if (errCode != ESP_OK) {
            xSemaphoreGive(gpsMutex);
            return errCode;
        }

        // parse mode
        errCode = parseMode(wordPtr[7], &gll.mode);
        if (errCode != ESP_OK) {
            xSemaphoreGive(gpsMutex);
            return errCode;
        }

        *nmeaDataType = GLL;
        nmeaData->gll = gll;
    } else if (!strncmp(wordPtr[0].pWord, "GPVTG", 5)) {
        vtg_t vtg = {0};
        // parse track degrees
        errCode = parseFloat(wordPtr[1], &vtg.trackDegrees1);
        if (errCode != ESP_OK) {
            xSemaphoreGive(gpsMutex);
            return errCode;
        }

        // parse track degrees unit
        errCode = parseUnit(wordPtr[2], &vtg.trackDegrees1Unit);
        if (errCode != ESP_OK) {
            xSemaphoreGive(gpsMutex);
            return errCode;
        }

        // parse track degrees
        errCode = parseFloat(wordPtr[3], &vtg.trackDegrees2);
        if (errCode != ESP_OK) {
            xSemaphoreGive(gpsMutex);
            return errCode;
        }

        // parse track degrees unit
        errCode = parseUnit(wordPtr[4], &vtg.trackDegrees2Unit);
        if (errCode != ESP_OK) {
            xSemaphoreGive(gpsMutex);
            return errCode;
        }

        // parse speed
        errCode = parseFloat(wordPtr[5], &vtg.speed1);
        if (errCode != ESP_OK) {
            xSemaphoreGive(gpsMutex);
            return errCode;
        }

        // parse speed unit
        errCode = parseUnit(wordPtr[6], &vtg.speed1Unit);
        if (errCode != ESP_OK) {
            xSemaphoreGive(gpsMutex);
            return errCode;
        }

        // parse speed
        errCode = parseFloat(wordPtr[7], &vtg.speed2);
        if (errCode != ESP_OK) {
            xSemaphoreGive(gpsMutex);
            return errCode;
        }

        // parse speed unit
        errCode = parseUnit(wordPtr[8], &vtg.speed2Unit);
        if (errCode != ESP_OK) {
            xSemaphoreGive(gpsMutex);
            return errCode;
        }

        *nmeaDataType = VTG;
        nmeaData->vtg = vtg;
    } else if (!strncmp(wordPtr[0].pWord, "GPGSV", 5)) {
        gsv_t gsv = {0};
        // parse total number of messages
        errCode = parseUint8(wordPtr[1], &gsv.totalNumberOfMessages);
        if (errCode != ESP_OK) {
            xSemaphoreGive(gpsMutex);
            return errCode;
        }

        // parse message number
        errCode = parseUint8(wordPtr[2], &gsv.messageNumber);
        if (errCode != ESP_OK) {
            xSemaphoreGive(gpsMutex);
            return errCode;
        }

        // parse satellites in view
        errCode = parseUint16(wordPtr[3], &gsv.numberOfSatellitesInView);
        if (errCode != ESP_OK) {
            xSemaphoreGive(gpsMutex);
            return errCode;
        }

        *nmeaDataType = GSV;
        nmeaData->gsv = gsv;
    } else {
        *nmeaDataType = NONE;
        xSemaphoreGive(gpsMutex);
        return ESP_FAIL;
    }

    xSemaphoreGive(gpsMutex);
    return errCode;
}
