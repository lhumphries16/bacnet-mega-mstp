/* Official bacnet-stack MS/TP UART adapter for Mega Serial1.
 * Mega pins: TX1/D18 -> DI, RX1/D19 <- RO, D2 -> DE and /RE. */
#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>
#include "bacnet/basic/sys/mstimer.h"
#include "rs485.h"

#define RS485_DE_DDR DDRE
#define RS485_DE_PORT PORTE
#define RS485_DE_BIT PE4
#if RS485_DE_BIT != PE4
#error "Mega D2 must map to PE4 for RS485 DE"
#endif
#if RS485_DE_BIT == PD2
#error "RS485 DE must not use PD2; PD2 is Mega USART1 RX / Arduino D19"
#endif
static uint32_t Baud_Rate = 38400;
static struct mstimer Silence_Timer;
unsigned long RS485_Timer_Silence(void) { return mstimer_elapsed(&Silence_Timer); }
void RS485_Timer_Silence_Reset(void) { mstimer_set(&Silence_Timer, 0); }
void RS485_Initialize(void)
{
#if defined(PRR1) && defined(PRUSART1)
    PRR1 &= (uint8_t)~_BV(PRUSART1);
#endif
    UCSR1B = _BV(TXEN1) | _BV(RXEN1);
    UCSR1C = _BV(UCSZ11) | _BV(UCSZ10);
    RS485_DE_DDR |= _BV(RS485_DE_BIT); RS485_DE_PORT &= (uint8_t)~_BV(RS485_DE_BIT);
}
uint32_t RS485_Get_Baud_Rate(void) { return Baud_Rate; }
uint32_t RS485_Baud_Rate_From_Kilo(uint8_t value)
{
    if (value == 255) return 38400;
    return value >= 115 ? 115200 : value >= 76 ? 76800 : value >= 57 ? 57600 : value >= 38 ? 38400 : value >= 19 ? 19200 : value >= 9 ? 9600 : 38400;
}
bool RS485_Set_Baud_Rate(uint32_t baud)
{
    if (baud != 9600 && baud != 19200 && baud != 38400 && baud != 57600 && baud != 76800 && baud != 115200) return false;
    Baud_Rate = baud; UCSR1A |= _BV(U2X1); UBRR1 = (F_CPU / (8UL * Baud_Rate)) - 1; return true;
}
void RS485_Transmitter_Enable(bool enable) { if (enable) RS485_DE_PORT |= _BV(RS485_DE_BIT); else RS485_DE_PORT &= (uint8_t)~_BV(RS485_DE_BIT); }
void RS485_Send_Data(const uint8_t *buffer, uint16_t length)
{
    while (length) {
        while (!(UCSR1A & _BV(UDRE1))) {}
        UDR1 = *buffer++;
        length--;
    }
    while (!(UCSR1A & _BV(TXC1))) {}
    UCSR1A |= _BV(TXC1);
    RS485_Timer_Silence_Reset();
}
void RS485_Turnaround_Delay(void)
{
    uint8_t nbytes = 4;
    RS485_Transmitter_Enable(false);
    while (nbytes) {
        while (!(UCSR1A & _BV(UDRE1))) {}
        UDR1 = 0xff;
        nbytes--;
    }
    while (!(UCSR1A & _BV(TXC1))) {}
    UCSR1A |= _BV(TXC1);
}
bool RS485_ReceiveError(void) { if (UCSR1A & _BV(DOR1)) { while (UCSR1A & _BV(RXC1)) (void)UDR1; return true; } return false; }
bool RS485_DataAvailable(uint8_t *data) { if (!(UCSR1A & _BV(RXC1))) return false; *data = UDR1; return true; }
void RS485_LED_Timers(void) {}
