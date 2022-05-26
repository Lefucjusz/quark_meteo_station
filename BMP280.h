/*
 * BMP280.h
 *
 *  Created on: 18.05.2022
 *      Author: Lefucjusz
 */

#ifndef BMP280_H_
#define BMP280_H_

#include <stdint.h>

#define BMP280_ID 0x58
#define BMP280_ADDR_LOW 0b1110110  // Chosen by the SDO pin state
#define BMP280_ADDR_HIGH 0b1110111
#define BMP280_CONFIG_SIZE 2 // 2 configuration registers
#define BMP280_CAL_DATA_SIZE 24 // 24 calibration registers

#define BMP280_CAL0_REG 0x88
#define BMP280_CAL1_REG 0x89
#define BMP280_CAL2_REG 0x8A
#define BMP280_CAL3_REG 0x8B
#define BMP280_CAL4_REG 0x8C
#define BMP280_CAL5_REG 0x8D
#define BMP280_CAL6_REG 0x8E
#define BMP280_CAL7_REG 0x8F
#define BMP280_CAL8_REG 0x90
#define BMP280_CAL9_REG 0x91
#define BMP280_CAL10_REG 0x92
#define BMP280_CAL11_REG 0x93
#define BMP280_CAL12_REG 0x94
#define BMP280_CAL13_REG 0x95
#define BMP280_CAL14_REG 0x96
#define BMP280_CAL15_REG 0x97
#define BMP280_CAL16_REG 0x98
#define BMP280_CAL17_REG 0x99
#define BMP280_CAL18_REG 0x9A
#define BMP280_CAL19_REG 0x9B
#define BMP280_CAL20_REG 0x9C
#define BMP280_CAL21_REG 0x9D
#define BMP280_CAL22_REG 0x9E
#define BMP280_CAL23_REG 0x9F

#define BMP280_ID_REG 0xD0
#define BMP280_RESET_REG 0xE0
#define BMP280_STATUS_REG 0xF3
#define BMP280_CTRL_MEAS_REG 0xF4
#define BMP280_CONFIG_REG 0xF5
#define BMP280_PRESS_MSB_REG 0xF7
#define BMP280_PRESS_LSB_REG 0xF8
#define BMP280_PRESS_XLSB_REG 0xF9
#define BMP280_TEMP_MSB_REG 0xFA
#define BMP280_TEMP_LSB_REG 0xFB
#define BMP280_TEMP_XLSB_REG 0xFC

typedef enum {
	BMP280_PRESENT,
	BMP280_ABSENT
} BMP280_detect_t;

typedef enum {
	BMP280_MODE_FORCED = 0x01,
	BMP280_MODE_NORMAL = 0x03,
	BMP280_PRES_OVERSAMPLING_X1 = 0x04,
	BMP280_PRES_OVERSAMPLING_X2 = 0x08,
	BMP280_PRES_OVERSAMPLING_X4 = 0x0C,
	BMP280_PRES_OVERSAMPLING_X8 = 0x10,
	BMP280_PRES_OVERSAMPLING_X16 = 0x14,
	BMP280_TEMP_OVERSAMPLING_X1 = 0x20,
	BMP280_TEMP_OVERSAMPLING_X2 = 0x40,
	BMP280_TEMP_OVERSAMPLING_X4 = 0x60,
	BMP280_TEMP_OVERSAMPLING_X8 = 0x80,
	BMP280_TEMP_OVERSAMPLING_X16 = 0xA0
} BMP280_control_flags_t;

typedef enum {
	BMP280_SPI_3_WIRE_ENABLE = 0x01,
	BMP280_FILTER_COEFF_2 = 0x04,
	BMP280_FILTER_COEFF_4 = 0x08,
	BMP280_FILTER_COEFF_8 = 0x0C,
	BMP280_FILTER_COEFF_16 = 0x10,
	BMP280_STBY_TIME_HALF_MS = 0x00,
	BMP280_STBY_TIME_62MS = 0x20,
	BMP280_STBY_TIME_125MS = 0x40,
	BMP280_STBY_TIME_250MS = 0x60,
	BMP280_STBY_TIME_500MS = 0x80,
	BMP280_STBY_TIME_1S = 0xA0,
	BMP280_STBY_TIME_2S = 0xC0,
	BMP280_STBY_TIME_4S = 0xE0
} BMP280_config_flags_t;

typedef union {
	uint8_t raw_data[BMP280_CONFIG_SIZE];
	struct {
		uint8_t control_flags;
		uint8_t config_flags;
	};
} BMP280_config_t;

typedef struct
{
	uint32_t pressure;
	int32_t temperature;
} BMP280_meas_t;

/**
 * @brief Initializes BMP280 sensor library
 *
 * @param config Pointer to struct with proper configuration set
 */
void BMP280_init(const BMP280_config_t* const config);

/**
 * @brief Checks whether BMP280 sensor is connected and responding
 *
 * @return BMP280_PRESENT if sensor detected, BMP280_ABSENT if sensor not detected
 */
BMP280_detect_t BMP280_detect(void);

/**
 * @brief Obtains measurement from BMP280 sensor
 *
 * @return Struct with complete measurement
 */
BMP280_meas_t BMP280_get_measurement(void);

#endif /* BMP280_H_ */
