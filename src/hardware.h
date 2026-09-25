/**
 * @brief ATmega2560 hardware guard shared by the local target and upstream
 *        AVR source files compiled by the Makefile.
 * @copyright SPDX-License-Identifier: MIT
 */
#ifndef HARDWARE_H
#define HARDWARE_H

#if !defined(F_CPU)
#error Set F_CPU in the Makefile
#endif

#if !defined(__AVR_ATmega2560__)
#error This reference requires -mmcu=atmega2560
#endif

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include "../vendor/bacnet-stack/ports/atmega328/iar2gcc.h"
#include "bacnet/basic/sys/bits.h"

#endif
