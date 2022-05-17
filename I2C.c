/*
 * I2C.c
 *
 *  Created on: 18.05.2022
 *      Author: Lefucjusz
 */

#include "I2C.h"
#include "clk.h"
#include "qm_i2c.h"
#include "qm_pinmux.h"
#include "qm_pin_functions.h"

static I2C_error_t error = STATUS_UNINITIALIZED;
static uint8_t slave_addr;

void I2C_init(uint8_t dev_addr) {
	slave_addr = dev_addr;

	/* Enable I2C 0 */
	clk_periph_enable(CLK_PERIPH_CLK | CLK_PERIPH_I2C_M0_REGISTER);

	/* Set pin mode as I2C */
	qm_pmux_select(QM_PIN_ID_6, QM_PIN_6_FN_I2C0_SCL);
	qm_pmux_select(QM_PIN_ID_7, QM_PIN_7_FN_I2C0_SDA);

	/* Configure I2C peripheral */
	qm_i2c_config_t i2c_config;

	i2c_config.address_mode = QM_I2C_7_BIT;
	i2c_config.mode = QM_I2C_MASTER;
	i2c_config.speed = QM_I2C_SPEED_STD; //TODO check with fast ^^

	if(qm_i2c_set_config(QM_I2C_0, &i2c_config)) {
		error = STATUS_INIT_FAILED;
	}

	error = STATUS_OK;
}

void I2C_write(uint8_t* data, uint8_t size, uint8_t reg_addr) {
	qm_i2c_status_t status;

	if(qm_i2c_master_write(QM_I2C_0, slave_addr, data, size, true, &status)) {
		error = STATUS_WRITE_FAILED;
		return;
	}

	error = STATUS_OK;
}

void I2C_write_byte(uint8_t byte, uint8_t reg_addr) {
	I2C_write(&byte, 1, reg_addr);
}

void I2C_read(uint8_t* buf, uint8_t size, uint8_t reg_addr) {
	qm_i2c_status_t status;

	/* Write read register address */
	if(qm_i2c_master_write(QM_I2C_0, slave_addr, &reg_addr, 1, false, &status)) {
		error = STATUS_WRITE_FAILED;
		return;
	}

	/* Read data from selected address */
	if(qm_i2c_master_read(QM_I2C_0, slave_addr, buf, size, true, &status)) {
		error = STATUS_READ_FAILED;
		return;
	}

	error = STATUS_OK;
}

uint8_t I2C_read_byte(uint8_t reg_addr) {
	uint8_t byte;
	I2C_read(&byte, 1, reg_addr);
	return byte;
}

I2C_error_t I2C_error(void) {
	return error;
}
