/*
 * Reuse the unchanged upstream Device implementation while replacing its
 * historical model string with the name of this reference target.
 */
#define Model_Name Mega2560_Model_Name
#include "../vendor/bacnet-stack/ports/atmega328/device.c"
#undef Model_Name

static const char *Mega2560_Model_Name = "Arduino Mega 2560 MS/TP Demo";
