/*
 * HD44780.c
 *
 *  Created on: 16.05.2022
 *      Author: Lefucjusz
 */

#include "HD44780.h"
#include "clk.h"
#include "qm_gpio.h"

/* Needed in HD44780_write_integer() */
#define HD44780_TMP_BUF_SIZE 11 // Value stored in int32_t has at most 10 digits - 2147483647 - one additional byte for null-terminator

#define HD44780_CGRAM_CHAR_SIZE 8 // Each custom char uses 8 bytes

typedef struct {
	uint8_t rows;
	uint8_t columns;
	uint8_t rows_first_addr[4]; // Display has at most 4 rows
} HD44780_type_data_t;

static HD44780_type_data_t HD44780_type_data[DISPLAY_TYPES_NUM] = {
		/* This is super weird display, it's 16x1, but behaves like 8x2 */
		{.rows = 2, .columns = 8, .rows_first_addr[0] = 0x00, .rows_first_addr[1] = 0x40},
		{.rows = 1, .columns = 16, .rows_first_addr[0] = 0x00},
		{.rows = 2, .columns = 16, .rows_first_addr[0] = 0x00, .rows_first_addr[1] = 0x40},
		{.rows = 4, .columns = 16, .rows_first_addr[0] = 0x00, .rows_first_addr[1] = 0x40, .rows_first_addr[2] = 0x10, .rows_first_addr[3] = 0x50},
		{.rows = 2, .columns = 20, .rows_first_addr[0] = 0x00, .rows_first_addr[1] = 0x40},
		{.rows = 4, .columns = 20, .rows_first_addr[0] = 0x00, .rows_first_addr[1] = 0x40, .rows_first_addr[2] = 0x14, .rows_first_addr[3] = 0x54},
		{.rows = 2, .columns = 40, .rows_first_addr[0] = 0x00, .rows_first_addr[1] = 0x40},
};

static HD44780_config_t* HD44780_config;

void HD44780_init(HD44780_config_t* config) {
	HD44780_config = config;

	/* If type is invalid, choose the most popular display */
	if((HD44780_config->type < 0) || (HD44780_config->type >= DISPLAY_TYPES_NUM)) {
		HD44780_config->type = DISPLAY_16x2;
	}

	/* WTF?! Weird 4-bit init instructions... */
	HD44780_write_cmd(0x30);
	clk_sys_udelay(5000);
	HD44780_write_cmd(0x30);
	clk_sys_udelay(1000);
	HD44780_write_cmd(0x30);
	clk_sys_udelay(1000);
	HD44780_write_cmd(0x02);

	/* Here begins the real configuration */
	HD44780_type_data_t type_data = HD44780_type_data[HD44780_config->type];
	if(type_data.rows == 1) {
		HD44780_write_cmd(FUNCTION_SET_CMD | ONE_LINE); // Initialize as 1 line, 5x8 matrix, 4-bit interface
	} else {
		HD44780_write_cmd(FUNCTION_SET_CMD | TWO_LINES); // Initialize as 2 lines, 5x8 matrix, 4-bit interface
	}

	HD44780_write_cmd(DISPLAY_ON_OFF_CMD | HD44780_config->on_off_flags); // Set display on/off flags
	HD44780_write_cmd(ENTRY_MODE_SET_CMD | HD44780_config->entry_mode_flags); // Set entry mode flags

	HD44780_clear();
}

void HD44780_write_byte(uint8_t byte, HD44780_mode_t mode) {
	/* Set RS line state */
	qm_gpio_set_pin_state(QM_GPIO_0, HD44780_config->RS, mode == CHARACTER ? QM_GPIO_HIGH : QM_GPIO_LOW);

	/* Write upper nibble */
	qm_gpio_set_pin_state(QM_GPIO_0, HD44780_config->D7, (byte & (1 << 7)) ? QM_GPIO_HIGH : QM_GPIO_LOW);
	qm_gpio_set_pin_state(QM_GPIO_0, HD44780_config->D6, (byte & (1 << 6)) ? QM_GPIO_HIGH : QM_GPIO_LOW);
	qm_gpio_set_pin_state(QM_GPIO_0, HD44780_config->D5, (byte & (1 << 5)) ? QM_GPIO_HIGH : QM_GPIO_LOW);
	qm_gpio_set_pin_state(QM_GPIO_0, HD44780_config->D4, (byte & (1 << 4)) ? QM_GPIO_HIGH : QM_GPIO_LOW);

	/* Pulse enable signal */
	qm_gpio_set_pin_state(QM_GPIO_0, HD44780_config->E, QM_GPIO_HIGH);
	clk_sys_udelay(1); // Wait 1us
	qm_gpio_set_pin_state(QM_GPIO_0, HD44780_config->E, QM_GPIO_LOW);

	/* Write lower nibble */
	qm_gpio_set_pin_state(QM_GPIO_0, HD44780_config->D7, (byte & (1 << 3)) ? QM_GPIO_HIGH : QM_GPIO_LOW);
	qm_gpio_set_pin_state(QM_GPIO_0, HD44780_config->D6, (byte & (1 << 2)) ? QM_GPIO_HIGH : QM_GPIO_LOW);
	qm_gpio_set_pin_state(QM_GPIO_0, HD44780_config->D5, (byte & (1 << 1)) ? QM_GPIO_HIGH : QM_GPIO_LOW);
	qm_gpio_set_pin_state(QM_GPIO_0, HD44780_config->D4, (byte & (1 << 0)) ? QM_GPIO_HIGH : QM_GPIO_LOW);

	/* Pulse enable signal */
	qm_gpio_set_pin_state(QM_GPIO_0, HD44780_config->E, QM_GPIO_HIGH);
	clk_sys_udelay(1); // Wait 1us
	qm_gpio_set_pin_state(QM_GPIO_0, HD44780_config->E, QM_GPIO_LOW);
}

void HD44780_write_cmd(uint8_t command) {
	HD44780_write_byte(command, INSTRUCTION);
}

void HD44780_write_char(char character) {
	HD44780_write_byte(character, CHARACTER);
}

void HD44780_clear(void) {
	/* Clear display */
	HD44780_write_cmd(CLEAR_DISPLAY_CMD);
	clk_sys_udelay(1000); // Wait 1ms

	/* Set cursor to first line */
	HD44780_type_data_t type_data = HD44780_type_data[HD44780_config->type];
	HD44780_write_cmd(SET_DDRAM_ADDR_CMD | type_data.rows_first_addr[0]);
	clk_sys_udelay(1000); // Wait 1ms
}

void HD44780_gotoxy(uint8_t x, uint8_t y) {
	/* Get data for selected display type */
	HD44780_type_data_t type_data = HD44780_type_data[HD44780_config->type];

	/* If rows out of range, select the last one */
	if(x > type_data.rows) {
		x = type_data.rows;
	}

	/* If columns out of range, select the last one */
	if(y > type_data.columns) {
		y = type_data.columns;
	}

	/* Get address of selected row, add column offset */
	uint8_t required_address = type_data.rows_first_addr[x - 1] + y - 1;

	/* Move to requested position */
	HD44780_write_cmd(SET_DDRAM_ADDR_CMD | required_address);
}

void HD44780_write_integer(int32_t number, uint8_t required_length) {
	/* If number is negative, display minus sign and convert it to positive */
	if(number < 0) {
		HD44780_write_char('-');
		number = -number;
	}

	char buffer[HD44780_TMP_BUF_SIZE] = {0};
	char* buffer_ptr = &buffer[HD44780_TMP_BUF_SIZE - 1]; // Set buffer pointer to last element
	uint8_t digit_count = 0;

	while(number > 0) {
		/* Decrementing has to be here, not at the end of loop, so that null-terminator at the end is preserved
		 * and buffer_ptr is properly aligned after exiting the while loop */
		buffer_ptr--;
		*buffer_ptr = (number % 10) + '0'; // Insert digits from the end of the array and move towards the beginning
		number /= 10;
		digit_count++;
	}

	/* Leading zeros handling - compute how many should be appended to get the required length and display them */
	int8_t leading_zeros = required_length - digit_count;
	while(leading_zeros > 0) {
		HD44780_write_char('0');
		leading_zeros--;
	}

	HD44780_write_string(buffer_ptr);
}

void HD44780_write_string(const char* string) {
	while(*string) {
		HD44780_write_char(*string++);
	}
}

void HD44780_load_custom_glyph(const uint8_t* glyph_array, HD44780_glyph_addr_t cgram_addr) {
	/* If provided address out of range, select last one */
	if(cgram_addr > CUSTOM_GLYPH_7) {
		cgram_addr = CUSTOM_GLYPH_7;
	}

	/* Set CGRAM pointer to required location */
	uint8_t cgram_offset = cgram_addr * HD44780_CGRAM_CHAR_SIZE;
	HD44780_write_cmd(SET_CGRAM_ADDR_CMD | cgram_offset);

	/* Load character */
	for(uint8_t i = 0; i < HD44780_CGRAM_CHAR_SIZE; i++) {
		HD44780_write_char(glyph_array[i]);
	}
}

void HD44780_load_custom_glyphs(const uint8_t* glyphs_array) {
	for(HD44780_glyph_addr_t i = CUSTOM_GLYPH_0; i < CUSTOM_GLYPHS_NUM; i++) {
		HD44780_load_custom_glyph(&glyphs_array[i * HD44780_CGRAM_CHAR_SIZE], i);
	}
}
