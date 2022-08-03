/*
 * average.h
 *
 *  Created on: 02.08.2022
 *      Author: Lefucjusz
 */

#ifndef AVERAGE_H_
#define AVERAGE_H_

#include <stdint.h>
#include "BMP280.h"
#include "DHT11.h"
#include "DS18B20.h"

void average_reset(void);

void average_init(const BMP280_meas_t* const BMP280_measurement,
		BMP280_meas_t* const BMP280_average,
		const DS18B20_meas_t* const DS18B20_measurement,
		DS18B20_meas_t* const DS18B20_average,
		const DHT11_meas_t* const DHT11_measurement,
		DHT11_meas_t* const DHT11_average);

void average_update_BMP280(void);

void average_update_DS18B20(void);

void average_update_DHT11(void);

#endif /* AVERAGE_H_ */
