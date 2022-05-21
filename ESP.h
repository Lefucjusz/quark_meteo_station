/*
 * ESP.h
 *
 *  Created on: 21.05.2022
 *      Author: Lefucjusz
 */

#ifndef ESP_H_
#define ESP_H_

#include "BMP280.h"
#include "DS18B20.h"
#include "DHT11.h"

void ESP_init(const BMP280_meas_t* const BMP280_measurement,
		const DS18B20_meas_t* const DS18B20_measurement,
		const DHT11_meas_t* const DHT11_measurement);

void ESP_send_measurements(void);

#endif /* ESP_H_ */
