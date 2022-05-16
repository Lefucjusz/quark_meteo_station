/*
 * HD44780.h
 *
 *  Created on: 16.05.2022
 *      Author: Lefucjusz
 */

#ifndef HD44780_H_
#define HD44780_H_

#include <stdint.h>

//TODO add function descriptions

typedef enum {
	CLEAR_DISPLAY_CMD = 0x01,
	ENTRY_MODE_SET_CMD = 0x04,
	DISPLAY_ON_OFF_CMD = 0x08,
	CURSOR_OR_DISP_SHIFT_MODE_CMD = 0x10, // This is such a useless command I haven't implemented it yet
	FUNCTION_SET_CMD = 0x20,
	SET_CGRAM_ADDR_CMD = 0x40,
	SET_DDRAM_ADDR_CMD = 0x80
} HD44780_commands;

typedef enum {
	ONE_LINE = 0x00,
	TWO_LINES = 0x08
} HD44780_lines;

typedef enum {
	DISPLAY_SCROLL_ON = 0x01,
	INCREASE_CURSOR_ON = 0x02,
} HD44780_entry_mode_flags;

typedef enum {
	CURSOR_BLINK_ON = 0x01,
	CURSOR_ON = 0x02,
	DISPLAY_ON = 0x04
} HD44780_on_off_flags;

typedef enum {
	DISPLAY_16x1_TYPE_1,
	DISPLAY_16x1_TYPE_2,
	DISPLAY_16x2,
	DISPLAY_16x4,
	DISPLAY_20x2,
	DISPLAY_20x4,
	DISPLAY_40x2,
	DISPLAY_TYPES_NUM // This value equals to number of display types
} HD44780_type_t;

typedef enum {
	INSTRUCTION,
	CHARACTER
} HD44780_mode_t;

typedef struct {
	uint8_t RS;
	uint8_t E;
	uint8_t D4;
	uint8_t D5;
	uint8_t D6;
	uint8_t D7;
	HD44780_type_t type;
	uint8_t entry_mode_flags;
	uint8_t on_off_flags;
} HD44780_config_t;

void HD44780_init(HD44780_config_t* config);

void HD44780_write_byte(uint8_t byte, HD44780_mode_t mode);

void HD44780_write_cmd(uint8_t command);

void HD44780_write_char(char character);

void HD44780_clear(void);

void HD44780_gotoxy(uint8_t x, uint8_t y);

void HD44780_num(uint8_t number); //TODO change so that longer numbers can be displayed too with leading zeros

void HD44780_write_string(const char* string);

#endif /* HD44780_H_ */
