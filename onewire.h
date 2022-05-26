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

/**
 * @brief Initializes 1-Wire library
 *
 * @param config Pointer to struct with proper configuration set
 */
void onewire_init(onewire_config_t* const config);

/**
 * @brief Performs 1-Wire bus reset
 *
 * @return ONEWIRE_PRESENT if any sensor responded, ONEWIRE_ABSENT if no sensor was detected
 */
onewire_detect_t onewire_reset(void);

/**
 * @brief Writes byte to 1-Wire device
 *
 * @param byte Byte to be written to the device
 */
void onewire_write_byte(uint8_t byte);

/**
 * @brief Reads byte from 1-Wire device
 *
 * @return Byte read from the device
 */
uint8_t onewire_read_byte(void);

#endif /* ONEWIRE_H_ */
