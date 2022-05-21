/*
 * timer.h
 *
 *  Created on: 21.05.2022
 *      Author: Lefucjusz
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <stdint.h>

void timer_init(void (*const timer_callback)(void), uint16_t period_ms);

#endif /* TIMER_H_ */
