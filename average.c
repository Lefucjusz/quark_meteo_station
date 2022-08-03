/*
 * average.c
 *
 *  Created on: 02.08.2022
 *      Author: Lefucjusz
 */

#include "average.h"
#include <string.h>

typedef struct {
	int32_t temp_sum;
	uint32_t pres_sum;
	uint16_t meas_num;
} BMP280_average_t;

typedef struct {
	int32_t temp_sum;
	uint16_t meas_num;
} DS18B20_average_t;

typedef struct {
	int32_t temp_sum;
	uint32_t humid_sum;
	uint16_t meas_num;
} DHT11_average_t;

static const BMP280_meas_t* BMP280_meas;
static const DS18B20_meas_t* DS18B20_meas;
static const DHT11_meas_t* DHT11_meas;

static BMP280_meas_t* BMP280_meas_avg;
static DS18B20_meas_t* DS18B20_meas_avg;
static DHT11_meas_t* DHT11_meas_avg;

static BMP280_average_t BMP280_avg;
static DS18B20_average_t DS18B20_avg;
static DHT11_average_t DHT11_avg;

void average_reset(void) {
	/* Clear all average structs */
	memset(&BMP280_avg, 0, sizeof(BMP280_average_t));
	memset(&DS18B20_avg, 0, sizeof(DS18B20_average_t));
	memset(&DHT11_avg, 0, sizeof(DHT11_average_t));
}

void average_init(const BMP280_meas_t* const BMP280_measurement,
		BMP280_meas_t* const BMP280_average,
		const DS18B20_meas_t* const DS18B20_measurement,
		DS18B20_meas_t* const DS18B20_average,
		const DHT11_meas_t* const DHT11_measurement,
		DHT11_meas_t* const DHT11_average) {
	BMP280_meas = BMP280_measurement;
	BMP280_meas_avg = BMP280_average;

	DS18B20_meas = DS18B20_measurement;
	DS18B20_meas_avg = DS18B20_average;

	DHT11_meas = DHT11_measurement;
	DHT11_meas_avg = DHT11_average;

	/* Clear averages */
	average_reset();
}

void average_update_BMP280(void) {
	/* Update average */
	BMP280_avg.pres_sum += BMP280_meas->pressure;
	BMP280_avg.temp_sum += BMP280_meas->temperature;
	BMP280_avg.meas_num++;

	/* Update result */
	BMP280_meas_avg->pressure = BMP280_avg.pres_sum / BMP280_avg.meas_num;
	BMP280_meas_avg->temperature = BMP280_avg.temp_sum / BMP280_avg.meas_num;
}

void average_update_DS18B20(void) {
	/* Convert temperature to integer format scaled by 100 */
	int16_t temperature = DS18B20_meas->integer * 100 + DS18B20_meas->fraction;
	if(DS18B20_meas->sign == DS18B20_NEGATIVE) {
		temperature = -temperature;
	}

	/* Update average */
	DS18B20_avg.temp_sum += temperature;
	DS18B20_avg.meas_num++;

	/* Update result */
	int16_t average_temperature = DS18B20_avg.temp_sum / DS18B20_avg.meas_num;
	DS18B20_meas_avg->sign = average_temperature >= 0 ? DS18B20_POSITIVE : DS18B20_NEGATIVE;
	DS18B20_meas_avg->integer = average_temperature / 100;
	DS18B20_meas_avg->fraction = average_temperature % 100;
}

void average_update_DHT11(void) {
	/* If erroneous measurement, return without updating */
	if(DHT11_meas->temperature == DHT11_NO_SENSOR_ERROR || DHT11_meas->temperature == DHT11_CHECKSUM_ERROR) {
		return;
	}

	/* Update average */
	DHT11_avg.temp_sum += DHT11_meas->temperature;
	DHT11_avg.humid_sum += DHT11_meas->humidity;
	DHT11_avg.meas_num++;

	/* Update result */
	DHT11_meas_avg->temperature = DHT11_avg.temp_sum / DHT11_avg.meas_num;
	DHT11_meas_avg->humidity = DHT11_avg.humid_sum / DHT11_avg.meas_num;
}



