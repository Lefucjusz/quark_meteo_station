/*
 * DS18B20.c
 *
 *  Created on: 20.05.2022
 *      Author: Lefucjusz
 */

#include "DS18B20.h"

void DS18B20_request_conversion(void) {
	onewire_reset();
	onewire_write_byte(DS18B20_SKIP_ROM);
	onewire_write_byte(DS18B20_CONVERT_T);
}

DS18B20_meas_t DS18B20_get_temperature(void) {
	DS18B20_meas_t measurement;

	/* Request reading scratchpad */
	onewire_reset();
	onewire_write_byte(DS18B20_SKIP_ROM);
	onewire_write_byte(DS18B20_READ_SCRATCHPAD);

	/* Read temperature bytes and extract sign */
	uint8_t lsb = onewire_read_byte();
	uint8_t msb = onewire_read_byte();
	measurement.sign = msb & 0x80 ? DS18B20_NEGATIVE : DS18B20_POSITIVE;

	/* If temperature is negative, convert it to positive */
	if(measurement.sign == DS18B20_NEGATIVE) {
		msb = ~msb;
		lsb = ~lsb + 1; // Actually it's the same as lsb = -lsb
	}

	/* Clever float-avoiding conversion equations I came up with years ago */
	measurement.integer = (uint8_t)(((msb << 4) | (lsb >> 4)) & 0x7F);
	measurement.fraction = (uint8_t)(((lsb & 0x0F) * 625) / 100);

	return measurement;
}


