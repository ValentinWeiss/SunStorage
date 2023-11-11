#include "lcdDisplay.h"

lcd_4bit_t lcd;

// Write Data is latched on the falling edge of E(nable)
void lcdPulseEnable(lcd_4bit_t* lcd) {
    // set E low
    gpio_set_level(lcd->e, 0);
    ets_delay_us(1);
    // set E high
    gpio_set_level(lcd->e, 1);
    ets_delay_us(1);  // enable pulse must be > 450ns
    // set E low
    gpio_set_level(lcd->e, 0);
    ets_delay_us(100);  // commands need > 37us to settle
}

// writes the lower 4 bits from value to the lcd
void lcdWrite4Bit(lcd_4bit_t* lcd, uint8_t value) {
    gpio_set_level(lcd->d4, (value & 1));
    gpio_set_level(lcd->d5, (value & 2));
    gpio_set_level(lcd->d6, (value & 4));
    gpio_set_level(lcd->d7, (value & 8));

    lcdPulseEnable(lcd);
}

void lcdWriteInstruction4Bit(lcd_4bit_t* lcd, uint8_t value) {
    // set RS to low
    gpio_set_level(lcd->rs, 0);
    // set E to low, just to make sure E is low while writing to lcd
    gpio_set_level(lcd->e, 0);

    // send upper 4 bits
    lcdWrite4Bit(lcd, value >> 4);

    ets_delay_us(100);

    // send lower 4 bits
    lcdWrite4Bit(lcd, value);
}

void lcdWriteCharacter4Bit(lcd_4bit_t* lcd, uint8_t value) {
    // set RS to high
    gpio_set_level(lcd->rs, 1);
    // set E to low
    gpio_set_level(lcd->e, 0);

    // send upper 4 bits
    lcdWrite4Bit(lcd, value >> 4);

    ets_delay_us(100);

    // send lower 4 bits
    lcdWrite4Bit(lcd, value);
}

// at most 80 characters per string; 20 per line
void lcdWriteString4Bit(lcd_4bit_t* lcd, char* value) {
    int row = 0;
    int cell = 0;

    const uint32_t rowAddresses[] = {LCD_LINE_ONE, LCD_LINE_TWO, LCD_LINE_THREE, LCD_LINE_FOUR};

    lcdClear(lcd);

    for (int i = 0; value[i] != '\0'; i++) {
        if (value[i] == '\n') {
            row++;
            cell = 0;
        } else {
            lcdWriteCharacter4Bit(lcd, (uint8_t)value[i]);
            cell++;
        }

        if (cell % LCD_LINE_LENGTH == 0 && cell != 0) {
            row++;
            cell = 0;
        }

        if (cell == 0) {
            lcdSetCursor4Bit(lcd, rowAddresses[row]);
        }

        ets_delay_us(80);  // 40us delay min
    }
}

void lcdShiftDisplayLeft4Bit(lcd_4bit_t* lcd, uint8_t n) {
    for (int i = 0; i < n; i++) {
        lcdWriteInstruction4Bit(lcd, 0x18);
    }
}

void lcdShiftDisplayRight4Bit(lcd_4bit_t* lcd, uint8_t n) {
    for (int i = 0; i < n; i++) {
        lcdWriteInstruction4Bit(lcd, 0x1C);
    }
}

// address in hex !
void lcdSetCursor4Bit(lcd_4bit_t* lcd, uint8_t address) {
    lcdWriteInstruction4Bit(lcd, LCD_SET_CURSOR | address);
    ets_delay_us(80);
}

void lcdClear(lcd_4bit_t* lcd) {
    lcdWriteInstruction4Bit(lcd, LCD_CLEAR);
    vTaskDelay(8 / portTICK_PERIOD_MS);  // = 4ms; lcd ~2ms delay
}

void lcdOn(lcd_4bit_t* lcd) {
    gpio_set_level(lcd->on_off, 1);
}

void lcdOff(lcd_4bit_t* lcd) {
    gpio_set_level(lcd->on_off, 0);
}

esp_err_t lcdInit4BitDefault(lcd_4bit_t* lcd) {
    return lcdInit4Bit(lcd, LCD_PIN_E, LCD_PIN_RS, LCD_PIN_D4, LCD_PIN_D5, LCD_PIN_D6, LCD_PIN_D7, LCD_PIN_ON_OFF);
}

esp_err_t lcdInit4Bit(lcd_4bit_t* lcd, gpio_num_t e, gpio_num_t rs, gpio_num_t d4, gpio_num_t d5, gpio_num_t d6, gpio_num_t d7, gpio_num_t on_off) {
    esp_err_t ret = ESP_OK;

    lcd->e = e;
    lcd->rs = rs;
    lcd->d4 = d4;
    lcd->d5 = d5;
    lcd->d6 = d6;
    lcd->d7 = d7;
    lcd->on_off = on_off;

    gpio_config_t io_config;
    io_config.pin_bit_mask = (1ULL << lcd->e) | (1ULL << lcd->rs) | (1ULL << lcd->d4) | (1ULL << lcd->d5) | (1ULL << lcd->d6) | (1ULL << lcd->d7) | (1ULL << lcd->on_off);
    io_config.mode = GPIO_MODE_OUTPUT;
    io_config.pull_up_en = GPIO_PULLUP_DISABLE;
    io_config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_config.intr_type = GPIO_INTR_DISABLE;
    ret = gpio_config(&io_config);
    if (ret != ESP_OK) ESP_LOGE("initLcd", "failed to init lcd");

    // Power-up delay
    ets_delay_us(100);  // initial 40 mSec delay

    // set RS to low
    gpio_set_level(lcd->rs, 0);

    // set E to low
    gpio_set_level(lcd->e, 0);

    // first reset
    lcdWrite4Bit(lcd, LCD_FUNCTION_RESET);
    vTaskDelay(10 / portTICK_PERIOD_MS);  // = 10ms; lcd delay is about ~4.1ms

    // second reset
    lcdWrite4Bit(lcd, LCD_FUNCTION_RESET);
    vTaskDelay(10 / portTICK_PERIOD_MS);  // = 10ms; lcd delay is about ~4.1ms

    // third reset, might not be necessary but I'm doing it anyway
    lcdWrite4Bit(lcd, LCD_FUNCTION_RESET);
    vTaskDelay(10 / portTICK_PERIOD_MS);  // = 10ms; lcd delay is about ~4.1ms

    lcdWriteInstruction4Bit(lcd, LCD_FUNCTION_SET_4BIT);
    ets_delay_us(80);

    lcdWriteInstruction4Bit(lcd, LCD_DISPLAY_OFF);
    ets_delay_us(80);

    lcdWriteInstruction4Bit(lcd, LCD_CLEAR);  // clears display
    vTaskDelay(4 / portTICK_PERIOD_MS);       // = 4ms; lcd ~2ms delay

    lcdWriteInstruction4Bit(lcd, LCD_ENTRY_MODE);
    ets_delay_us(80);

    lcdWriteInstruction4Bit(lcd, LCD_DISPLAY_ON);  // turn the display ON
    ets_delay_us(80);                              // 40 uS delay (min)

    vTaskDelay(100 / portTICK_PERIOD_MS);  // just to give it some time to settle

    return ret;
}

esp_err_t initLcdDisplay() {
    esp_err_t errCode = ESP_OK;

    errCode = lcdInit4BitDefault(&lcd);
    if (errCode != ESP_OK) {
        ESP_LOGE("display_task", "Initalization of lcd Display failed");
    }

    return errCode;
}