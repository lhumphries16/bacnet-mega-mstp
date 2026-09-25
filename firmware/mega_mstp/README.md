# CTRL Scout Mega BACnet MS/TP target

This is a new physical target. `firmware/mega_demo/` remains the legacy relay
bench and is not modified by this target.

The protocol implementation is the official `bacnet-stack` AVR implementation,
pinned at revision `6bfb0108d4d68835fd0c1062731b54f559cb1375` (master observed
2026-09-20). The upstream sources are kept in `firmware/vendor/bacnet-stack/`;
the derived files under `port/` only reduce the object table and add AI
dispatch for this fan array.

## Hardware map

| Function | Mega pin / object |
|---|---|
| HW-97 DE + /RE | D2 / PE4 |
| HW-97 DI / RO | TX1 D18 / PD3 / RX1 D19 / PD2 |
| PWM board 1 / Fan 1 | D5 / Timer3A |
| PWM board 2 / Fan 2 | D6 / Timer4A |
| PWM board 4 / Fan 3 | D7 / Timer4B |
| INA3221 | SDA D20, SCL D21 |

PWM board channel 3 / OUT3 is intentionally unused because its terminal is
defective. D7 controls board channel 4.

The official AVR MS/TP timer was moved from Timer0 to Timer2 in
`timer_mega2560.c`; Arduino Timer0 is not used by this standalone target and
PWM uses Timer3/Timer4. No MS/TP timer is shared with D5/D6/D7.

## BACnet objects

- Device 1234, MS/TP MAC 10, 38400 baud, Max Master 20
- BV1 `Remote Start` — writable Present_Value
- AV1 `Remote Speed Command` — writable Present_Value, clamped 0..1000
- AI1 `Fan 1 Current` — read-only amperes
- AI2 `Fan 2 Current` — read-only amperes
- AI3 `Fan 3 Current` — read-only amperes

Boot sets BV1 false, AV1 zero, PWM zero, and initializes all current values to
zero. INA3221 current uses a modest EMA and the centralized `INA_SHUNT_OHMS`
constant in `main.c`. RPM is not measured or inferred.

## Build and upload

From a clean repository checkout, first fetch the exact upstream source:

~~~powershell
powershell -ExecutionPolicy Bypass -File firmware/bootstrap_bacnet_stack.ps1
~~~

Install AVR-GCC, avr-libc, GNU Make, and avrdude. From this directory:

```text
make clean all
make AVRDUDE_PORT=COM5 install
```

Replace `COM5` with the Mega bootloader port. `make clean all` recreates the object files, dependency files, ELF, map, and HEX image; those generated files are intentionally ignored.
The MS/TP DE register mapping is compile-guarded so D2 cannot regress to PD2,
which is USART1 RX.
