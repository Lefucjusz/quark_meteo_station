/*
 * utoa.c
 *
 *  Created on: 21.05.2022
 *      Author: Lefucjusz
 */

#include "utoa.h"
#include <stdint.h>
#include <string.h>

static char* strrev(char* string) {
	int i, j;
	unsigned length = strlen(string);
	char temp;

	for(i = 0, j = length - 1; i < j; i++, j--) {
		temp = string[i];
		string[i] = string[j];
		string[j] = temp;
	}

	return string;
}

/* This is NON-STANDARD utoa(), it works only with base 10 numbers!
 * (actually every utoa() is non-standard, but you get the point...)
 */
char* utoa(unsigned number, char* result) {
	uint8_t digit_count = 0;

	/* Convert digits to ASCII codes and place them in buffer */
	do {
		result[digit_count] = (number % 10) + '0';
		number /= 10;
		digit_count++;
	} while(number > 0);

	/* Terminate string */
	result[digit_count] = '\0';

	/* Number is now in reversed order, reverse it to get proper order */
	strrev(result);

	return result;
}
