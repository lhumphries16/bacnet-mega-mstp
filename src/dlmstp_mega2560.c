/*
 * The upstream MS/TP datalink is retained unchanged. This wrapper preloads
 * the local guarded hardware header before the upstream source includes its
 * ATmega328 hardware header by relative name.
 */
#include "hardware.h"
#include "../vendor/bacnet-stack/ports/atmega328/dlmstp.c"
