#ifndef AUTHENTICATOR__H
#define AUTHENTICATOR__H

#include <hardware_config.h>
#include <avr/eeprom.h>
#include "../lib/micro-ecc/uECC.h"
#include "io_command.h"

/* ------------------------------------------- TEST MODE ---------------------------------------- */
#ifndef TEST   // If MODE is not defined (with Makefile), set it to 0 by default
#define TEST 0 // No test
#endif
/* ---------------------------------------------------------------------------------------------- */

/* ------------------------------------------- AUTHENTICATOR ------------------------------------ */
int get_consent(void);
int make_credentials(uint8_t *array);
int get_assertion(uint8_t *array);
/* ---------------------------------------------------------------------------------------------- */

#endif /* AUTHENTICATOR__H */