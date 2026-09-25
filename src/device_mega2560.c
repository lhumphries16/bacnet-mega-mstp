/*
 * Reuse the unchanged upstream Device implementation while giving this
 * reference target an accurate model string.
 */
#define Model_Name Mega2560_Model_Name
#include "../vendor/bacnet-stack/ports/atmega328/device.c"
#undef Model_Name

void Mega2560_Device_Model_Name_Init(void)
{
    Mega2560_Model_Name = "Arduino Mega 2560 MS/TP Demo";
}
