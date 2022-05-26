/*
 * timer.h
 *
 *  Created on: 21.05.2022
 *      Author: Lefucjusz
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <stdint.h>

/**
 * @brief Initializes the hardware timer
 *
 * @param timer_callback Pointer to function to be executed when timer overflows
 * @param period_ms Period of the timer (how often the timer_callback will be called)
 */
void timer_init(void (*const timer_callback)(void), uint16_t period_ms);

#endif /* TIMER_H_ */
