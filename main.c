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

#define BMP280_DEV_ADDR 0x76
#define BMP280_CAL_DATA_SIZE 24

#define D7_PIN QM_PIN_ID_13 // 0
#define D6_PIN QM_PIN_ID_12 // 1
#define D5_PIN QM_PIN_ID_11 // 2
#define D4_PIN QM_PIN_ID_10 // 3
#define RS_PIN QM_PIN_ID_2 // 5
#define E_PIN QM_PIN_ID_5 // 4

static void pin_setup(void)
{
	qm_pmux_select(D7_PIN, QM_PMUX_FN_0);
	qm_pmux_select(D6_PIN, QM_PMUX_FN_0);
	qm_pmux_select(D5_PIN, QM_PMUX_FN_0);
	qm_pmux_select(D4_PIN, QM_PMUX_FN_0);
	qm_pmux_select(RS_PIN, QM_PMUX_FN_0);
	qm_pmux_select(E_PIN, QM_PMUX_FN_0);

	qm_gpio_port_config_t cfg;
	cfg.direction = (1 << D7_PIN) | (1 << D6_PIN) | (1 << D5_PIN) | (1 << D4_PIN) | (1 << RS_PIN) | (1 << E_PIN);
	qm_gpio_set_config(QM_GPIO_0, &cfg);
}

int main(void)
{
	pin_setup();

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

	I2C_init(BMP280_ADDR_LOW);

	BMP280_config_t bmp_config = {
			.config_flags = BMP280_STBY_TIME_8 | BMP280_FILTER_COEFF_2,
			.control_flags = BMP280_MODE_NORMAL | BMP280_PRES_OVERSAMPLING_X1 | BMP280_TEMP_OVERSAMPLING_X1
	};
	BMP280_init(&bmp_config);

	if(BMP280_detect() == BMP280_ABSENT) {
		HD44780_write_string("Can't find BMP280!");
		while(1);
	}

	HD44780_write_string("BMP280 detected!");
	clk_sys_udelay(2 * 1000 * 1000); // Wait 2 seconds
	HD44780_clear();

	BMP280_meas_t measurement;

	while(1) {
		measurement = BMP280_get_measurement();

		HD44780_gotoxy(1, 1);
		HD44780_write_integer(measurement.temperature / 100, 0);
		HD44780_write_char('.');
		HD44780_write_integer(measurement.temperature % 100, 2);
		HD44780_write_char('C');

		HD44780_gotoxy(2, 1);
		HD44780_write_integer(measurement.pressure / 100, 0);
		HD44780_write_char('.');
		HD44780_write_integer(measurement.pressure % 100, 2);
		HD44780_write_string("hPa");

		clk_sys_udelay(1 * 1000 * 1000); // Wait 1 second
	}

	return 0;
}

