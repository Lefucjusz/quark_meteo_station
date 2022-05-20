/*
 * main.c
 *
 *  Created on: 16.05.2022
 *      Author: Lefucjusz
 */

#include "clk.h"
#include "qm_gpio.h"
#include "qm_interrupt.h"
#include "qm_interrupt_router.h"
#include "qm_isr.h"
#include "qm_pinmux.h"
#include "qm_pwm.h"
#include "HD44780.h"
#include "I2C.h"
#include "BMP280.h"
#include "onewire.h"

#define D7_PIN QM_PIN_ID_13 // 0
#define D6_PIN QM_PIN_ID_12 // 1
#define D5_PIN QM_PIN_ID_11 // 2
#define D4_PIN QM_PIN_ID_10 // 3
#define RS_PIN QM_PIN_ID_2 // 5
#define E_PIN QM_PIN_ID_5 // 4

#define ONEWIRE_PIN QM_PIN_ID_8 // 7

static void pin_setup(qm_gpio_port_config_t* const gpio_config)
{
	qm_pmux_select(D7_PIN, QM_PMUX_FN_0);
	qm_pmux_select(D6_PIN, QM_PMUX_FN_0);
	qm_pmux_select(D5_PIN, QM_PMUX_FN_0);
	qm_pmux_select(D4_PIN, QM_PMUX_FN_0);
	qm_pmux_select(RS_PIN, QM_PMUX_FN_0);
	qm_pmux_select(E_PIN, QM_PMUX_FN_0);

	qm_pmux_select(ONEWIRE_PIN, QM_PMUX_FN_0);
	qm_pmux_pullup_en(ONEWIRE_PIN, true);

	gpio_config->direction = (1 << D7_PIN) | (1 << D6_PIN) | (1 << D5_PIN) | (1 << D4_PIN) | (1 << RS_PIN) | (1 << E_PIN);
	qm_gpio_set_config(QM_GPIO_0, gpio_config);
}

int main(void)
{
	qm_gpio_port_config_t gpio_config;
	pin_setup(&gpio_config);

	HD44780_config_t lcd_config = {
			.D4 = D4_PIN,
			.D5 = D5_PIN,
			.D6 = D6_PIN,
			.D7 = D7_PIN,
			.RS = RS_PIN,
			.E = E_PIN,
			.type = HD44780_DISPLAY_20x2,
			.entry_mode_flags = HD44780_INCREASE_CURSOR_ON,
			.on_off_flags = HD44780_DISPLAY_ON
	};
	HD44780_init(&lcd_config);

	onewire_config_t onewire_config = {
			.gpio_config = &gpio_config,
			.onewire_pin = ONEWIRE_PIN
	};
	onewire_init(&onewire_config);

	onewire_detect_t response = onewire_reset();

	if(response == ONEWIRE_ABSENT) {
		HD44780_write_string("DS18B20 absent :(");
		while(1);
	}

	HD44780_write_string("DS18B20 present!");

	while(1) {
		onewire_reset();
		onewire_write_byte(0xCC);
		onewire_write_byte(0x44);
		clk_sys_udelay(1 * 1000 * 1000);

		onewire_reset();
		onewire_write_byte(0xCC);
		onewire_write_byte(0xBE);
		uint8_t lsb = onewire_read_byte();
		uint8_t msb = onewire_read_byte();

		uint8_t integer = (uint8_t)( ((msb << 4) | (lsb >> 4)) & 0x7F);
		uint8_t fraction = (uint8_t)( ((lsb & 0x0F) * 625) / 100);

		HD44780_clear();
		HD44780_write_integer(integer, 0);
		HD44780_write_char('.');
		HD44780_write_integer(fraction, 2);
		HD44780_write_char('C');
	}


	return 0;
}

