/**
 * @brief Timer2 millisecond clock for the ATmega2560 MS/TP target.
 * @details At 16 MHz, a divide-by-64 clock is 250 kHz. Loading Timer2
 *          with 6 leaves 250 ticks before overflow, or 1 ms.
 * @copyright SPDX-License-Identifier: MIT
 */
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>
#include "bacnet/basic/sys/mstimer.h"

static volatile unsigned long Millisecond_Counter;

void mstimer_init(void)
{
#if defined(PRR1) && defined(PRTIM2)
    PRR1 &= (uint8_t)~_BV(PRTIM2);
#endif
    TCCR2A = 0;
    TCCR2B = _BV(CS22);
    TCNT2 = 6;
    TIFR2 = _BV(TOV2);
    TIMSK2 |= _BV(TOIE2);
}

ISR(TIMER2_OVF_vect)
{
    TCNT2 = 6;
    Millisecond_Counter++;
}

unsigned long mstimer_now(void)
{
    unsigned long result;
    uint8_t saved = SREG;
    cli();
    result = Millisecond_Counter;
    SREG = saved;
    return result;
}
