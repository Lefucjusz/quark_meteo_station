/*
 * BMP280.c
 *
 *  Created on: 18.05.2022
 *      Author: Lefucjusz
 */

#include "BMP280.h"
#include "I2C.h"

#define BMP280_MEAS_REG_SIZE 3 // Each measurement register consists of 3 bytes
#define BMP280_MEAS_DATA_SIZE (2 * BMP280_MEAS_REG_SIZE) // Pressure and temperature registers, BMP280_MEAS_REGS_SIZE bytes each

typedef union
{
	uint8_t raw_data[BMP280_CAL_DATA_SIZE];
	struct
	{
		uint16_t T1;
		int16_t T2;
		int16_t T3;

		uint16_t P1;
		int16_t P2;
		int16_t P3;
		int16_t P4;
		int16_t P5;
		int16_t P6;
		int16_t P7;
		int16_t P8;
		int16_t P9;
	};
} BMP280_cal_t;

static BMP280_cal_t cal;
static BMP280_meas_t meas;

static void BMP280_read_calibration_regs(void) {
	/* Read all calibration registers at once */
	I2C_read(cal.raw_data, BMP280_CAL_DATA_SIZE, BMP280_CAL0_REG);
}

static void BMP280_read_measurement_regs(void) {
	uint8_t buffer[BMP280_MEAS_DATA_SIZE];

	/* Read raw measurements */
	I2C_read(buffer, BMP280_MEAS_DATA_SIZE, BMP280_PRESS_MSB_REG);

	/* Merge obtained values into 24-bit variables */
	meas.pressure = (buffer[0] << 12) | (buffer[1] << 4) | (buffer[2] >> 4);
	meas.temperature = (buffer[3] << 12) | (buffer[4] << 4) | (buffer[5] >> 4);
}

static void BMP280_compute_compensation(void) {
	int32_t var1, var2, t_fine;
	int32_t pres_signed = (int32_t)meas.pressure;

	/* Compute t_fine and temperature */
	var1 = ((((meas.temperature >> 3) - ((int32_t)cal.T1 << 1))) * ((int32_t)cal.T2)) >> 11;
	var2 = (((((meas.temperature >> 4) - ((int32_t)cal.T1)) * ((meas.temperature >> 4) - ((int32_t)cal.T1))) >> 12) * ((int32_t)cal.T3)) >> 14;
	t_fine = var1 + var2;
	meas.temperature = (t_fine * 5 + 128) >> 8;

	/* Compute pressure */
	var1 = (((int32_t)t_fine) >> 1) - (int32_t)64000;
	var2 = (((var1 >> 2) * (var1 >> 2)) >> 11) * ((int32_t)cal.P6);
	var2 = var2 + ((var1 * ((int32_t)cal.P5)) << 1);
	var2 = (var2 >> 2) + (((int32_t)cal.P4) << 16);
	var1 = (((cal.P3 * (((var1 << 2) * (var1 >> 2)) >> 13)) >> 3) + ((((int32_t)cal.P2) * var1) >> 1)) >> 18;
	var1 = ((((32768 + var1)) * ((int32_t)cal.P1)) >> 15);
	if(var1 == 0) {
		meas.pressure = 0;
		return;
	}
	meas.pressure = (((uint32_t)(((int32_t)1048576) - pres_signed) - (var2 >> 12))) * 3125;
	if(meas.pressure < 0x80000000) {
		meas.pressure = (meas.pressure << 1) / ((uint32_t)var1);
	} else {
		meas.pressure = (meas.pressure / (uint32_t)var1) >> 1;
	}
	var1 = (((int32_t)cal.P9) * ((int32_t)(((meas.pressure >> 3) * (meas.pressure >> 3)) >> 13))) >> 12;
	var2 = (((int32_t)(meas.pressure >> 2)) * ((int32_t)cal.P8)) >> 13;
	meas.pressure = (uint32_t)((int32_t)meas.pressure + ((var1 + var2 + cal.P7) >> 4));
}

void BMP280_init(BMP280_config_t* config) {
	/* Configure BMP280 */
	I2C_write(config->raw_data, BMP280_CONFIG_SIZE, BMP280_CTRL_MEAS_REG);

	/* Read calibration data */
	BMP280_read_calibration_regs();
}

BMP280_detect_t BMP280_detect(void) {
	if(I2C_read_byte(BMP280_ID_REG) == BMP280_ID) {
		return BMP280_PRESENT;
	}

	return BMP280_ABSENT;
}

BMP280_meas_t BMP280_get_measurement(void) {
	BMP280_read_measurement_regs();
	BMP280_compute_compensation();
	return meas;
}
