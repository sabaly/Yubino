#include <avr/sfr_defs.h>
#include <avr/interrupt.h>

#include "uart.h"
#include "ring_buffer.h"

#define RX_BUFFER_SIZE 41
uint8_t rx_raw_buffer[RX_BUFFER_SIZE];
struct ring_buffer rx_buffer;

void UART__init()
{
#define BAUD 115200
#define BAUD_TOL 3
#include <util/setbaud.h>
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;
#if USE_2X
    UCSR0A |= (1 << U2X0);
#else
    UCSR0A &= ~(1 << U2X0);
#endif
    // enable RX, TX for UART0
    UCSR0B = _BV(TXEN0) | _BV(RXEN0) | _BV(RXCIE0);
    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); // 8 bit data format

    ring_buffer__init(&rx_buffer, rx_raw_buffer, RX_BUFFER_SIZE);
}

uint8_t
UART__getc()
{
    uint8_t data;
    while (ring_buffer__pop(&rx_buffer, &data) == 1)
        ;
    return data;
}

void UART__putc(uint8_t c)
{
    loop_until_bit_is_set(UCSR0A, UDRE0);
    UDR0 = c;
}

ISR(USART_RX_vect)
{
    ring_buffer__push(&rx_buffer, UDR0);
}

void reinit()
{
    rx_buffer.head = 0;
    rx_buffer.tail = 0;
}