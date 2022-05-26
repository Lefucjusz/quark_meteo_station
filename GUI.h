/*
 * GUI.h
 *
 *  Created on: 21.05.2022
 *      Author: Lefucjusz
 */

#ifndef GUI_H_
#define GUI_H_

#include "BMP280.h"
#include "DS18B20.h"
#include "DHT11.h"

/**
 * @brief Initializes Graphic User Interface
 *
 * @param BMP280_measurement Pointer to struct storing BMP280 sensor measurement
 * @param DS18B20_measurement Pointer to struct storing DS18B20 sensor measurement
 * @param DHT11_measurement Pointer to struct storing DHT11 sensor measurement
 */
void GUI_init(const BMP280_meas_t* const BMP280_measurement,
		const DS18B20_meas_t* const DS18B20_measurement,
		const DHT11_meas_t* const DHT11_measurement);

/**
 * @brief Refreshes GUI with new measurements
 */
void GUI_update(void);

#endif /* GUI_H_ */
