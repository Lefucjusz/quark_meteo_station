/*
 * ESP.c
 *
 *  Created on: 21.05.2022
 *      Author: Lefucjusz
 *
 * This library implements absolutely basic way of sending TCP request with ESP with
 * AT firmware. The communication is performed in the open loop, i.e. responses from
 * ESP module are not checked (to be honest, the Tx from ESP isn't even connected
 * to Intel Quark to simplify the hardware :P). This means that if for some reason
 * transmission has failed, the program has no way of knowing it - the data just
 * disappear into nothingness.
 *
 * In the original project (https://github.com/Lefucjusz/MCS48_meteo_station) this
 * technique seemed to perform surprisingly well, failure rate was around a few
 * measurements per week. The measurement is sent to the server every hour,
 * so it gives 24*7 = 168 measurements per week, of which, let's assume, 5 failed.
 * It gives failure rate around 5/168 =~ 3%. For this project I consider it to be
 * perfectly enough, but if someone needs better reliability, feel free to
 * extend this library!
 *
 * The WiFi's SSID and password are programmed into ESP's flash, so that I don't
 * have to remember to erase them from code before each push to repo :P I used
 * another program for this task, it's just one AT command.
 *
 */

#include "ESP.h"
#include "utoa.h"
#include <string.h>
#include "clk.h"
#include "qm_uart.h"

static const BMP280_meas_t* BMP280_meas;
static const DS18B20_meas_t* DS18B20_meas;
static const DHT11_meas_t* DHT11_meas;

static const char* const device_name = "Quark";
static const char* const host_address = "lowd.herokuapp.com";
static const char* const host_port = "80";
static const char* const api_path = "/api/sensors/full";

#define ESP_MAX_POST_SIZE 200
#define ESP_MAX_TEMP_SIZE 100
#define ESP_MAX_UTOA_SIZE 8 // 8 chars will be enough, the longest value is pressure (max 6 digits) + null-terminator + 1 just in case ;)

static char post_buffer[ESP_MAX_POST_SIZE]; // Buffer for POST request
static char temp_buffer[ESP_MAX_TEMP_SIZE]; // Temporary buffer for AT commands and prepared JSON
static char utoa_buffer[ESP_MAX_UTOA_SIZE]; // Buffer used for number-to-string conversions

static void ESP_write_string(const char* string) {
	uint8_t length = strlen(string);
	qm_uart_write_buffer(QM_UART_0, (const uint8_t* const)string, length);
}

static void ESP_prepare_json(void) {
	/* Prepare JSON in string */
	strcpy(temp_buffer, "{\"name\":\"");
	strcat(temp_buffer, device_name);

	strcat(temp_buffer, "\",\"Ti\":\"");
	utoa(BMP280_meas->temperature, utoa_buffer);
	strcat(temp_buffer, utoa_buffer);

	strcat(temp_buffer, "\",\"P\":\"");
	utoa(BMP280_meas->pressure, utoa_buffer);
	strcat(temp_buffer, utoa_buffer);

	strcat(temp_buffer, "\",\"To\":\"");
	if(DS18B20_meas->sign == DS18B20_NEGATIVE) {
		strcat(temp_buffer, "-");
	}
	/* Send as one value, representing temperature * 100 */
	uint16_t temperature = DS18B20_meas->integer * 100 + DS18B20_meas->fraction;
	utoa(temperature, utoa_buffer);
	strcat(temp_buffer, utoa_buffer);

	strcat(temp_buffer, "\",\"RH\":\"");
	utoa(DHT11_meas->humidity, utoa_buffer);
	strcat(temp_buffer, utoa_buffer);
	strcat(temp_buffer, "\"}");
}

static void ESP_prepare_post_request(void) {
	/* Prepare JSON */
	ESP_prepare_json();

	/* Prepare request string */
	strcpy(post_buffer, "POST ");
	strcat(post_buffer, api_path);
	strcat(post_buffer, " HTTP/1.1\r\nHost:");
	strcat(post_buffer, host_address);
	strcat(post_buffer, "\r\nContent-Length:");

	uint8_t payload_length = strlen(temp_buffer);
	utoa(payload_length, utoa_buffer);
	strcat(post_buffer, utoa_buffer);
	strcat(post_buffer, "\r\nContent-Type:application/json\r\n\r\n");

	/* Append payload */
	strcat(post_buffer, temp_buffer);
	strcat(post_buffer, "\r\n");
}

static void ESP_write_tcp_start_AT(void) {
	/* Prepare AT string */
	strcpy(temp_buffer, "AT+CIPSTART=\"TCP\",\"");
	strcat(temp_buffer, host_address);
	strcat(temp_buffer, "\",");
	strcat(temp_buffer, host_port);
	strcat(temp_buffer, "\r\n");

	/* Send request via UART to ESP */
	ESP_write_string(temp_buffer);
}

static void ESP_write_tcp_send_AT(void) {
	/* Prepare request string */
	strcpy(temp_buffer, "AT+CIPSEND=");
	uint16_t post_length = strlen(post_buffer);
	utoa(post_length, utoa_buffer);
	strcat(temp_buffer, utoa_buffer);
	strcat(temp_buffer, "\r\n");

	/* Send request via UART to ESP */
	ESP_write_string(temp_buffer);
}

void ESP_init(const BMP280_meas_t* const BMP280_measurement,
		const DS18B20_meas_t* const DS18B20_measurement,
		const DHT11_meas_t* const DHT11_measurement) {
	BMP280_meas = BMP280_measurement;
	DS18B20_meas = DS18B20_measurement;
	DHT11_meas = DHT11_measurement;
}

void ESP_send_measurements(void) {
	/* Request connection to server and wait 10 seconds for Heroku app to wake up */
	ESP_write_tcp_start_AT();
	clk_sys_udelay(10 * 1000 * 1000);

	/* Prepare POST request */
	ESP_prepare_post_request();

	/* Request TCP data transmission from ESP and wait 100ms for ESP to be ready */
	ESP_write_tcp_send_AT();
	clk_sys_udelay(100 * 1000);

	/* Send POST request */
	ESP_write_string(post_buffer);
}
