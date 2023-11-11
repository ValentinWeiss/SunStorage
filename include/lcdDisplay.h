#ifndef LCD_H
#define LCD_H

#include <rom/ets_sys.h>

#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "settings.h"

// lcd commands
#define LCD_CLEAR 0x01              // replace all characters with ASCII 'space', return cursor to first position on first line, set DDRAM address to 00H
#define LCD_HOME 0x02               // return cursor to first position on first line
#define LCD_ENTRY_MODE 0x06         // shift cursor from left to right on read/write
#define LCD_DISPLAY_OFF 0x08        // turn display off
#define LCD_DISPLAY_ON 0x0C         // display on, cursor off, don't blink cursor
#define LCD_DISPLAY_ON_CURSOR 0x0E  // display on, cursor on, don't blink cursor
#define LCD_FUNCTION_RESET 0x30     // reset the LCD
#define LCD_FUNCTION_SET_4BIT 0x28  // 4-bit data, 2-line display, 5 x 8 font
#define LCD_SET_CURSOR 0x80         // set cursor position (set DDRAM Address)

#define LCD_LINE_ONE 0x00    // start of first line; Addresses range from 0x00 to 0x13
#define LCD_LINE_TWO 0x40    // start of second line; Addresses range from 0x40 to 0x53
#define LCD_LINE_THREE 0x14  // start of third line; Addresses range from 0x14 to 0x27
#define LCD_LINE_FOUR 0x54   // start of fourth line; Addresses range from 0x54 to 0x67
#define LCD_LINE_LENGTH 0x14
#define LCD_NUMBER_ROWS 4

typedef struct {
    gpio_num_t e;
    gpio_num_t rs;
    gpio_num_t d4;
    gpio_num_t d5;
    gpio_num_t d6;
    gpio_num_t d7;
    gpio_num_t on_off;
} lcd_4bit_t;

extern lcd_4bit_t lcd;

esp_err_t initLcdDisplay();
void lcdDelayMs(uint16_t ms);
void lcdPulseEnable(lcd_4bit_t* lcd);
void lcdWrite4Bit(lcd_4bit_t* lcd, uint8_t value);
void lcdWriteInstruction4Bit(lcd_4bit_t* lcd, uint8_t value);
void lcdWriteCharacter4Bit(lcd_4bit_t* lcd, uint8_t value);
void lcdWriteString4Bit(lcd_4bit_t* lcd, char* value);
void lcdShiftDisplayLeft4Bit(lcd_4bit_t* lcd, uint8_t n);
void lcdShiftDisplayRight4Bit(lcd_4bit_t* lcd, uint8_t n);
void lcdSetCursor4Bit(lcd_4bit_t* lcd, uint8_t address);
void lcdClear(lcd_4bit_t* lcd);
void lcdOff(lcd_4bit_t* lcd);
void lcdOn(lcd_4bit_t* lcd);
esp_err_t lcdInit4Bit(lcd_4bit_t* lcd, gpio_num_t e, gpio_num_t rs, gpio_num_t d4, gpio_num_t d5, gpio_num_t d6, gpio_num_t d7, gpio_num_t on_off);
esp_err_t lcdInit4BitDefault(lcd_4bit_t* lcd);

#endif