/**
 * @brief Minimal RAM-backed AV1 and BV1 objects for the platform reference.
 * @details This is demonstration glue, not a general BACnet object library.
 * @copyright SPDX-License-Identifier: MIT
 */
#include <stdbool.h>
#include <stdint.h>
#include "bacnet/bacapp.h"
#include "bacnet/bacdef.h"
#include "bacnet/bacdcode.h"
#include "bacnet/basic/object/av.h"
#include "bacnet/basic/object/bv.h"

static float Demo_Value;
static BACNET_BINARY_PV Demo_Enable;

static bool valid_instance(uint32_t instance)
{
    return instance == 1;
}

void Analog_Value_Init(void)
{
    Demo_Value = 0.0f;
}

unsigned Analog_Value_Count(void)
{
    return 1;
}

uint32_t Analog_Value_Index_To_Instance(unsigned index)
{
    return index == 0 ? 1 : BACNET_MAX_INSTANCE;
}

unsigned Analog_Value_Instance_To_Index(uint32_t instance)
{
    return valid_instance(instance) ? 0 : 1;
}

bool Analog_Value_Valid_Instance(uint32_t instance)
{
    return valid_instance(instance);
}

const char *Analog_Value_Name_ASCII(uint32_t instance)
{
    return valid_instance(instance) ? "Demo Value" : "AV-X";
}

bool Analog_Value_Object_Name(
    uint32_t instance, BACNET_CHARACTER_STRING *name)
{
    return valid_instance(instance) &&
        characterstring_init_ansi(name, Analog_Value_Name_ASCII(instance));
}

float Analog_Value_Present_Value(uint32_t instance)
{
    return valid_instance(instance) ? Demo_Value : 0.0f;
}

bool Analog_Value_Present_Value_Set(
    uint32_t instance, float value, uint8_t priority)
{
    (void)priority;
    if (!valid_instance(instance)) return false;
    Demo_Value = value;
    return true;
}

void Binary_Value_Init(void)
{
    Demo_Enable = BINARY_INACTIVE;
}

unsigned Binary_Value_Count(void)
{
    return 1;
}

uint32_t Binary_Value_Index_To_Instance(unsigned index)
{
    return index == 0 ? 1 : BACNET_MAX_INSTANCE;
}

unsigned Binary_Value_Instance_To_Index(uint32_t instance)
{
    return valid_instance(instance) ? 0 : 1;
}

bool Binary_Value_Valid_Instance(uint32_t instance)
{
    return valid_instance(instance);
}

const char *Binary_Value_Name_ASCII(uint32_t instance)
{
    return valid_instance(instance) ? "Demo Enable" : "BV-X";
}

bool Binary_Value_Object_Name(
    uint32_t instance, BACNET_CHARACTER_STRING *name)
{
    return valid_instance(instance) &&
        characterstring_init_ansi(name, Binary_Value_Name_ASCII(instance));
}

BACNET_BINARY_PV Binary_Value_Present_Value(uint32_t instance)
{
    return valid_instance(instance) ? Demo_Enable : BINARY_INACTIVE;
}

bool Binary_Value_Present_Value_Set(
    uint32_t instance, BACNET_BINARY_PV value)
{
    if (!valid_instance(instance)) return false;
    Demo_Enable = value == BINARY_ACTIVE ? BINARY_ACTIVE : BINARY_INACTIVE;
    return true;
}

static int check_read_request(BACNET_READ_PROPERTY_DATA *data)
{
    if (data->array_index != BACNET_ARRAY_ALL) {
        data->error_class = ERROR_CLASS_PROPERTY;
        data->error_code = ERROR_CODE_PROPERTY_IS_NOT_AN_ARRAY;
        return BACNET_STATUS_ERROR;
    }
    return 0;
}

int Analog_Value_Read_Property(BACNET_READ_PROPERTY_DATA *data)
{
    BACNET_BIT_STRING flags;
    BACNET_CHARACTER_STRING name;
    int length = 0;

    if (!Analog_Value_Valid_Instance(data->object_instance)) {
        data->error_class = ERROR_CLASS_OBJECT;
        data->error_code = ERROR_CODE_UNKNOWN_OBJECT;
        return BACNET_STATUS_ERROR;
    }
    if (check_read_request(data) < 0) return BACNET_STATUS_ERROR;
    switch (data->object_property) {
        case PROP_OBJECT_IDENTIFIER:
            length = encode_application_object_id(
                data->application_data, OBJECT_ANALOG_VALUE, 1);
            break;
        case PROP_OBJECT_NAME:
            characterstring_init_ansi(&name, "Demo Value");
            length = encode_application_character_string(
                data->application_data, &name);
            break;
        case PROP_OBJECT_TYPE:
            length = encode_application_enumerated(
                data->application_data, OBJECT_ANALOG_VALUE);
            break;
        case PROP_PRESENT_VALUE:
            length = encode_application_real(
                data->application_data, Demo_Value);
            break;
        case PROP_STATUS_FLAGS:
            bitstring_init(&flags);
            length = encode_application_bitstring(
                data->application_data, &flags);
            break;
        case PROP_EVENT_STATE:
            length = encode_application_enumerated(
                data->application_data, EVENT_STATE_NORMAL);
            break;
        case PROP_OUT_OF_SERVICE:
            length = encode_application_boolean(data->application_data, false);
            break;
        case PROP_UNITS:
            length = encode_application_enumerated(
                data->application_data, UNITS_NO_UNITS);
            break;
        default:
            data->error_class = ERROR_CLASS_PROPERTY;
            data->error_code = ERROR_CODE_UNKNOWN_PROPERTY;
            length = BACNET_STATUS_ERROR;
            break;
    }
    return length;
}

bool Analog_Value_Write_Property(BACNET_WRITE_PROPERTY_DATA *data)
{
    BACNET_APPLICATION_DATA_VALUE value = { 0 };
    int length;

    if (!Analog_Value_Valid_Instance(data->object_instance)) {
        data->error_class = ERROR_CLASS_OBJECT;
        data->error_code = ERROR_CODE_UNKNOWN_OBJECT;
        return false;
    }
    if (data->array_index != BACNET_ARRAY_ALL) {
        data->error_class = ERROR_CLASS_PROPERTY;
        data->error_code = ERROR_CODE_PROPERTY_IS_NOT_AN_ARRAY;
        return false;
    }
    if (data->object_property != PROP_PRESENT_VALUE) {
        data->error_class = ERROR_CLASS_PROPERTY;
        data->error_code = ERROR_CODE_WRITE_ACCESS_DENIED;
        return false;
    }
    length = bacapp_decode_application_data(
        data->application_data, data->application_data_len, &value);
    if (length < 0 || value.tag != BACNET_APPLICATION_TAG_REAL) {
        data->error_class = ERROR_CLASS_PROPERTY;
        data->error_code = ERROR_CODE_INVALID_DATA_TYPE;
        return false;
    }
    return Analog_Value_Present_Value_Set(
        data->object_instance, value.type.Real, data->priority);
}

static const char *binary_name(uint32_t instance, bool active)
{
    if (!valid_instance(instance)) return "";
    return active ? "Active" : "Inactive";
}

int Binary_Value_Read_Property(BACNET_READ_PROPERTY_DATA *data)
{
    BACNET_BIT_STRING flags;
    BACNET_CHARACTER_STRING text;
    int length = 0;

    if (!Binary_Value_Valid_Instance(data->object_instance)) {
        data->error_class = ERROR_CLASS_OBJECT;
        data->error_code = ERROR_CODE_UNKNOWN_OBJECT;
        return BACNET_STATUS_ERROR;
    }
    if (check_read_request(data) < 0) return BACNET_STATUS_ERROR;
    switch (data->object_property) {
        case PROP_OBJECT_IDENTIFIER:
            length = encode_application_object_id(
                data->application_data, OBJECT_BINARY_VALUE, 1);
            break;
        case PROP_OBJECT_NAME:
            characterstring_init_ansi(&text, "Demo Enable");
            length = encode_application_character_string(
                data->application_data, &text);
            break;
        case PROP_OBJECT_TYPE:
            length = encode_application_enumerated(
                data->application_data, OBJECT_BINARY_VALUE);
            break;
        case PROP_PRESENT_VALUE:
            length = encode_application_enumerated(
                data->application_data, Binary_Value_Present_Value(1));
            break;
        case PROP_STATUS_FLAGS:
            bitstring_init(&flags);
            length = encode_application_bitstring(
                data->application_data, &flags);
            break;
        case PROP_EVENT_STATE:
            length = encode_application_enumerated(
                data->application_data, EVENT_STATE_NORMAL);
            break;
        case PROP_OUT_OF_SERVICE:
            length = encode_application_boolean(data->application_data, false);
            break;
        case PROP_INACTIVE_TEXT:
        case PROP_ACTIVE_TEXT:
            characterstring_init_ansi(
                &text, binary_name(1, data->object_property == PROP_ACTIVE_TEXT));
            length = encode_application_character_string(
                data->application_data, &text);
            break;
        default:
            data->error_class = ERROR_CLASS_PROPERTY;
            data->error_code = ERROR_CODE_UNKNOWN_PROPERTY;
            length = BACNET_STATUS_ERROR;
            break;
    }
    return length;
}

bool Binary_Value_Write_Property(BACNET_WRITE_PROPERTY_DATA *data)
{
    BACNET_APPLICATION_DATA_VALUE value = { 0 };
    int length;

    if (!Binary_Value_Valid_Instance(data->object_instance)) {
        data->error_class = ERROR_CLASS_OBJECT;
        data->error_code = ERROR_CODE_UNKNOWN_OBJECT;
        return false;
    }
    if (data->array_index != BACNET_ARRAY_ALL) {
        data->error_class = ERROR_CLASS_PROPERTY;
        data->error_code = ERROR_CODE_PROPERTY_IS_NOT_AN_ARRAY;
        return false;
    }
    if (data->object_property != PROP_PRESENT_VALUE) {
        data->error_class = ERROR_CLASS_PROPERTY;
        data->error_code = ERROR_CODE_WRITE_ACCESS_DENIED;
        return false;
    }
    length = bacapp_decode_application_data(
        data->application_data, data->application_data_len, &value);
    if (length < 0 || value.tag != BACNET_APPLICATION_TAG_ENUMERATED ||
        value.type.Enumerated > BINARY_ACTIVE) {
        data->error_class = ERROR_CLASS_PROPERTY;
        data->error_code = ERROR_CODE_INVALID_DATA_TYPE;
        return false;
    }
    return Binary_Value_Present_Value_Set(
        data->object_instance, (BACNET_BINARY_PV)value.type.Enumerated);
}
