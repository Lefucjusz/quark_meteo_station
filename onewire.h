/*
 * onewire.h
 *
 *  Created on: 20.05.2022
 *      Author: Lefucjusz
 */

#ifndef ONEWIRE_H_
#define ONEWIRE_H_

#include <stdint.h>
#include "qm_gpio.h"

typedef struct {
	qm_gpio_port_config_t* const gpio_config;
	uint8_t onewire_pin;
} onewire_config_t;

typedef enum {
	ONEWIRE_PRESENT,
	ONEWIRE_ABSENT
} onewire_detect_t;

void onewire_init(onewire_config_t* const onewire_config);
onewire_detect_t onewire_reset(void);
void onewire_write_byte(uint8_t byte);
uint8_t onewire_read_byte(void);

#endif /* ONEWIRE_H_ */
