/**
 * @brief Minimal BACnet MS/TP demonstration for the Arduino Mega 2560.
 * @copyright SPDX-License-Identifier: MIT
 */
#include <stdbool.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include "rs485.h"
#include "bacnet/datalink/datalink.h"
#include "bacnet/datalink/dlmstp.h"
#include "bacnet/npdu.h"
#include "bacnet/basic/services.h"
#include "bacnet/basic/sys/mstimer.h"
#include "bacnet/basic/object/device.h"
#include "bacnet/basic/object/av.h"
#include "bacnet/basic/object/bv.h"

extern bool Send_I_Am_Flag;
extern void Mega2560_Device_Model_Name_Init(void);

enum {
    DEVICE_INSTANCE = 1234,
    MSTP_MAC = 10,
    MSTP_BAUD = 38400,
    MSTP_MAX_MASTER = 20
};

int main(void)
{
    uint16_t pdu_length;
    BACNET_ADDRESS source;
    uint8_t pdu[MAX_MPDU + 16];

    RS485_Initialize();
    mstimer_init();
    Analog_Value_Init();
    Binary_Value_Init();
    Device_Set_Object_Instance_Number(DEVICE_INSTANCE);
    Mega2560_Device_Model_Name_Init();
    Analog_Value_Present_Value_Set(1, 0.0f, 0);
    Binary_Value_Present_Value_Set(1, BINARY_INACTIVE);

    dlmstp_set_mac_address(MSTP_MAC);
    dlmstp_set_max_master(MSTP_MAX_MASTER);
    dlmstp_set_max_info_frames(1);
    RS485_Set_Baud_Rate(MSTP_BAUD);
    dlmstp_init(NULL);

    Send_I_Am_Flag = true;
    sei();

    for (;;) {
        pdu_length = dlmstp_receive(
            &source, pdu, MAX_MPDU, 0);
        if (pdu_length) {
            npdu_handler(&source, pdu, pdu_length);
        }
    }
}
