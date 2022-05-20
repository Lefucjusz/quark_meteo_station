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
#include "DS18B20.h"

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

	gpio_config->direction = (1 << D7_PIN) | (1 << D6_PIN) | (1 << D5_PIN) | (1 << D4_PIN) | (1 << RS_PIN) | (1 << E_PIN);
	qm_gpio_set_config(QM_GPIO_0, gpio_config);
}

static void display_layout(void) {
	HD44780_write_string("I: ");
	HD44780_gotoxy(2, 1);
	HD44780_write_string("P: ");
	HD44780_gotoxy(1, 12);
	HD44780_write_string("O: ");
	HD44780_gotoxy(2, 15);
	HD44780_write_string("H: ");
}

static void display_DS18B20(const DS18B20_meas_t* const measurement) {
	HD44780_gotoxy(1, 15);
	if(measurement->sign == DS18B20_NEGATIVE) {
		HD44780_write_char('-');
	}
	HD44780_write_integer(measurement->integer, 0);
	HD44780_write_char('.');
	HD44780_write_integer(measurement->fraction, 2);
	HD44780_write_char('C');
}

static void display_BMP280(const BMP280_meas_t* const measurement) {
	HD44780_gotoxy(1, 4);
	HD44780_write_integer(measurement->temperature / 100, 0);
	HD44780_write_char('.');
	HD44780_write_integer(measurement->temperature % 100, 2);
	HD44780_write_string("C");

	HD44780_gotoxy(2, 4);
	HD44780_write_integer(measurement->pressure / 100, 0);
	HD44780_write_char('.');
	HD44780_write_integer(measurement->pressure % 100, 2);
	HD44780_write_string("hPa");
}

typedef enum {
	REQUEST_CONVERSION,
	GET_MEASUREMENT
} state_t;

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

	display_layout();

	onewire_config_t onewire_config = {
			.gpio_config = &gpio_config,
			.onewire_pin = ONEWIRE_PIN
	};
	onewire_init(&onewire_config);

	I2C_init(BMP280_ADDR_LOW);

	BMP280_config_t bmp280_config = {
			.config_flags = BMP280_STBY_TIME_1S | BMP280_FILTER_COEFF_2,
			.control_flags = BMP280_MODE_NORMAL | BMP280_PRES_OVERSAMPLING_X1 | BMP280_TEMP_OVERSAMPLING_X1
	};
	BMP280_init(&bmp280_config);

	state_t state = REQUEST_CONVERSION;
	DS18B20_meas_t DS18B20_meas;
	BMP280_meas_t BMP280_meas;

	while(1) {
		switch(state) {
			case REQUEST_CONVERSION:
				DS18B20_request_conversion();
				state = GET_MEASUREMENT;
				break;
			case GET_MEASUREMENT:
				DS18B20_meas = DS18B20_get_temperature();
				display_DS18B20(&DS18B20_meas);
				state = REQUEST_CONVERSION;
				break;
			default:
				break;
		}

		BMP280_meas = BMP280_get_measurement();
		display_BMP280(&BMP280_meas);

		clk_sys_udelay(500 * 1000); // Wait for 500ms
	}

	return 0;
}

