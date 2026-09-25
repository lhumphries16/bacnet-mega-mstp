# Wiring

This document covers only the Mega 2560, the external half-duplex RS-485
transceiver, and the BACnet MS/TP trunk.

| Mega signal | ATmega2560 pin | Transceiver |
| --- | --- | --- |
| D18 / TX1 | PD3 | DI |
| D19 / RX1 | PD2 | RO |
| D2 | PE4 | DE and /RE |
| GND | GND | GND |
| appropriate supply | VCC | VCC |
| A / B | RS-485 A / B | BACnet MS/TP trunk |

The local source assumes:

- USART1 is used for the BACnet serial channel;
- D2/PE4 drives the transceiver direction input(s);
- the transceiver is half-duplex;
- signal polarity is verified against the selected transceiver and receiving
  device documentation.

Use a low-voltage setup and follow the selected transceiver documentation for
logic levels, supply limits, connector pinout, protection, termination, and
biasing. This repository does not prescribe those values for a particular
transceiver.
