# Porting notes

This target is a small ATmega2560 platform delta around the upstream AVR
reference. The upstream ATmega328 port remains the comparison point because
the MS/TP datalink and several service helpers can be compiled unchanged.

## MCU target

| Concern | Upstream ATmega328 reference | This target |
| --- | --- | --- |
| MCU | ATmega328P | ATmega2560 |
| Clock | 16 MHz | 16 MHz |
| Build flag | -mmcu=atmega328p | -mmcu=atmega2560 |
| avrdude MCU | m328p | m2560 |
| bootloader programmer | board-specific Arduino setting | wiring by default |

## Serial interface

| Concern | Upstream ATmega328 reference | This target |
| --- | --- | --- |
| UART | USART0 | USART1 |
| Arduino pins | D0/D1 | D18/D19 |
| AVR receive register | UCSR0A / UDR0 / RXC0 | UCSR1A / UDR1 / RXC1 |
| AVR transmit register | UCSR0A / UDR0 / UDRE0 / TXC0 | UCSR1A / UDR1 / UDRE1 / TXC1 |
| baud register | UBRR0 | UBRR1 |
| double-speed bit | U2X0 | U2X1 |
| power reduction | PRR / PRUSART0 | PRR1 / PRUSART1 |
| direction control | PD2 | PE4 |

The local UART implementation is src/rs485_mega2560.c. It retains the
upstream RS-485 API so the upstream MS/TP datalink can use the target without
a protocol-layer change.

## Timer

The target uses Timer2 for the millisecond clock. At 16 MHz, a divide-by-64
clock is 250 kHz; 250 timer ticks are 1 ms, so an 8-bit overflow is scheduled
from count 6.

Timer2 was selected because it is an available 8-bit timer on the Mega and
provides a direct 1 ms overflow calculation for this reference. The timer
implementation is local because the upstream ATmega328 source uses a
different timer/register arrangement.

## Build differences

The root Makefile:

- fetches source from vendor/bacnet-stack;
- compiles with the ATmega2560 target;
- preloads src/hardware.h so upstream AVR files see the Mega guard;
- compiles the upstream service helpers directly;
- links only the minimal local demonstration objects.

The source wrapper src/dlmstp_mega2560.c exists because the upstream datalink
includes its hardware header by a relative name. It does not copy or modify
the datalink source.
