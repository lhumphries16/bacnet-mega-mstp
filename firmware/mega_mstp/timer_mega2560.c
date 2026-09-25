/* BACnet mstimer port for Mega. Arduino Timer0 is deliberately untouched. */
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>
#include "bacnet/basic/sys/mstimer.h"

static volatile unsigned long Millisecond_Counter;

void mstimer_init(void)
{
    TCCR2A = 0;
    TCCR2B = _BV(CS22); /* clk/64: 250 ticks is approximately 1 ms */
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
