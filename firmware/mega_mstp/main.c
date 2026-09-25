/* CTRL Scout physical BACnet MS/TP fan-array controller.
 * Protocol services remain in the pinned official bacnet-stack sources. */
#include <stdbool.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "hardware.h"
#include "rs485.h"
#include "bacnet/datalink/datalink.h"
#include "bacnet/npdu.h"
#include "bacnet/basic/services.h"
#include "bacnet/basic/sys/mstimer.h"
#include "bacnet/basic/object/device.h"
#include "bacnet/basic/object/av.h"
#include "bacnet/basic/object/bv.h"
#include "bacnet/basic/object/ai.h"

extern bool Send_I_Am_Flag;
enum { DEVICE_INSTANCE = 1234, MSTP_MAC = 10, MSTP_BAUD = 38400, MSTP_MAX_MASTER = 20, INA3221_ADDRESS = 0x40 };
/* Calibrate this one value from the installed INA3221 breakout. */
static const float INA_SHUNT_OHMS = 0.1f;
static const float CURRENT_EMA_ALPHA = 0.25f;
static float Current_A[3];

static void pwm_init(void)
{
    DDRE |= _BV(PE3); /* D5 / OC3A */
    DDRH |= _BV(PH3) | _BV(PH4); /* D6 / OC4A, D7 / OC4B: PWM board 2 and 4 */
    TCCR3A = _BV(COM3A1) | _BV(WGM30); TCCR3B = _BV(WGM32) | _BV(CS31) | _BV(CS30);
    TCCR4A = _BV(COM4A1) | _BV(COM4B1) | _BV(WGM40); TCCR4B = _BV(WGM42) | _BV(CS41) | _BV(CS40);
    OCR3A = 0; OCR4A = 0; OCR4B = 0;
}
static void pwm_apply(bool enabled, float command)
{
    if (command < 0.0f) command = 0.0f; if (command > 1000.0f) command = 1000.0f;
    uint8_t duty = enabled ? (uint8_t)((command * 255.0f / 1000.0f) + 0.5f) : 0;
    OCR3A = duty; OCR4A = duty; OCR4B = duty;
}
static bool twi_wait(uint8_t expected)
{
    uint16_t guard = 0; while (!(TWCR & _BV(TWINT)) && guard++ < 60000) {}
    return (TWSR & 0xF8) == expected;
}
static bool twi_start(uint8_t address)
{
    TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN); if (!twi_wait(0x08) && !twi_wait(0x10)) return false;
    TWDR = address; TWCR = _BV(TWINT) | _BV(TWEN); return twi_wait((address & 1) ? 0x40 : 0x18);
}
static void twi_stop(void) { TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWSTO); }
static bool ina_write(uint8_t reg, uint16_t value)
{
    if (!twi_start(INA3221_ADDRESS << 1)) return false;
    TWDR = reg; TWCR = _BV(TWINT) | _BV(TWEN); if (!twi_wait(0x28)) return false;
    TWDR = (uint8_t)(value >> 8); TWCR = _BV(TWINT) | _BV(TWEN); if (!twi_wait(0x28)) return false;
    TWDR = (uint8_t)value; TWCR = _BV(TWINT) | _BV(TWEN); if (!twi_wait(0x28)) return false;
    twi_stop(); return true;
}
static bool ina_read(uint8_t reg, uint16_t *value)
{
    if (!twi_start(INA3221_ADDRESS << 1)) return false;
    TWDR = reg; TWCR = _BV(TWINT) | _BV(TWEN); if (!twi_wait(0x28)) return false;
    if (!twi_start((INA3221_ADDRESS << 1) | 1)) return false;
    TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWEA); if (!twi_wait(0x50)) return false; uint8_t high = TWDR;
    TWCR = _BV(TWINT) | _BV(TWEN); if (!twi_wait(0x58)) return false; *value = ((uint16_t)high << 8) | TWDR; twi_stop(); return true;
}
static void ina_init(void)
{
    TWSR = 0; TWBR = 72; TWCR = _BV(TWEN); ina_write(0x00, 0x7127);
    for (uint8_t i = 0; i < 3; i++) Current_A[i] = 0.0f;
}
static void sample_currents(void)
{
    for (uint8_t i = 0; i < 3; i++) {
        uint16_t raw; if (!ina_read((uint8_t)(0x01 + i * 2), &raw)) continue;
        float current = ((float)(((int16_t)raw) >> 3) * 0.00004f) / INA_SHUNT_OHMS;
        if (current < 0.0f) current = 0.0f;
        Current_A[i] = CURRENT_EMA_ALPHA * current + (1.0f - CURRENT_EMA_ALPHA) * Current_A[i];
        Analog_Input_Present_Value_Set(i + 1, Current_A[i]);
    }
}

int main(void)
{
    uint16_t pdu_len; BACNET_ADDRESS src; uint8_t pdu[MAX_MPDU + 16]; unsigned long last_sample = 0;
    RS485_Initialize(); mstimer_init(); pwm_init(); ina_init();
    Analog_Value_Init(); Binary_Value_Init(); Analog_Input_Init(); Device_Set_Object_Instance_Number(DEVICE_INSTANCE);
    Analog_Input_Object_Instance_Add(1); Analog_Input_Object_Instance_Add(2); Analog_Input_Object_Instance_Add(3);
    Analog_Input_Name_Set(1, "Fan 1 Current"); Analog_Input_Name_Set(2, "Fan 2 Current"); Analog_Input_Name_Set(3, "Fan 3 Current");
    Analog_Input_Units_Set(1, UNITS_AMPERES); Analog_Input_Units_Set(2, UNITS_AMPERES); Analog_Input_Units_Set(3, UNITS_AMPERES);
    Analog_Value_Name_Set(1, "Remote Speed Command"); Binary_Value_Name_Set(1, "Remote Start");
    Binary_Value_Present_Value_Set(1, BINARY_INACTIVE); Analog_Value_Present_Value_Set(1, 0.0f, 0); pwm_apply(false, 0.0f);
    dlmstp_set_mac_address(MSTP_MAC); dlmstp_set_max_master(MSTP_MAX_MASTER); dlmstp_set_max_info_frames(1);
    RS485_Set_Baud_Rate(MSTP_BAUD); dlmstp_init(NULL); Send_I_Am_Flag = true; sei();
    for (;;) {
        bool enabled = Binary_Value_Present_Value(1) == BINARY_ACTIVE; float speed = Analog_Value_Present_Value(1);
        if (speed < 0.0f) speed = 0.0f; if (speed > 1000.0f) speed = 1000.0f; Analog_Value_Present_Value_Set(1, speed, 0);
        pwm_apply(enabled, speed); unsigned long now = mstimer_now();
        if (now - last_sample >= 100) { last_sample = now; sample_currents(); }
        pdu_len = dlmstp_receive(&src, pdu, MAX_MPDU, 0); if (pdu_len) npdu_handler(&src, pdu, pdu_len);
    }
}
