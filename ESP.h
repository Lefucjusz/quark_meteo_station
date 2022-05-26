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

#define ESP_CONNECTION_TIME 10 // s

/**
 * @brief Initializes ESP module library
 *
 * @param BMP280_measurement Pointer to struct storing BMP280 sensor measurement
 * @param DS18B20_measurement Pointer to struct storing DS18B20 sensor measurement
 * @param DHT11_measurement Pointer to struct storing DHT11 sensor measurement
 */
void ESP_init(const BMP280_meas_t* const BMP280_measurement,
		const DS18B20_meas_t* const DS18B20_measurement,
		const DHT11_meas_t* const DHT11_measurement);

/**
 * @brief Requests ESP TCP connection to measurements server
 */
void ESP_connect_to_server(void);

/**
 * @brief Requests ESP to send current measuement values to measurements server
 */
void ESP_send_measurements(void);

#endif /* ESP_H_ */
