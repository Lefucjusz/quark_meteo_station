/*
 * onewire.c
 *
 *  Created on: 20.05.2022
 *      Author: Lefucjusz
 */

#include "onewire.h"
#include "clk.h"
#include "qm_gpio.h"

static onewire_config_t* onewire_cfg;

typedef enum {
	ONEWIRE_OUTPUT,
	ONEWIRE_INPUT
} onewire_pin_dir_t;

static void onewire_pin_set_direction(onewire_pin_dir_t direction) {
	/* Set or clear proper bit in GPIO config */
	if(direction == ONEWIRE_OUTPUT) {
		onewire_cfg->gpio_config->direction |= (1 << onewire_cfg->onewire_pin);
	} else {
		onewire_cfg->gpio_config->direction &= ~(1 << onewire_cfg->onewire_pin);
	}

	/* Apply modified config */
	qm_gpio_set_config(QM_GPIO_0, onewire_cfg->gpio_config);
}

void onewire_init(onewire_config_t* const onewire_config) {
	onewire_cfg = onewire_config;
	onewire_pin_set_direction(ONEWIRE_INPUT);
	qm_gpio_clear_pin(QM_GPIO_0, onewire_cfg->onewire_pin); //TODO describe the trick with constant low and changing direction
}

onewire_detect_t onewire_reset(void) {
	/* Drive 1-Wire pin low for 500us - reset condition */
	onewire_pin_set_direction(ONEWIRE_OUTPUT);
	clk_sys_udelay(500);

	/* Release pin and wait 100us for device response */
	onewire_pin_set_direction(ONEWIRE_INPUT);
	clk_sys_udelay(100);

	/* Read 1-Wire line state */
	qm_gpio_state_t response;
	qm_gpio_read_pin(QM_GPIO_0, onewire_cfg->onewire_pin, &response);

	/* Wait for 400us to complete the time slot */
	clk_sys_udelay(400);

	return response == QM_GPIO_LOW ? ONEWIRE_PRESENT : ONEWIRE_ABSENT;
}

void onewire_write_byte(uint8_t byte) {
	for(uint8_t i = 0; i < 8; i++) {
		/* Drive 1-Wire pin low */
		onewire_pin_set_direction(ONEWIRE_OUTPUT);

		/* Release pin with proper timing to send "1" or "0" */
		if(byte & 0x01) {
			clk_sys_udelay(5);
			onewire_pin_set_direction(ONEWIRE_INPUT);
			clk_sys_udelay(55);
		} else {
			clk_sys_udelay(60);
			onewire_pin_set_direction(ONEWIRE_INPUT);
		}

		byte >>= 1;
	}
}

uint8_t onewire_read_byte(void) {
	uint8_t byte = 0;
	for(uint8_t i = 0; i < 8; i++) {
		byte >>= 1;

		/* Drive 1-Wire pin low for 5us */
		onewire_pin_set_direction(ONEWIRE_OUTPUT);
		clk_sys_udelay(5);

		/* Release 1-Wire pin and wait for 5us for device response */
		onewire_pin_set_direction(ONEWIRE_INPUT);
		clk_sys_udelay(5);

		/* Read 1-Wire line state */
		qm_gpio_state_t response;
		qm_gpio_read_pin(QM_GPIO_0, onewire_cfg->onewire_pin, &response);

		/* If line high, set bit in result */
		if(response == QM_GPIO_HIGH) {
			byte |= 0x80;
		}

		/* Wait for 50us to complete the time slot */
		clk_sys_udelay(50);
	}
	return byte;
}


