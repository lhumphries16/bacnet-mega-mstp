# Validation

Validation for this repository has three separate tiers.

## 1. Compile tested

From the repository root:

~~~sh
./bootstrap_bacnet_stack.sh
make clean all
~~~

Acceptance:

- the pinned revision is checked out;
- AVR-GCC completes every compilation unit;
- the linker produces the ELF;
- objcopy produces the HEX;
- avr-size reports the image size.

## 2. Flash tested

Connect an Arduino Mega 2560 bootloader port and run:

~~~text
make AVRDUDE_PORT=COM15 install
~~~

Acceptance:

- avrdude identifies the ATmega2560 bootloader;
- the HEX image is written without verification errors;
- the upload reports successful flash verification.

## 3. Physical BACnet MS/TP tested

Use a BACnet MS/TP client on the same trunk.

1. Send Who-Is and observe I-Am for device instance 1234.
2. Read the Device object identifier and object list.
3. Read BV1, named Demo Enable.
4. Write BV1 active and read it back; write it inactive and read it back.
5. Read AV1, named Demo Value.
6. Write 42.5 to AV1 and read it back; restore 0.0 and read it back.
7. Reset or power-cycle the Mega and confirm BV1 returns inactive and AV1
   returns 0.0.

The source-level cleanup in this repository invalidates any application-level
test result from an earlier target. Each tier must be recorded after this
minimal reference image is built and tested.

## Acceptance record — 2026-09-25

Source baseline: `564ec4d787ba1260d902f0544d74d1f9a61295ae`.
The pinned bacnet-stack checkout was verified at
`6bfb0108d4d68835fd0c1062731b54f559cb1375`, matching
`BACNET_STACK_REVISION`.

| Tier | Result | Evidence |
| --- | --- | --- |
| Compile | PASS | Windows x64; GNU Make 4.2.1; AVR-GCC 16.1.0; AVR Binutils 2.46.1; `make clean all` completed. |
| Flash | PASS | Arduino Mega 2560 bootloader on COM15; avrdude 8.2; 21,446 bytes written and verified. |
| Physical BACnet MS/TP | PASS | YABE 2.1.0 on COM17 discovered Device 1234, read the expected object list, exercised BV1 and AV1 writes, restored defaults, and passed reset/default checks. Bench test only. |

The build produced `bacnet_mega2560_mstp.elf` (72,816 bytes) and
`bacnet_mega2560_mstp.hex` (60,351 bytes). `avr-size` reported text 20,962,
data 484, and bss 207 bytes. This is 21,446 bytes of flash image data and
691 bytes of static SRAM use. The build emitted nonfatal warnings from pinned
upstream BACnet sources; no upstream source was changed to silence them.

Windows identified the Arduino Mega 2560 programming interface as COM15:
VID `2341`, PID `0042`, device serial `8573432313035170F010`. The separate
PC-side USB-RS485 client interface remained COM17: FTDI VID `0403`, PID
`6001`, serial `BH00SKCVA`. The verified upload command was:

~~~text
avrdude -v -c wiring -p m2560 -P COM15 -b 115200 -D -U flash:w:bacnet_mega2560_mstp.hex:i
~~~

avrdude 8.2 identified the Wiring/STK500v2 bootloader, negotiated at 115200,
read ATmega2560 signature `1E 98 01`, wrote 21,446 bytes, and verified the
flash. The previous attempt failed during avrdude's separate chip-erase step.
The successful command used `-D` to skip that step and let the bootloader erase
pages as it wrote. The previous attempt also omitted an explicit baud; the
successful command used 115200, so that parameter is now explicit in Makefile.

YABE used client MS/TP MAC 1; the Mega used MAC 10 at 38400 baud, Max Master
20, and Max Info Frames 1. Who-Is discovery returned I-Am for Device 1234.
The object list was Device 1234, AV1 Demo Value, and BV1 Demo Enable. BV1
defaulted inactive, read back active after a write, then read back inactive
after restoration. AV1 defaulted to 0.0, read back 42.5 after a write, then
read back 0.0 after restoration. Following reset, BV1 was inactive and AV1
was 0.0. The screenshots showed successful property replies and both values
restored to 0. This is bench validation only; it is not production, field, or
BTL certification.
