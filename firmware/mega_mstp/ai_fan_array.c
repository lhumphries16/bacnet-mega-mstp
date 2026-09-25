/* Small read-only AI object implementation for the three INA3221 channels. */
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "bacnet/bacdef.h"
#include "bacnet/bacdcode.h"
#include "bacnet/basic/object/ai.h"

static float Values[3];
static const char *Names[3] = { "Fan 1 Current", "Fan 2 Current", "Fan 3 Current" };
static BACNET_ENGINEERING_UNITS Units[3] = { UNITS_AMPERES, UNITS_AMPERES, UNITS_AMPERES };

static int Index(uint32_t instance) { return instance >= 1 && instance <= 3 ? (int)instance - 1 : -1; }
void Analog_Input_Init(void) { memset(Values, 0, sizeof(Values)); }
unsigned Analog_Input_Count(void) { return 3; }
uint32_t Analog_Input_Index_To_Instance(unsigned index) { return index < 3 ? index + 1 : BACNET_MAX_INSTANCE; }
unsigned Analog_Input_Instance_To_Index(uint32_t instance) { return Index(instance) < 0 ? 3 : (unsigned)Index(instance); }
bool Analog_Input_Valid_Instance(uint32_t instance) { return Index(instance) >= 0; }
bool Analog_Input_Object_Instance_Add(uint32_t instance) { return Analog_Input_Valid_Instance(instance); }
bool Analog_Input_Name_Set(uint32_t instance, const char *name) { int i = Index(instance); (void)name; return i >= 0; }
const char *Analog_Input_Name_ASCII(uint32_t instance) { int i = Index(instance); return i >= 0 ? Names[i] : "AI-X"; }
bool Analog_Input_Object_Name(uint32_t instance, BACNET_CHARACTER_STRING *name) { return characterstring_init_ansi(name, Analog_Input_Name_ASCII(instance)); }
bool Analog_Input_Units_Set(uint32_t instance, BACNET_ENGINEERING_UNITS units) { int i = Index(instance); if (i < 0) return false; Units[i] = units; return true; }
BACNET_ENGINEERING_UNITS Analog_Input_Units(uint32_t instance) { int i = Index(instance); return i >= 0 ? Units[i] : UNITS_NO_UNITS; }
float Analog_Input_Present_Value(uint32_t instance) { int i = Index(instance); return i >= 0 ? Values[i] : 0.0f; }
void Analog_Input_Present_Value_Set(uint32_t instance, float value) { int i = Index(instance); if (i >= 0) Values[i] = value; }

int Analog_Input_Read_Property(BACNET_READ_PROPERTY_DATA *rpdata)
{
    int length = 0; BACNET_BIT_STRING flags; BACNET_CHARACTER_STRING name;
    if (!Analog_Input_Valid_Instance(rpdata->object_instance)) return BACNET_STATUS_ERROR;
    switch (rpdata->object_property) {
        case PROP_OBJECT_IDENTIFIER: length = encode_application_object_id(rpdata->application_data, OBJECT_ANALOG_INPUT, rpdata->object_instance); break;
        case PROP_OBJECT_NAME: characterstring_init_ansi(&name, Analog_Input_Name_ASCII(rpdata->object_instance)); length = encode_application_character_string(rpdata->application_data, &name); break;
        case PROP_OBJECT_TYPE: length = encode_application_enumerated(rpdata->application_data, OBJECT_ANALOG_INPUT); break;
        case PROP_PRESENT_VALUE: length = encode_application_real(rpdata->application_data, Analog_Input_Present_Value(rpdata->object_instance)); break;
        case PROP_STATUS_FLAGS: bitstring_init(&flags); bitstring_set_bit(&flags, STATUS_FLAG_IN_ALARM, false); bitstring_set_bit(&flags, STATUS_FLAG_FAULT, false); bitstring_set_bit(&flags, STATUS_FLAG_OVERRIDDEN, false); bitstring_set_bit(&flags, STATUS_FLAG_OUT_OF_SERVICE, false); length = encode_application_bitstring(rpdata->application_data, &flags); break;
        case PROP_EVENT_STATE: length = encode_application_enumerated(rpdata->application_data, EVENT_STATE_NORMAL); break;
        case PROP_OUT_OF_SERVICE: length = encode_application_boolean(rpdata->application_data, false); break;
        case PROP_UNITS: length = encode_application_enumerated(rpdata->application_data, Analog_Input_Units(rpdata->object_instance)); break;
        default: rpdata->error_class = ERROR_CLASS_PROPERTY; rpdata->error_code = ERROR_CODE_UNKNOWN_PROPERTY; length = BACNET_STATUS_ERROR; break;
    }
    return length;
}

bool Analog_Input_Write_Property(BACNET_WRITE_PROPERTY_DATA *wp_data)
{
    wp_data->error_class = ERROR_CLASS_PROPERTY;
    wp_data->error_code = ERROR_CODE_WRITE_ACCESS_DENIED;
    return false;
}
