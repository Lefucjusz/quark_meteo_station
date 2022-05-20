/*
 * DS18B20.h
 *
 *  Created on: 20.05.2022
 *      Author: Lefucjusz
 */

#ifndef DS18B20_H_
#define DS18B20_H_

#include "onewire.h"

typedef enum {
	DS18B20_CONVERT_T = 0x44,
	DS18B20_READ_SCRATCHPAD = 0xBE,
	DS18B20_SKIP_ROM = 0xCC
} DS18B20_command_t;

typedef enum {
	DS18B20_POSITIVE,
	DS18B20_NEGATIVE
} DS18B20_sign_t;

typedef struct {
	DS18B20_sign_t sign; // The sign has to be stored independently
	uint8_t integer; // Storing it in integer will fail for case when temperature is from (-1; 0) interval, as integer is zero then
	uint8_t fraction;
} DS18B20_meas_t;

void DS18B20_request_conversion(void);
DS18B20_meas_t DS18B20_get_temperature(void);

#endif /* DS18B20_H_ */
