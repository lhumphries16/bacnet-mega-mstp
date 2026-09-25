# BACnet Mega MS/TP

Standalone BACnet MS/TP firmware target for an Arduino Mega 2560, extracted from the CTRL Scout physical bench implementation.

This repository is a bench/reference target. It is intended to prove BACnet MS/TP communication with a small fan-array controller; it is not a CAT-rated instrument, a mains-voltage controller, or a production safety device.

## What this target exposes

The Mega presents one BACnet MS/TP device:

| Setting | Value |
| --- | --- |
| Device instance | 1234 |
| MS/TP MAC | 10 |
| Baud rate | 38400 |
| Max Master | 20 |
| Max Info Frames | 1 |

| Object | Name | Access | Meaning |
| --- | --- | --- | --- |
| BV1 | Remote Start | read/write | Enables the three PWM outputs |
| AV1 | Remote Speed Command | read/write | Command from 0 to 1000 |
| AI1 | Fan 1 Current | read-only | INA3221 channel 1, amperes |
| AI2 | Fan 2 Current | read-only | INA3221 channel 2, amperes |
| AI3 | Fan 3 Current | read-only | INA3221 channel 3, amperes |

On boot, Remote Start is inactive, Remote Speed Command is zero, and all PWM outputs are off. AV1 is clamped to 0…1000. The firmware does not measure or infer RPM, airflow, or mechanical health.

## Hardware map

| Function | Mega connection |
| --- | --- |
| RS-485 transceiver DE and /RE | D2 / PE4 |
| RS-485 transceiver DI | TX1 / D18 / PD3 |
| RS-485 transceiver RO | RX1 / D19 / PD2 |
| PWM board channel 1 / Fan 1 | D5 / OC3A |
| PWM board channel 2 / Fan 2 | D6 / OC4A |
| PWM board channel 4 / Fan 3 | D7 / OC4B |
| INA3221 SDA | D20 |
| INA3221 SCL | D21 |

PWM board channel 3 is intentionally unused because the bench board's channel 3 terminal was defective. D7 drives board channel 4.

The official MS/TP timer is moved to Timer2. PWM uses Timer3 and Timer4, so the MS/TP timer does not share D5, D6, or D7.

## Repository boundary

This repository contains:

- the Mega 2560 application and hardware adapters;
- the reduced BACnet object implementations needed by this target;
- the exact upstream-source bootstrap script;
- build and wiring documentation.

It intentionally does not contain the CTRL Scout Python runtime, web UI, recipe engine, .NET sidecar, or application-specific equipment profiles. Those remain in the Scout repository.

The upstream `bacnet-stack` source is fetched into `firmware/vendor/bacnet-stack` at the exact revision below. It is not copied into this repository and is not modified by the build.

- Repository: https://github.com/bacnet-stack/bacnet-stack
- Pinned revision: `6bfb0108d4d68835fd0c1062731b54f559cb1375`

The files in `firmware/mega_mstp/port/` are the target-specific, reduced object-table and dispatch adaptations used by this application. The upstream pull request is intentionally deferred; this repository is the standalone deliverable for the Mega target.

## Build and upload

### Prerequisites

Install:

- AVR-GCC and avr-libc;
- GNU Make;
- Git;
- avrdude;
- PowerShell 5+ or PowerShell 7 for the bootstrap script.

From the repository root on Windows:

```powershell
powershell -ExecutionPolicy Bypass -File firmware/bootstrap_bacnet_stack.ps1
Set-Location firmware/mega_mstp
make clean all
make AVRDUDE_PORT=COM5 install
```

Replace `COM5` with the Mega bootloader port. On Linux/macOS, fetch the same pinned revision into `firmware/vendor/bacnet-stack` manually, then run `make clean all` from `firmware/mega_mstp` and set `AVRDUDE_PORT` to the serial device.

Build outputs (`.elf`, `.hex`, `.map`, object files, and dependency files) are generated locally and ignored by Git.

## Wiring notes

Use a low-voltage bench supply and the appropriate common/reference wiring for the selected RS-485 transceiver and fan/PWM hardware. The firmware does not provide galvanic isolation, CAT measurement protection, or field miswire protection.

For the HW-97-style RS-485 board:

- Mega TX1/D18 → DI;
- Mega RX1/D19 ← RO;
- Mega D2 → DE and /RE;
- connect VCC/GND according to the transceiver board;
- connect RS-485 A/B to the MS/TP network with polarity verified at the receiving device.

Connect the INA3221 to Mega I²C (SDA D20, SCL D21). The current conversion assumes a 0.1 Ω shunt in `main.c`; calibrate `INA_SHUNT_OHMS` for the installed breakout before treating readings as meaningful.

## Verification

The minimum bench verification is:

1. Build the firmware and confirm the size report.
2. Flash the Mega and confirm it starts with all outputs off.
3. Discover device 1234 from a BACnet MS/TP client.
4. Read BV1, AV1, and AI1–AI3.
5. Write AV1 and BV1, then confirm PWM output behavior.
6. Confirm current values change with the fan load.
7. Power-cycle the bench and confirm the safe startup state.

The target was originally exercised as part of CTRL Scout's physical bench work. A successful firmware build is not equivalent to electrical, protocol, or field acceptance.

## Known limitations

- Device instance, MAC, baud, and object inventory are compile-time values.
- The object set is intentionally small and fan-array-specific.
- INA3221 communication failure currently leaves the last sampled value in place; a production target should expose explicit sensor quality/fault state.
- The firmware has no application-level safety supervisor or independent physical RELEASE ALL circuit.
- The target is for low-voltage laboratory work only.
- The current implementation has been extracted from the Scout bench target; generalizing the port or proposing upstream changes is a separate task.

## License and attribution

See [NOTICE.md](NOTICE.md) for source provenance and attribution. Review the pinned upstream repository's licensing terms before redistributing a build that includes the fetched BACnet stack.
