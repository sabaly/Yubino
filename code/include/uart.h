#ifndef SERIAL_H
#define SERIAL_H

#define F_CPU 16000000UL

#include <stdint.h>
#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <avr/interrupt.h>

#include "uart.h"
#include "ring_buffer.h"

void UART__putc(uint8_t);
uint8_t UART__getc(void);

void UART__init(void); // initialize UART
void reinit(void);     // reinitialize the buffer to head=tail=0

#endif