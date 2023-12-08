#include "io_command.h"

/* ------------------------------------------ COMMANDS ------------------------------------------ */

void send_command(uint8_t *command, int command_size)
{
    for (int i = 0; i < command_size; ++i)
    {
        UART__putc(command[i]);
    }
}

void recv_command(uint8_t *command, int command_size)
{
    for (int i = 0; i < command_size; ++i)
        command[i] = UART__getc();
}
/* ---------------------------------------------------------------------------------------------- */