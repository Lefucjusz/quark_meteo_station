/*
 * GUI.c
 *
 *  Created on: 21.05.2022
 *      Author: Lefucjusz
 */

#include "GUI.h"
#include "HD44780.h"

static const BMP280_meas_t* BMP280_meas;
static const DS18B20_meas_t* DS18B20_meas;
static const DHT11_meas_t* DHT11_meas;

static void GUI_draw_layout(void) {
	HD44780_write_string("I: ");
	HD44780_gotoxy(2, 1);
	HD44780_write_string("P: ");
	HD44780_gotoxy(1, 12);
	HD44780_write_string("O: ");
	HD44780_gotoxy(2, 15);
	HD44780_write_string("H: ");
}

static void GUI_draw_BMP280(void) {
	HD44780_gotoxy(1, 4);
	HD44780_write_integer(BMP280_meas->temperature / 100, 0);
	HD44780_write_char('.');
	HD44780_write_integer(BMP280_meas->temperature % 100, 2);
	HD44780_write_string("C ");

	HD44780_gotoxy(2, 4);
	HD44780_write_integer(BMP280_meas->pressure / 100, 0);
	HD44780_write_char('.');
	HD44780_write_integer(BMP280_meas->pressure % 100, 2);
	HD44780_write_string("hPa ");
}

static void GUI_draw_DS18B20(void) {
	HD44780_gotoxy(1, 14);
	if(DS18B20_meas->sign == DS18B20_NEGATIVE) {
		HD44780_write_char('-');
	} else {
		HD44780_write_char(' ');
	}
	HD44780_write_integer(DS18B20_meas->integer, 0);
	HD44780_write_char('.');
	HD44780_write_integer(DS18B20_meas->fraction, 2);
	HD44780_write_string("C ");
}

static void GUI_draw_DHT11(void) {
	HD44780_gotoxy(2, 18);
	HD44780_write_integer(DHT11_meas->humidity, 0);
	HD44780_write_string("%");
}

void GUI_init(const BMP280_meas_t* const BMP280_measurement,
		const DS18B20_meas_t* const DS18B20_measurement,
		const DHT11_meas_t* const DHT11_measurement) {
	BMP280_meas = BMP280_measurement;
	DS18B20_meas = DS18B20_measurement;
	DHT11_meas = DHT11_measurement;

	/* Clear display and draw static elements of the view */
	HD44780_clear();
	GUI_draw_layout();
}

void GUI_update(void) {
	GUI_draw_BMP280();
	GUI_draw_DS18B20();
	GUI_draw_DHT11();
}

