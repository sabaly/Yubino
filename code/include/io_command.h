#ifndef IO_COMMAND_H
#define IO_COMMAND_H

#include "uart.h"

/* -------------------------------- CONSTANTS (from the subject) -------------------------------- */
#define COMMAND_LIST_CREDENTIALS 0
#define COMMAND_MAKE_CREDENTIAL 1
#define COMMAND_GET_ASSERTION 2
#define COMMAND_RESET 3
#define STATUS_OK 0
#define STATUS_ERR_COMMAND_UNKNOWN 1
#define STATUS_ERR_CRYPTO_FAILED 2
#define STATUS_ERR_BAD_PARAMETER 3
#define STATUS_ERR_NOT_FOUND 4
#define STATUS_ERR_STORAGE_FULL 5
#define STATUS_ERR_APPROVAL 6
/* ---------------------------------------------------------------------------------------------- */

/* ------------------------------------------ COMMANDS ------------------------------------------ */
void send_command(uint8_t *command, int command_size);
void recv_command(uint8_t *command, int command_size);
/* ---------------------------------------------------------------------------------------------- */

#endif /* IO_COMMAND_H */