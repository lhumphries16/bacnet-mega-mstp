MCU ?= atmega2560
AVRDUDE_MCU ?= m2560
AVRDUDE_PORT ?= COM5
TARGET := ctrl_scout_mega_mstp
VENDOR := ../vendor/bacnet-stack
BACNET_SRC := $(VENDOR)/src
BACNET_CORE := $(BACNET_SRC)/bacnet
BACNET_BASIC := $(BACNET_CORE)/basic
PORT := port

CC := avr-gcc
AR := avr-ar
OBJCOPY := avr-objcopy
SIZE := avr-size
AVRDUDE := avrdude
ifeq ($(OS),Windows_NT)
RM := cmd.exe /c del /q
else
RM := rm -f
endif

LOCAL_CSRCS := main.c timer_mega2560.c rs485_mega2560.c ai_fan_array.c \
  $(PORT)/device.c $(PORT)/av.c $(PORT)/bv.c $(PORT)/h_rp.c $(PORT)/h_wp.c \
  $(PORT)/adc.c $(PORT)/stack.c $(PORT)/nvdata.c $(PORT)/eeprom.c
MSTP_CSRCS := $(PORT)/dlmstp.c
COMMON_CSRCS := $(VENDOR)/ports/atmega328/apdu.c $(VENDOR)/ports/atmega328/h_whois.c \
  $(BACNET_BASIC)/tsm/tsm.c $(BACNET_BASIC)/npdu/h_npdu.c \
  $(BACNET_BASIC)/sys/bigend.c $(BACNET_BASIC)/sys/debug.c \
  $(BACNET_BASIC)/sys/mstimer.c \
  $(BACNET_BASIC)/service/s_iam.c $(BACNET_BASIC)/service/h_noserv.c
CORE_CSRCS := $(BACNET_CORE)/datalink/crc.c $(BACNET_CORE)/npdu.c \
  $(BACNET_CORE)/bacdcode.c $(BACNET_CORE)/bacint.c $(BACNET_CORE)/bacreal.c \
  $(BACNET_CORE)/bacstr.c $(BACNET_CORE)/iam.c $(BACNET_CORE)/rp.c \
  $(BACNET_CORE)/wp.c $(BACNET_CORE)/whois.c $(BACNET_CORE)/bacaddr.c \
  $(BACNET_CORE)/abort.c $(BACNET_CORE)/reject.c $(BACNET_CORE)/bacerror.c \
  $(BACNET_CORE)/bacapp.c

CSRCS := $(LOCAL_CSRCS) $(MSTP_CSRCS) $(COMMON_CSRCS)
COBJS := $(CSRCS:.c=.o)
CORE_OBJS := $(CORE_CSRCS:.c=.o)
INCLUDES := -I. -I$(PORT) -I$(BACNET_SRC) -I$(BACNET_CORE)
DEFINES := -DBACDL_MSTP -DMAX_APDU=50 -DBIG_ENDIAN=0 -DMAX_TSM_TRANSACTIONS=0 \
  -DBACNET_SVC_SERVER -DBACAPP_REAL -DBACAPP_OBJECT_ID -DBACAPP_UNSIGNED \
  -DBACAPP_ENUMERATED -DBACAPP_CHARACTER_STRING -DBACNET_USE_DOUBLE=0 \
  -DWRITE_PROPERTY -DBACNET_PROTOCOL_REVISION=9 -DF_CPU=16000000UL
CFLAGS := -mmcu=$(MCU) -Os -ffunction-sections -fdata-sections -Wall -Wextra \
  -mcall-prologues -finline-functions-called-once -fsigned-char $(DEFINES) \
  $(INCLUDES) -MD -MP
LDFLAGS := -mmcu=$(MCU) -Wl,--gc-sections,-static -Wl,-Map=$(TARGET).map

all: $(TARGET).hex size

$(TARGET).elf: $(COBJS) $(CORE_OBJS)
	$(CC) $(COBJS) $(CORE_OBJS) $(LDFLAGS) -o $@

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

size: $(TARGET).elf
	$(SIZE) $<

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

install: $(TARGET).hex
	$(AVRDUDE) -c wiring -p $(AVRDUDE_MCU) -P $(AVRDUDE_PORT) -U flash:w:$(TARGET).hex

clean:
	-$(RM) $(subst /,\,$(COBJS)) $(subst /,\,$(CORE_OBJS)) $(TARGET).elf $(TARGET).hex $(TARGET).map
	-$(RM) $(subst /,\,$(COBJS:.o=.d)) $(subst /,\,$(CORE_OBJS:.o=.d))

-include $(COBJS:.o=.d) $(CORE_OBJS:.o=.d)
.PHONY: all size install clean
