/*
 * DHT11.h
 *
 *  Created on: 21.05.2022
 *      Author: Lefucjusz
 */

#ifndef DHT11_H_
#define DHT11_H_

#include <stdint.h>
#include "qm_gpio.h"

typedef struct {
	qm_gpio_port_config_t* const gpio_config;
	uint8_t DHT11_pin;
} DHT11_config_t;

typedef enum {
	DHT11_PRESENT,
	DHT11_ABSENT
} DHT11_detect_t;

typedef struct {
	uint8_t humidity;
	uint8_t temperature;
} DHT11_meas_t;

/**
 * @brief Initializes DHT11 sensor library
 *
 * @param config Pointer to struct with proper configuration set
 */
void DHT11_init(DHT11_config_t* const DHT11_config);

/**
 * @brief Obtains measurement from DHT11 sensor
 *
 * @return Struct with complete measurement
 */
DHT11_meas_t DHT11_get_measurement(void);

#endif /* DHT11_H_ */
