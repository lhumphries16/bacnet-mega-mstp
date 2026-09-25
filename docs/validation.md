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
- reset leaves the serial transceiver in receive mode.

## 3. Physical BACnet MS/TP tested

Use a BACnet MS/TP client on the same trunk.

1. Send Who-Is and observe I-Am for device instance 1234.
2. Read the Device object identifier and object list.
3. Read BV1, named Demo Enable.
4. Write BV1 inactive/active and read it back.
5. Read AV1, named Demo Value.
6. Write a real value to AV1 and read it back.
7. Power-cycle the Mega and confirm BV1 returns inactive and AV1 returns zero.

The source-level cleanup in this repository invalidates any application-level
test result from an earlier target. Each tier must be recorded after this
minimal reference image is built and tested.
