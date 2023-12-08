#ifndef UTILS_MACROS_H
#define UTILS_MACROS_H

/* ------------------------------------------ INCLUDES ------------------------------------------ */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>   
#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include <util/atomic.h>
/* ---------------------------------------------------------------------------------------------- */

/* ------------------------------------------ REFERENCES ----------------------------------------- /
 *  Reference to AVR Libc Documentation for utility functions and macros:
 * _delay_ms and F_CPU: https://www.nongnu.org/avr-libc/user-manual/group__util__delay.html
 * _BV and loop_until_bit_is_S/C: https://www.nongnu.org/avr-libc/user-manual/group__avr__sfr.html
 * ---------------------------------------------------------------------------------------------- */

/* ------------------------------------------ CPU CLOCK & BAUD ---------------------------------- */
// Define the CPU clock frequency to 16 MHz if F_CPU isn't specified in Makefile
#ifndef F_CPU
#define F_CPU 16000000UL
#endif

// Compute the UBRR (USART Baud Rate Register) value for setting up the baud rate
#define CALC_UBRR(baud_rate) ((F_CPU / (16UL * (baud_rate))) - 1)
/* ---------------------------------------------------------------------------------------------- */

/* ---------------------------------------- FORMATTING MACROS ----------------------------------- */
#define Bd "\x1b[1m" // ANSI escape code for bold text
#define Rt "\x1b[0m" // ANSI escape code to reset formatting
/* ---------------------------------------------------------------------------------------------- */

/* ---------------------------------------- BITWISE MACROS -------------------------------------- */
// Compound Assignment Bitwise:
#define CA_BOR(a, b) ((a) |= (b))    // OR (a |= b)
#define CA_BNAND(a, b) ((a) &= ~(b)) // NAND (a &= ~b)
#define CA_BXOR(a, b) ((a) ^= (b))   // XOR (a ^= b)

// Let Y, a bitmask be such as uint8_t Y = (_BV(bit) | _BV(bit2) | _BV(bit3) | ...)
// Perform bitwise operations on specified bit(s) (equivalent to 'X OPE Y')
#define CA_BOP(X, operation, ...)                                                 \
    do                                                                            \
    {                                                                             \
        uint8_t Y = 0, bits_to_set[] = {__VA_ARGS__};                             \
        size_t num_bits_to_toggle = sizeof(bits_to_set) / sizeof(bits_to_set[0]); \
        for (size_t i = 0; i < num_bits_to_toggle; ++i)                           \
        {                                                                         \
            CA_BOR((Y), (_BV(bits_to_set[i])));                                   \
        }                                                                         \
        operation((X), (Y));                                                      \
    } while (0)

// Apply bitwise OR to set specified bit(s) (equivalent to 'X |= Y')
#define SET_BITS(X, ...) CA_BOP(X, CA_BOR, __VA_ARGS__)

// Equivalent to X = Y
#define ASSIGN_BITS(X, ...)       \
    do                            \
    {                             \
        (X) = 0;                  \
        SET_BITS(X, __VA_ARGS__); \
    } while (0)

// Apply bitwise NAND to clear specified bit(s) (equivalent to 'X &= ~Y')
#define CLEAR_BITS(X, ...) CA_BOP(X, CA_BNAND, __VA_ARGS__)

// Apply bitwise XOR to toggle specified bit(s) (equivalent to 'X ^= Y')
#define TOGGLE_BITS(X, ...) CA_BOP(X, CA_BXOR, __VA_ARGS__)

/* ---------------------------------------------------------------------------------------------- */

/* ---------------------------------------- PORT/PIN/REG CONTROL -------------------------------- */
#define PORT_DIR(x) (DDR##x)           // Access Data Direction Register for port 'x' (e.g. DDRB)
#define PORT_LVL(x) (PORT##x)          // Access Output Level Register for port 'x' (e.g. PORTB)
#define SET_PIN_DIR(x, n) (DD##x##n)   // Set direction of pin 'n' of port 'x' (e.g. DDB5)
#define SET_PIN_LVL(x, n) (PORT##x##n) // Set output level for pin 'n' of port 'x' (e.g. PORTB5)

// Continuous polling of a register bit until it is set/cleared:
#define WAIT_BIT_SET(reg, bit) loop_until_bit_is_set((reg), (bit))
#define WAIT_BIT_CLR(reg, bit) loop_until_bit_is_clear((reg), (bit))

#define READ_PIN(x, n) ((PIN##x) & _BV(PIN##x##n)) // Check if pin 'n' on port 'x' is high
/* ---------------------------------------------------------------------------------------------- */

/* -------------------------------------- LED STATE MANIPULATION--------------------------------- */
#define TURN_LED_ON(port, bit) SET_BITS((port), (bit))    // Activates the LED
#define TURN_LED_OFF(port, bit) CLEAR_BITS((port), (bit)) // Deactivates the LED
#define TOGGLE_LED(port, bit) TOGGLE_BITS((port), (bit))  // Flips LED state: ON to OFF or OFF to ON
#define DELAY_MS 500U                                     // Delay duration in ms
/* ---------------------------------------------------------------------------------------------- */

#endif /* UTILS_MACROS_H */