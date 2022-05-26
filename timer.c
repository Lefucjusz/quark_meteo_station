/*
 * timer.c
 *
 *  Created on: 21.05.2022
 *      Author: Lefucjusz
 */

#ifndef TIMER_C_
#define TIMER_C_

#define QUARK_XTAL_FREQ 32000000ULL // 32MHz
#define TRIMMING_VALUE 275 // Clock cycles, used to calibrate the timer, has to be fine-tuned for each board

void (*callback)(void);

#include "timer.h"
#include "qm_pwm.h"
#include "clk.h"
#include "qm_interrupt.h"
#include "qm_interrupt_router.h"
#include "qm_isr.h"

static void callback_wrapper(void* data, uint32_t timer_int) {
	callback();
}

void timer_init(void (*const timer_callback)(void), uint16_t period_ms) {
	callback = timer_callback;

	/* Configure the timer */
	qm_pwm_config_t timer_config;
	timer_config.lo_count = ((QUARK_XTAL_FREQ * period_ms) / 1000) + TRIMMING_VALUE; // Timer duration in clock cycles
	timer_config.hi_count = 0;						// Unused in timer mode
	timer_config.mode = QM_PWM_MODE_TIMER_COUNT;	// Counter mode
	timer_config.mask_interrupt = false;			// Enable the interrupt
	timer_config.callback_data = NULL;				// No data passed to callback function
	timer_config.callback = callback_wrapper;		// Callback function

	/* Apply timer configuration */
	qm_pwm_set_config(QM_PWM_0, QM_PWM_ID_1, &timer_config);

	/* Enable PWM block clocking */
	clk_periph_enable(CLK_PERIPH_CLK | CLK_PERIPH_PWM_REGISTER);

	/* Enable timer interrupt */
	QM_IR_UNMASK_INT(QM_IRQ_PWM_0_INT);
	QM_IRQ_REQUEST(QM_IRQ_PWM_0_INT, qm_pwm_0_isr_0);

	/* Start the timer. */
	qm_pwm_start(QM_PWM_0, QM_PWM_ID_1);
}

#endif /* TIMER_C_ */
