/*
 * I2C.h
 *
 *  Created on: 18.05.2022
 *      Author: Lefucjusz
 */

#ifndef I2C_H_
#define I2C_H_

#include <stdint.h>

typedef enum {
	STATUS_OK,
	STATUS_INIT_FAILED,
	STATUS_WRITE_FAILED,
	STATUS_READ_FAILED
} I2C_error_t;

void I2C_init(uint8_t dev_addr);

void I2C_write(uint8_t* data, uint8_t size, uint8_t reg_addr);

void I2C_write_byte(uint8_t byte, uint8_t reg_addr);

void I2C_read(uint8_t* buf, uint8_t size, uint8_t reg_addr);

uint8_t I2C_read_byte(uint8_t reg_addr);

I2C_error_t I2C_error(void);

#endif /* I2C_H_ */
