# BACnet MS/TP on Arduino Mega 2560

A small reference target for running the open-source bacnet-stack BACnet MS/TP implementation on an Arduino Mega 2560 / ATmega2560 using USART1 and an external RS-485 transceiver.

## Why this exists

The purpose of this repository is narrow:

> How can the upstream bacnet-stack BACnet MS/TP implementation run on an Arduino Mega 2560 / ATmega2560 through USART1 and an external half-duplex RS-485 transceiver?

The repository isolates the reusable platform delta:

- ATmega2560 build and bootloader settings;
- USART1 register handling;
- Arduino Mega pin mapping;
- RS-485 driver/receiver direction control;
- a Timer2 millisecond clock;
- a reproducible upstream dependency;
- a minimal BACnet device used to exercise the protocol.

This is a reference target, not a new BACnet implementation and not a fork of bacnet-stack.

## Hardware requirements

- Arduino Mega 2560 / ATmega2560 running at 16 MHz;
- compatible external half-duplex RS-485 transceiver;
- low-voltage supply appropriate for the Mega and transceiver;
- BACnet MS/TP trunk and a BACnet client for validation;
- AVR-GCC, avr-libc, GNU Make, Git, and avrdude.

The transceiver must expose separate receive, transmit, and direction-control signals. Follow the selected transceiver documentation for supply, logic levels, signal naming, and network connection.

## Pin map

| Mega signal | ATmega2560 pin | Transceiver / network |
| --- | --- | --- |
| D18 / TX1 | PD3 | DI |
| D19 / RX1 | PD2 | RO |
| D2 | PE4 | DE and /RE |
| GND | GND | GND |
| 5 V or transceiver-rated supply | VCC | VCC |
| A / B | RS-485 A / B | MS/TP trunk |

D2 is the only direction-control mapping supported by this reference. The
source contains compile-time checks that make the PE4 mapping explicit and
reject an accidental PD2 mapping.

## Architecture

~~~text
BACnet MS/TP client
        |
      RS-485
        |
external half-duplex transceiver
        |
USART1 + D2 DE/RE
        |
ATmega2560
        |
bacnet-stack
        |
minimal BACnet demonstration device
~~~

The Mega firmware uses the upstream MS/TP datalink and BACnet service code.
Local platform code supplies the UART, direction control, millisecond timer,
and hardware guard. The demonstration values are held in RAM and are not
connected to physical outputs.

## BACnet demonstration device

The reference starts one BACnet MS/TP device with:

| Setting | Value |
| --- | --- |
| Device instance | 1234 |
| MS/TP MAC | 10 |
| Baud rate | 38400 |
| Max Master | 20 |
| Max Info Frames | 1 |

| Object | Name | Access | Meaning |
| --- | --- | --- | --- |
| BV1 | Demo Enable | read/write | In-memory binary value |
| AV1 | Demo Value | read/write | In-memory real value |

The target is intended to demonstrate Who-Is, I-Am, ReadProperty, and
WriteProperty for the Device, BV1, and AV1 objects.

## Build from the repository root

The dependency revision is stored once in BACNET_STACK_REVISION.

POSIX shell:

~~~sh
./bootstrap_bacnet_stack.sh
make clean all
~~~

PowerShell:

~~~powershell
powershell -ExecutionPolicy Bypass -File .\bootstrap_bacnet_stack.ps1
make clean all
~~~

The build produces the ELF, HEX, map, and size report. The upstream checkout
is fetched into vendor/bacnet-stack and remains ignored by Git.

## Flash

For an Arduino Mega bootloader connection:

~~~text
make AVRDUDE_PORT=COM15 install
~~~

The default programmer is wiring and the default MCU is m2560. Override
AVRDUDE_PORT, AVRDUDE_PROGRAMMER, or AVRDUDE_MCU when the programming
connection requires it.

## Validation

Validation is intentionally separated into tiers:

1. Compile tested: the root Makefile completes with the pinned dependency.
2. Flash tested: the generated image is accepted by the Mega bootloader.
3. Physical BACnet MS/TP tested: a client discovers device 1234 and completes
   the ReadProperty and WriteProperty checks listed in docs/validation.md.

A tier is not implied by the existence of the code or by an earlier
application-level bench result.

## Relationship to bacnet-stack

The dependency is fetched from:

https://github.com/bacnet-stack/bacnet-stack

The exact revision is recorded in BACNET_STACK_REVISION and verified by both
bootstrap scripts. No upstream source is edited by this repository.

The local source files are intentionally small:

- src/hardware.h: ATmega2560 guard and shared platform include;
- src/rs485_mega2560.c: USART1 and PE4 direction-control implementation;
- src/timer_mega2560.c: Timer2 millisecond implementation;
- src/dlmstp_mega2560.c: include wrapper that supplies the Mega hardware
  definition to the unchanged upstream MS/TP datalink;
- src/device_mega2560.c: include wrapper that gives the unchanged upstream
  Device implementation an accurate model string;
- src/demo_objects.c: only the two RAM-backed objects required by this
  demonstration;
- src/main.c: target configuration and receive loop.

Everything else needed from the upstream ATmega328 reference is compiled from
the fetched checkout rather than copied into this repository.

## Limitations

- This is a low-voltage laboratory reference, not a certified field product.
- Electrical isolation, transient protection, network termination, and biasing
  are outside this repository and depend on the selected hardware.
- Device instance, MS/TP MAC, baud rate, and Max Master are compile-time
  settings in the demonstration.
- The demonstration values are volatile and reset on power-up.
- No physical output is controlled by BV1 or AV1.
- The target does not provide a safety supervisor or independent output release
  circuit.

A small portion of the local target was separated from a larger private
hardware project; application-specific code was intentionally removed from
this repository.
