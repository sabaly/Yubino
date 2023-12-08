#ifndef HARDWARE_CONFIG
#define HARDWARE_CONFIG

#include "utils_macros.h"

/* ------------------------------------------- VARIABLES ---------------------------------------- */
#define TMT 20
extern volatile uint8_t timeout; 
/* ---------------------------------------------------------------------------------------------- */

/* ------------------------------------------- LED ---------------------------------------------- */
void init_LED_blink(void);
/* ---------------------------------------------------------------------------------------------- */

/* ------------------------------------------- BUTTON ------------------------------------------- */
void debounce(void);
int is_button_pushed(void);
void init_button(void);
/* ---------------------------------------------------------------------------------------------- */

/* ------------------------------------------- HARDWARE INTERACTION ----------------------------- */
void init_Hardware(void);
void reset_config(void);
int setup_sleep_WDT_Timer1(void);
int blinkLedUntilTimeoutorButton(void);
int get_alea_from_Timer0(uint8_t *dest, unsigned size);
/* ---------------------------------------------------------------------------------------------- */

#endif /* HARDWARE_CONFIG */