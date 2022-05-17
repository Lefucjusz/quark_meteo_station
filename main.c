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

#define BMP280_DEV_ADDR 0x76
#define BMP280_CAL_DATA_SIZE 26

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

typedef union
{
	uint8_t regs[BMP280_CAL_DATA_SIZE];
	struct
	{
		uint16_t T1;
		int16_t T2;
		int16_t T3;

		uint16_t P1;
		int16_t P2;
		int16_t P3;
		int16_t P4;
		int16_t P5;
		int16_t P6;
		int16_t P7;
		int16_t P8;
		int16_t P9;
	};
} bmp280_cal_t;

bmp280_cal_t cal;

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
			.type = DISPLAY_20x2,
			.entry_mode_flags = INCREASE_CURSOR_ON,
			.on_off_flags = DISPLAY_ON
	};

	HD44780_init(&lcd_config);

	I2C_init(BMP280_DEV_ADDR);

	uint8_t bmp_id = I2C_read_byte(0xD0);

	if(I2C_error()) {
		HD44780_write_string("I2C error! :(");
	} else if(bmp_id == 0x58) {
		HD44780_write_string("BMP detected!");
	} else {
		HD44780_write_string("BMP not found! :(");
	}

	while(1);

	return 0;
}

