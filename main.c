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
#include "DHT11.h"
#include "GUI.h"

#define D7_PIN QM_PIN_ID_13 // 0
#define D6_PIN QM_PIN_ID_12 // 1
#define D5_PIN QM_PIN_ID_11 // 2
#define D4_PIN QM_PIN_ID_10 // 3
#define RS_PIN QM_PIN_ID_2 // 5
#define E_PIN QM_PIN_ID_5 // 4

#define ONEWIRE_PIN QM_PIN_ID_8 // 7
#define DHT11_PIN QM_PIN_ID_9 // 8

typedef enum {
	REQUEST_CONVERSION,
	GET_MEASUREMENT
} state_t;

qm_gpio_port_config_t gpio_config;
DS18B20_meas_t DS18B20_meas;
BMP280_meas_t BMP280_meas;
DHT11_meas_t DHT11_meas;
state_t state = REQUEST_CONVERSION;

static void pin_setup(void)
{
	qm_pmux_select(D7_PIN, QM_PMUX_FN_0);
	qm_pmux_select(D6_PIN, QM_PMUX_FN_0);
	qm_pmux_select(D5_PIN, QM_PMUX_FN_0);
	qm_pmux_select(D4_PIN, QM_PMUX_FN_0);
	qm_pmux_select(RS_PIN, QM_PMUX_FN_0);
	qm_pmux_select(E_PIN, QM_PMUX_FN_0);

	qm_pmux_select(ONEWIRE_PIN, QM_PMUX_FN_0);
	qm_pmux_select(DHT11_PIN, QM_PMUX_FN_0);

	gpio_config.direction = (1 << D7_PIN) | (1 << D6_PIN) | (1 << D5_PIN) | (1 << D4_PIN) | (1 << RS_PIN) | (1 << E_PIN);
	qm_gpio_set_config(QM_GPIO_0, &gpio_config);
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

	onewire_config_t onewire_config = {
			.gpio_config = &gpio_config,
			.onewire_pin = ONEWIRE_PIN
	};
	onewire_init(&onewire_config);

	DHT11_config_t DHT11_config = {
			.gpio_config = &gpio_config,
			.DHT11_pin = DHT11_PIN
	};
	DHT11_init(&DHT11_config);

	I2C_init(BMP280_ADDR_LOW);

	BMP280_config_t BMP280_config = {
			.config_flags = BMP280_STBY_TIME_1S | BMP280_FILTER_COEFF_2,
			.control_flags = BMP280_MODE_NORMAL | BMP280_PRES_OVERSAMPLING_X1 | BMP280_TEMP_OVERSAMPLING_X1
	};
	BMP280_init(&BMP280_config);

	GUI_init(&BMP280_meas, &DS18B20_meas, &DHT11_meas);

	while(1) {
		switch(state) {
			case REQUEST_CONVERSION:
				DS18B20_request_conversion();
				state = GET_MEASUREMENT;
				break;
			case GET_MEASUREMENT:
				DS18B20_meas = DS18B20_get_temperature();
				state = REQUEST_CONVERSION;
				break;
			default:
				break;
		}

		BMP280_meas = BMP280_get_measurement();
		DHT11_meas = DHT11_get_measurement();

		GUI_update();

		clk_sys_udelay(500 * 1000); // Wait for 500ms
	}

	return 0;
}

