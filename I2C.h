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
	STATUS_UNINITIALIZED,
	STATUS_INIT_FAILED,
	STATUS_WRITE_FAILED,
	STATUS_READ_FAILED
} I2C_error_t;

/**
 * @brief Initializes I2C peripheral library
 *
 * @param dev_addr Address of the slave device
 */
void I2C_init(uint8_t dev_addr);

/**
 * @brief Writes data to I2C slave device
 *
 * @param data Pointer to buffer containing data to be written
 * @param size Size of the data to be written in bytes
 * @param reg_addr Address of the first register to write data to
 */
void I2C_write(const uint8_t* const data, uint8_t size, uint8_t reg_addr);

/**
 * @brief Writes byte to I2C slave device
 *
 * @param byte Byte to be written to device
 * @param reg_addr Address of the register to write the byte to
 */
void I2C_write_byte(uint8_t byte, uint8_t reg_addr);

/**
 * @brief Reads data from I2C slave device
 *
 * @param buf Pointer to buffer large enough to store read data
 * @param size Size of the data to be read in bytes
 * @param reg_addr Address of the first register to read data from
 */
void I2C_read(uint8_t* const buf, uint8_t size, uint8_t reg_addr);

/**
 * @brief Reads byte from I2C slave device
 *
 * @param reg_addr Address of the register to write the byte to
 *
 * @return Byte read from the device
 */
uint8_t I2C_read_byte(uint8_t reg_addr);

/**
 * @brief Used to check whether the transmission has been successful
 *
 * @return STATUS_OK if transmission succeeded, STATUS_*_FAILED in case of
 * 		   failure, exact value depends on the type of failure the library
 * 		   encountered
 */
I2C_error_t I2C_error(void);

#endif /* I2C_H_ */
