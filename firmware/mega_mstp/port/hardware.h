/**
 * @brief This module manages the project hardware defines
 * @author Steve Karg <skarg@users.sourceforge.net>
 * @date 2007
 * @copyright SPDX-License-Identifier: MIT
 */
#ifndef HARDWARE_H
#define HARDWARE_H

#if !defined(F_CPU)
#error Set F_CPU in the Makefile
#endif

/* IAR compiler specific configuration */
#if defined(__IAR_SYSTEMS_ICC__) || defined(__IAR_SYSTEMS_ASM__)
#include <ioavr.h>
#include <iom328p.h>
#endif

/* AVR-GCC compiler specific configuration */
#if defined(__GNUC__)
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/power.h>
#endif

#if !defined(__AVR_ATmega2560__)
#error Firmware is configured for ATmega2560 only (-mmcu=atmega2560)
#endif

#include "iar2gcc.h"
#include "bacnet/basic/sys/bits.h"

#endif
