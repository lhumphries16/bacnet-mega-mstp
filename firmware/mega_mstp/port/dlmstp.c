/*
 * Build the pinned official MS/TP datalink with the Mega hardware header.
 * The upstream source includes its own ATmega328 hardware header by relative
 * path, so this wrapper preloads the local guarded hardware definition before
 * including the unchanged official implementation.
 */
#include "hardware.h"
#include "../../vendor/bacnet-stack/ports/atmega328/dlmstp.c"
