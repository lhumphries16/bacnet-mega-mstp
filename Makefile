MCU ?= atmega2560
AVRDUDE_MCU ?= m2560
AVRDUDE_PROGRAMMER ?= wiring
AVRDUDE_BAUD ?= 115200
ifeq ($(OS),Windows_NT)
AVRDUDE_PORT ?= COM5
else
AVRDUDE_PORT ?= /dev/ttyUSB0
endif
TARGET ?= bacnet_mega2560_mstp

VENDOR ?= vendor/bacnet-stack
BACNET_SRC := $(VENDOR)/src
BACNET_CORE := $(BACNET_SRC)/bacnet
BACNET_BASIC := $(BACNET_CORE)/basic
BACNET_PORT := $(VENDOR)/ports/atmega328

CC := avr-gcc
OBJCOPY := avr-objcopy
SIZE := avr-size
AVRDUDE := avrdude

ifeq ($(OS),Windows_NT)
RM := cmd.exe /c del /q
else
RM := rm -f
endif

LOCAL_CSRCS := \
  src/main.c \
  src/demo_objects.c \
  src/device_mega2560.c \
  src/dlmstp_mega2560.c \
  src/rs485_mega2560.c \
  src/timer_mega2560.c

UPSTREAM_PORT_CSRCS := \
  $(BACNET_PORT)/apdu.c \
  $(BACNET_PORT)/eeprom.c \
  $(BACNET_PORT)/h_rp.c \
  $(BACNET_PORT)/h_whois.c \
  $(BACNET_PORT)/h_wp.c \
  $(BACNET_PORT)/nvdata.c

COMMON_CSRCS := \
  $(BACNET_BASIC)/tsm/tsm.c \
  $(BACNET_BASIC)/npdu/h_npdu.c \
  $(BACNET_BASIC)/sys/bigend.c \
  $(BACNET_BASIC)/sys/debug.c \
  $(BACNET_BASIC)/sys/mstimer.c \
  $(BACNET_BASIC)/service/s_iam.c \
  $(BACNET_BASIC)/service/h_noserv.c

CORE_CSRCS := \
  $(BACNET_CORE)/datalink/crc.c \
  $(BACNET_CORE)/npdu.c \
  $(BACNET_CORE)/bacdcode.c \
  $(BACNET_CORE)/bacint.c \
  $(BACNET_CORE)/bacreal.c \
  $(BACNET_CORE)/bacstr.c \
  $(BACNET_CORE)/iam.c \
  $(BACNET_CORE)/rp.c \
  $(BACNET_CORE)/wp.c \
  $(BACNET_CORE)/whois.c \
  $(BACNET_CORE)/bacaddr.c \
  $(BACNET_CORE)/abort.c \
  $(BACNET_CORE)/reject.c \
  $(BACNET_CORE)/bacerror.c \
  $(BACNET_CORE)/bacapp.c

CSRCS := $(LOCAL_CSRCS) $(UPSTREAM_PORT_CSRCS) $(COMMON_CSRCS) $(CORE_CSRCS)
COBJS := $(CSRCS:.c=.o)

INCLUDES := -Isrc -I$(BACNET_PORT) -I$(BACNET_SRC) -I$(BACNET_CORE)
DEFINES := \
  -DBACDL_MSTP \
  -DMAX_APDU=50 \
  -DBIG_ENDIAN=0 \
  -DMAX_TSM_TRANSACTIONS=0 \
  -DBACNET_SVC_SERVER \
  -DBACAPP_REAL \
  -DBACAPP_OBJECT_ID \
  -DBACAPP_UNSIGNED \
  -DBACAPP_ENUMERATED \
  -DBACAPP_CHARACTER_STRING \
  -DBACNET_USE_DOUBLE=0 \
  -DWRITE_PROPERTY \
  -DBACNET_PROTOCOL_REVISION=9 \
  -DF_CPU=16000000UL

CFLAGS := -mmcu=$(MCU) -Os -ffunction-sections -fdata-sections \
  -Wall -Wextra -mcall-prologues -finline-functions-called-once \
  -fsigned-char $(DEFINES) $(INCLUDES) -include src/hardware.h -MD -MP
LDFLAGS := -mmcu=$(MCU) -Wl,--gc-sections,-static -Wl,-Map=$(TARGET).map

all: $(TARGET).hex size

$(TARGET).elf: $(COBJS)
	$(CC) $(COBJS) $(LDFLAGS) -o $@

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

size: $(TARGET).elf
	$(SIZE) $<

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

install: $(TARGET).hex
	$(AVRDUDE) -c $(AVRDUDE_PROGRAMMER) -p $(AVRDUDE_MCU) \
	  -P $(AVRDUDE_PORT) -b $(AVRDUDE_BAUD) -D \
	  -U flash:w:$(TARGET).hex:i

ifeq ($(OS),Windows_NT)
clean:
	-$(RM) $(subst /,\\,$(COBJS)) $(TARGET).elf $(TARGET).hex $(TARGET).map
	-$(RM) $(subst /,\\,$(COBJS:.o=.d))
else
clean:
	-$(RM) $(COBJS) $(TARGET).elf $(TARGET).hex $(TARGET).map
	-$(RM) $(COBJS:.o=.d)
endif

-include $(COBJS:.o=.d)

.PHONY: all size install clean
