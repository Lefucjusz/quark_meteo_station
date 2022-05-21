/*
 * main.c
 *
 *  Created on: 16.05.2022
 *      Author: Lefucjusz
 */

#include "clk.h"
#include "qm_gpio.h"
#include "qm_pinmux.h"
#include "qm_uart.h"
#include "HD44780.h"
#include "I2C.h"
#include "BMP280.h"
#include "onewire.h"
#include "DS18B20.h"
#include "DHT11.h"
#include "GUI.h"
#include "ESP.h"
#include "timer.h"

#define RS_PIN QM_PIN_ID_16 // 13
#define E_PIN QM_PIN_ID_18 	// 12
#define D4_PIN QM_PIN_ID_17 // 11
#define D5_PIN QM_PIN_ID_0 // 10
#define D6_PIN QM_PIN_ID_24 // 9
#define D7_PIN QM_PIN_ID_9 // 8

#define ONEWIRE_PIN QM_PIN_ID_14 // A2
#define DHT11_PIN QM_PIN_ID_15 // A3

#define TX_PIN QM_PIN_ID_12 // 1
#define RX_PIN QM_PIN_ID_13 // 0
#define UART_BAUD_RATE 9600

#define TIMER_PERIOD 1000 // ms
#define SERVER_UPDATE_PERIOD 3600 // s

typedef enum {
	REQUEST_CONVERSION,
	GET_MEASUREMENT
} DS18B20_readout_state_t;

typedef enum {
	IDLE,
	CONNECT_TO_SERVER,
	AWAIT_FOR_CONNECTION,
	SEND_DATA
} ESP_transmission_state_t;

typedef enum {
	UP_TO_DATE,
	UPDATE_REQUEST
} GUI_state_t;

static GUI_state_t update_state = UPDATE_REQUEST;
static ESP_transmission_state_t ESP_state = IDLE;
static qm_gpio_port_config_t gpio_config;

static void gpio_setup(void) {
	/* Set pin multiplexer to GPIO */
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

static void uart_setup(void) {
	qm_uart_config_t uart_config;

	/* Set pin multiplexer to UART */
	qm_pmux_select(TX_PIN, QM_PMUX_FN_2);
	qm_pmux_select(RX_PIN, QM_PMUX_FN_2);

	uart_config.hw_fc = false; // Turn off hardware automatic flow control
	uart_config.line_control = QM_UART_LC_8N1; // 8 bit frame, no parity bit, 1 stop bit

	/* Equations from "Intel® Quark™ microcontroller D2000 – Change the Baud-rate of UART for STDOUT" datasheet */
	uint16_t divider = 32000000 / UART_BAUD_RATE;
	uint8_t second_arg = divider / 16;
	uint8_t third_arg = divider - second_arg * 16;
	uart_config.baud_divisor = QM_UART_CFG_BAUD_DL_PACK(0, second_arg, third_arg);

	/* Apply UART configuration */
	qm_uart_set_config(QM_UART_0, &uart_config);

	/* Enable UART_A clocking */
	clk_periph_enable(CLK_PERIPH_CLK | CLK_PERIPH_UARTA_REGISTER);
}

static void timer_callback(void) {
	static uint16_t server_update_counter = 0;
	update_state = UPDATE_REQUEST;
	/* Do not count when ESP is sending data */
	if(ESP_state == IDLE) {
		server_update_counter++;
	}
	/* Request data send operation, take correction for time ESP needs to connect to server */
	if(server_update_counter >= (SERVER_UPDATE_PERIOD - ESP_CONNECTION_TIME)) {
		server_update_counter = 0;
		ESP_state = CONNECT_TO_SERVER;
	}
}

int main(void)
{
	DS18B20_meas_t DS18B20_meas;
	BMP280_meas_t BMP280_meas;
	DHT11_meas_t DHT11_meas;
	DS18B20_readout_state_t DS18B20_state = REQUEST_CONVERSION;

	/* Configure GPIO */
	gpio_setup();

	/* Configure UART */
	uart_setup();

	/* Configure HD44780 display */
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

	/* Configure 1-Wire bus */
	onewire_config_t onewire_config = {
			.gpio_config = &gpio_config,
			.onewire_pin = ONEWIRE_PIN
	};
	onewire_init(&onewire_config);

	/* Configure DHT11 sensor */
	DHT11_config_t DHT11_config = {
			.gpio_config = &gpio_config,
			.DHT11_pin = DHT11_PIN
	};
	DHT11_init(&DHT11_config);

	/* Configure I2C bus */
	I2C_init(BMP280_ADDR_LOW);

	/* Configure BMP280 sensor */
	BMP280_config_t BMP280_config = {
			.config_flags = BMP280_STBY_TIME_1S | BMP280_FILTER_COEFF_2,
			.control_flags = BMP280_MODE_NORMAL | BMP280_PRES_OVERSAMPLING_X1 | BMP280_TEMP_OVERSAMPLING_X1
	};
	BMP280_init(&BMP280_config);

	/* Configure GUI */
	GUI_init(&BMP280_meas, &DS18B20_meas, &DHT11_meas);

	/* Configure ESP */
	ESP_init(&BMP280_meas, &DS18B20_meas, &DHT11_meas);

	/* Timer init */
	timer_init(timer_callback, TIMER_PERIOD);

	/* Main loop */
	while(1) {
		if(update_state == UPDATE_REQUEST) {
			/* State machine for DS18B20 sensor readout */
			switch(DS18B20_state) {
				case REQUEST_CONVERSION:
					DS18B20_request_conversion();
					DS18B20_state = GET_MEASUREMENT;
					break;
				case GET_MEASUREMENT:
					DS18B20_meas = DS18B20_get_temperature();
					DS18B20_state = REQUEST_CONVERSION;
					break;
				default:
					break;
			}
			BMP280_meas = BMP280_get_measurement();
			DHT11_meas = DHT11_get_measurement();

			/* State machine for ESP data transmission */
			switch(ESP_state) {
				case IDLE:
					break;
				case CONNECT_TO_SERVER:
					/* Request connection to server and wait 10 seconds for Heroku app to wake up
					 * The waiting state is needed, because Heroku sleep apps and my ESP library
					 * works in open loop, i.e. I don't receive any data back from ESP to check
					 * whether it has woken up already.
					 */
					ESP_connect_to_server();
					ESP_state = AWAIT_FOR_CONNECTION;
					break;
				case AWAIT_FOR_CONNECTION: {
					static uint8_t counter = 0;
					counter++;
					/* State transition will take one second, so wait here one second less */
					if(counter >= ESP_CONNECTION_TIME - 1) {
						ESP_state = SEND_DATA;
						counter = 0;
					}
				} break;
				case SEND_DATA:
					ESP_send_measurements();
					ESP_state = IDLE;
					break;
				default:
					break;
			}

			/* Update GUI and finish update cycle */
			GUI_update();
			update_state = UP_TO_DATE;
		}
	}

	return 0;
}

