#include "dcf77.h"

// signal
volatile uint8_t DCF77_SignalState = 0;
volatile uint8_t DCF77_prevSignalState = 0;

int leapSecond = 0;  // 1 if leap second on buffer 19 is set
int leapSecBit = 0;  // value of leap second bit (should be 0)

// flanks
uint64_t leadingEdge = 0;
uint64_t trailingEdge = 0;
uint64_t previousLeadingEdge = 0;

// buffer
uint64_t dcf_bit_buffer;                                   // bits 59...0 are used to store the 60 bits
const uint8_t bitValues[] = {1, 2, 4, 8, 10, 20, 40, 80};  // decimal values of bits
uint64_t dcf_bit_buffer_final;
uint8_t bufferPosition = 0;
uint8_t lastBufferPosition = 0;

// error handling
bool dcfValidSignal = false;
bool lastDcfValidSignal = false;

// access control
volatile bool finalizing = false;

// last valid date received
dcf77_date_t lastValidDate = {UINT8_MAX, UINT8_MAX, UINT8_MAX, UINT8_MAX, UINT8_MAX, UINT8_MAX, true, true};

// function declararations for use only in dcf77.c
void copyBuffer();
uint64_t millis();
void processBit(uint64_t bit);
void decodeBufferContents();
uint16_t bitDecode(uint64_t buffer, uint8_t bitStart, uint8_t bitEnd);
void finalizeBuffer();
bool checkParity();
uint8_t getBit(uint64_t buffer, uint8_t bit);

esp_err_t dcf77Init() {
    esp_err_t errCode = ESP_OK;

    // set input pin
    errCode = gpio_set_direction(DCF77_PIN, GPIO_MODE_INPUT);
    if (errCode != ESP_OK) return errCode;
    gpio_set_pull_mode(DCF77_PIN, GPIO_PULLUP_ENABLE);
    if (errCode != ESP_OK) return errCode;

    return errCode;
}

uint64_t millis() {
    int64_t current_time = esp_timer_get_time();
    uint64_t millis = current_time / 1000;
    return millis;
}

void copyBuffer() {
    lastBufferPosition = bufferPosition;
    lastDcfValidSignal = dcfValidSignal;

    dcf_bit_buffer_final = dcf_bit_buffer;
}

void dcf77ReadSignal() {
    static bool errorCondition;

    // check for rising edge
    if (DCF77_SignalState == 1) {
        leadingEdge = millis();
        return;
    }

    // check for falling edge
    if (DCF77_SignalState == 0) {
        trailingEdge = millis();

        // check if this rising flank was detected too quickly after previous rising flank
        if ((leadingEdge - previousLeadingEdge) < 900) {
            errorCondition = true;
            ESP_LOGE("dcf77ReadSignal", "Rising flank came to quickly");
        }

        // check if detected pulse too short or too long
        if (((trailingEdge - leadingEdge) < 70) || ((trailingEdge - leadingEdge) > 230)) {
            errorCondition = true;
            ESP_LOGE("dcf77ReadSignal", "Pulse is too long or short");
        }

        // if error detected, start over
        if (errorCondition) {
            errorCondition = false;
            previousLeadingEdge = leadingEdge;
            ESP_LOGE("dcf77ReadSignal", "Starting over");
            return;
        }

        // no errors...

        // end of minute check, gap of appr. 2000ms
        if (((leadingEdge - previousLeadingEdge) > 1900) && ((leadingEdge - previousLeadingEdge) < 2100)) {
            dcfValidSignal = checkParity();

            copyBuffer();

            // reset dcf_bit_buffer
            dcf_bit_buffer = 0;
            bufferPosition = 0;

            finalizeBuffer();
        }

        previousLeadingEdge = leadingEdge;

        // distinguish between 1's and 0's
        if (trailingEdge - leadingEdge < 170) {
            processBit(0);
        } else {
            processBit(1);
        }
    }
}

bool checkParity() {
    // minute parity check
    uint8_t minuteParityCounter = 0;
    bool parityCheckMinute = false;

    for (uint8_t i = 21; i <= 27; i++) {
        minuteParityCounter += getBit(dcf_bit_buffer, i);
    }
    // if even number of 1's
    if ((getBit(dcf_bit_buffer, 28) + minuteParityCounter) % 2 == 0) {
        parityCheckMinute = true;
    } else {
        ESP_LOGE("dcf77_checkParity", "Failed Parity Check Minute");
        return false;
    }

    // hour parity check
    uint8_t hourParityCounter = 0;
    bool parityCheckHour = false;

    for (int i = 29; i <= 34; i++) {
        hourParityCounter += getBit(dcf_bit_buffer, i);
    }
    // if even number of 1's
    if ((getBit(dcf_bit_buffer, 35) + hourParityCounter) % 2 == 0) {
        parityCheckHour = true;
    } else {
        ESP_LOGE("dcf77_checkParity", "Failed Parity Check Hour");
        return false;
    }

    // data parity check
    uint8_t dataParityCounter = 0;
    bool parityCheckData = false;

    for (int i = 36; i <= 57; i++) {
        dataParityCounter += getBit(dcf_bit_buffer, i);
    }
    // if even number of 1's
    if ((getBit(dcf_bit_buffer, 58) + dataParityCounter) % 2 == 0) {
        parityCheckData = true;
    } else {
        ESP_LOGE("dcf77_checkParity", "Failed Parity Check Data");
        return false;
    }

    // finally, check all parity bits
    if (parityCheckMinute == true && parityCheckHour == true && parityCheckData == true) {
        return true;
    }

    return false;
}

void processBit(uint64_t bit) {
    dcf_bit_buffer |= (bit << bufferPosition);

    if (bufferPosition == 19) {
        leapSecond = bit;
    }

    // handle possible leap second
    if (bufferPosition == 59) {
        leapSecBit = bit;
    }

    bufferPosition++;

    // check for buffer overflow
    if (bufferPosition == 60 && leapSecond == 1) {
        if (bufferPosition > 60) {
            bufferPosition = 0;
            ESP_LOGE("dcf77_processBit", "Buffer Overflow");
            return;
        }
    } else if (bufferPosition > 59) {
        bufferPosition = 0;
        ESP_LOGE("dcf77_processBit", "Buffer Overflow");
        return;
    }
}

void finalizeBuffer() {
    if (finalizing) {
        return;
    }
    finalizing = true;

    if (lastBufferPosition == 60 && lastDcfValidSignal == true && leapSecond == 1 && lastValidDate.minute == 59 && leapSecBit == 0) {
        decodeBufferContents();
    } else if (lastBufferPosition == 59 && lastDcfValidSignal == true) {
        decodeBufferContents();
    } else {  // if (bufferPosition == 59)
        ESP_LOGE("dcf77_finalizeBuffer", "Signal not valid");
        dcfValidSignal = false;
    }

    finalizing = false;
}

bool isLeapYear(uint16_t year) {
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
        return true;
    } else {
        return false;
    }
}

void decodeBufferContents() {
    // decodes buffer
    lastValidDate.minute = bitDecode(dcf_bit_buffer_final, 21, 27);
    lastValidDate.hour = bitDecode(dcf_bit_buffer_final, 29, 34);
    lastValidDate.day = bitDecode(dcf_bit_buffer_final, 36, 41);
    lastValidDate.weekDay = bitDecode(dcf_bit_buffer_final, 42, 44);
    lastValidDate.month = bitDecode(dcf_bit_buffer_final, 45, 49);
    lastValidDate.year = bitDecode(dcf_bit_buffer_final, 50, 57) + 2000;

    // Summertime bit. '1' = Summertime, '0' = wintertime
    lastValidDate.dst = bitDecode(dcf_bit_buffer_final, 17, 17);

    lastValidDate.leapYear = isLeapYear(lastValidDate.year);
}

uint16_t bitDecode(uint64_t buffer, uint8_t bitStart, uint8_t bitEnd) {
    uint16_t value = 0;
    uint8_t i = 0;

    while (bitStart <= bitEnd) {
        if (getBit(buffer, bitStart) == 1) {
            value = value + bitValues[i];
        }
        bitStart++;
        i++;
    }
    return value;
}

uint8_t getBit(uint64_t buffer, uint8_t bit) {
    return (buffer >> bit) & 1;
}

void printDCF77Date(dcf77_date_t* date) {
    printf("Minute: %d Hour: %d Day: %d WeekDay: %d Month: %d Year: %d DST: %d leapYear: %d\n", date->minute, date->hour, date->day, date->weekDay, date->month, date->year, date->dst, date->leapYear);
}

bool dcf77ReadTime(dcf77_date_t* date) {
    date->minute = lastValidDate.minute;
    date->hour = lastValidDate.hour;
    date->day = lastValidDate.day;
    date->weekDay = lastValidDate.weekDay;
    date->month = lastValidDate.month;
    date->year = lastValidDate.year;
    date->dst = lastValidDate.dst;
    date->leapYear = lastValidDate.leapYear;

    // checks if signal is valid
    if (dcfValidSignal == false) {
        return false;
    }

    // checks if signal is currently being decoded
    if (finalizing == true) {
        return false;
    }

    return true;
}
