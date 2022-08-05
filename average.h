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

/**
 * @brief Clears internal data structures used to store components of the average
 */
void average_reset(void);

/**
 * @brief Initializes average library
 *
 * @param BMP280_measurement Pointer to struct storing measurement obtained from BMP280 sensor
 * @param BMP280_average Pointer to struct to store BMP280 measurement average in
 * @param DS18B20_measurement Pointer to struct storing measurement obtained from DS18B20 sensor
 * @param DS18B20_average Pointer to struct to store DS18B20 measurement average in
 * @param DHT11_measurement Pointer to struct storing measurement obtained from DHT11 sensor
 * @param DHT11_average Pointer to struct to store DHT11 measurement average in
 */
void average_init(const BMP280_meas_t* const BMP280_measurement,
		BMP280_meas_t* const BMP280_average,
		const DS18B20_meas_t* const DS18B20_measurement,
		DS18B20_meas_t* const DS18B20_average,
		const DHT11_meas_t* const DHT11_measurement,
		DHT11_meas_t* const DHT11_average);

/**
 * @brief Updates BMP280 measurement average by taking data from struct pointed by
 * 		  BMP280_measurement, updating internal average data structure and storing
 * 		  current average values in structure pointed by BMP280_average
 */
void average_update_BMP280(void);

/**
 * @brief Updates DS18B20 measurement average by taking data from struct pointed by
 * 		  DS18B20_measurement, updating internal average data structure and storing
 * 		  current average values in structure pointed by DS18B20_average
 */
void average_update_DS18B20(void);

/**
 * @brief Updates DHT11 measurement average by taking data from struct pointed by
 * 		  DHT11_measurement, updating internal average data structure and storing
 * 		  current average values in structure pointed by DHT11_average
 */
void average_update_DHT11(void);

#endif /* AVERAGE_H_ */
