/*
 * DHT11.c
 *
 *  Created on: 21.05.2022
 *      Author: Lefucjusz
 */

#include "DHT11.h"
#include "clk.h"

#define DHT11_FRAME_LENGTH 40 // bits
#define DHT11_SYNCHRO_LOOP_TIMEOUT 200

/* Just an arbitrary special values that normally can't exist as a readout result */
#define DHT11_NO_SENSOR_ERROR 0xFD
#define DHT11_CHECKSUM_ERROR 0xFE

static DHT11_config_t* DHT11_config;
static uint64_t data_frame;

typedef enum {
	DHT11_OUTPUT,
	DHT11_INPUT
} DHT11_pin_dir_t;

typedef enum {
	DHT11_CHECKSUM_INVALID,
	DHT11_CHECKSUM_VALID
} DHT11_checksum_t;

static void DHT11_pin_set_direction(DHT11_pin_dir_t direction) {
	/* Set or clear proper bit in GPIO config */
	if(direction == DHT11_OUTPUT) {
		DHT11_config->gpio_config->direction |= (1 << DHT11_config->DHT11_pin);
	} else {
		DHT11_config->gpio_config->direction &= ~(1 << DHT11_config->DHT11_pin);
	}

	/* Apply modified config */
	qm_gpio_set_config(QM_GPIO_0, DHT11_config->gpio_config);
}

static DHT11_detect_t DHT11_reset(void) {
	/* Drive DHT11 pin low for 20ms */
	DHT11_pin_set_direction(DHT11_OUTPUT);
	clk_sys_udelay(20 * 1000);

	/* Release pin and wait 40us for DHT11 response */
	DHT11_pin_set_direction(DHT11_INPUT);
	clk_sys_udelay(40);

	/* Read DHT11 line state */
	qm_gpio_state_t response;
	qm_gpio_read_pin(QM_GPIO_0, DHT11_config->DHT11_pin, &response);

	/* Wait for 160us to complete the time slot */
	clk_sys_udelay(160);

	return response == QM_GPIO_LOW ? DHT11_PRESENT : DHT11_ABSENT;
}

static DHT11_detect_t DHT11_read_data_frame(void) {
	if(DHT11_reset() == DHT11_ABSENT) {
		return DHT11_ABSENT; // Sensor not detected, stop the readout
	}

	uint8_t timeout;
	qm_gpio_state_t response;

	/* Retrieve data */
	for(uint8_t i = 0; i < DHT11_FRAME_LENGTH; i++) {
		data_frame <<= 1;

		/* Wait for 75us and read DHT11 line state */
		clk_sys_udelay(75);
		qm_gpio_read_pin(QM_GPIO_0, DHT11_config->DHT11_pin, &response);

		/* If line high, set bit in result */
		if(response == QM_GPIO_HIGH) {
			data_frame |= 0x01;
			/* This is very clever idea, enabling timings self-correction on every "1" bit transmitted
			 * Loop waits for the start of transmission signal from DHT11, so that next bit will be
			 * received perfectly on time. Timeout prevents from getting stuck in this loop forever
			 * in case of some terrible transmission failure.
			 */
			timeout = DHT11_SYNCHRO_LOOP_TIMEOUT;
			/* The condition is true on entry, as response is QM_GPIO_HIGH indeed - it was checked in the if above */
			while((response == QM_GPIO_HIGH) && timeout) {
				qm_gpio_read_pin(QM_GPIO_0, DHT11_config->DHT11_pin, &response);
				timeout--;
			}
		}
	}

	return DHT11_PRESENT;
}

static DHT11_checksum_t DHT11_validate_checksum(void) { // Algorithm from DHT11 datasheet
	uint8_t a, b, c, d, e;
	a = (data_frame >> 32) & 0xFF;
	b = (data_frame >> 24) & 0xFF;
	c = (data_frame >> 16) & 0xFF;
	d = (data_frame >> 8) & 0xFF;
	e = data_frame & 0xFF;

	return (((a + b + c + d) & 0xFF) == e) ? DHT11_CHECKSUM_VALID : DHT11_CHECKSUM_INVALID;
}


void DHT11_init(DHT11_config_t* const config) {
	DHT11_config = config;
	DHT11_pin_set_direction(DHT11_INPUT);

	/* This library uses the same trick as 1-Wire library.
	 * See onewire.c -> onewire_init() comment for details
	 */
	qm_gpio_clear_pin(QM_GPIO_0, DHT11_config->DHT11_pin);
}

DHT11_meas_t DHT11_get_measurement(void) {
	DHT11_meas_t measurement;

	/* Error - sensor not detected */
	if(DHT11_read_data_frame() == DHT11_ABSENT) {
		measurement.humidity = DHT11_NO_SENSOR_ERROR;
		measurement.temperature = DHT11_NO_SENSOR_ERROR;
		return measurement;
	}

	/* Error - invalid checksum */
	if(DHT11_validate_checksum() == DHT11_CHECKSUM_INVALID) {
		measurement.humidity = DHT11_CHECKSUM_ERROR;
		measurement.temperature = DHT11_CHECKSUM_ERROR;
		return measurement;
	}

	/* Successful readout */
	measurement.humidity = (data_frame >> 32) & 0xFF;
	measurement.temperature = (data_frame >> 16) & 0xFF;

	return measurement;
}
