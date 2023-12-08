#include "authenticator.h"

/* --------------------------------------------- MAIN ------------------------------------------- */
int main(void)
{
    uint8_t type, status, count;
    uint8_t array[56] = {0};

    eeprom_update_byte((uint8_t *)0, 0); // Initialize number of apps in EEPROM to 0
    uECC_set_rng(&get_alea_from_Timer0); // Set RNG function for micro-ecc

    UART__init();
    init_Hardware();
    sei();

    while (1)
    {
        type = UART__getc();
        switch (type)
        {
        case COMMAND_LIST_CREDENTIALS:
            if (UCSR0A & (1 << RXC0))
                UART__putc(STATUS_ERR_BAD_PARAMETER);
            else
            {
                count = eeprom_read_byte((uint8_t *)0);
                UART__putc(STATUS_OK);
                UART__putc(count);
                for (int i = 0; i < count; i++)
                {
                    eeprom_read_block((void *)array, (void *)(i * 57 + 1 + 20), 16); // Read credential_id from EEPROM to array
                    send_command(array, 16);                                         // Send credential_id to Yubino
                    eeprom_read_block((void *)array, (void *)(i * 57 + 1), 20);      // Read app_id from EEPROM to array
                    send_command(array, 20);                                         // Send app_id to Yubino
                }
            }
            break;
        case COMMAND_MAKE_CREDENTIAL:
            recv_command(array, 20);
            if (UCSR0A & (1 << RXC0))
                UART__putc(STATUS_ERR_BAD_PARAMETER);
            else
            {
                status = make_credentials(array);
                UART__putc(status);
                if (!status)
                    send_command(array, 56); // Send to Yubino
            }
            break;
        case COMMAND_GET_ASSERTION:
            recv_command(array, 40);
            if (UCSR0A & (1 << RXC0))
                UART__putc(STATUS_ERR_BAD_PARAMETER);
            else
            {
                status = get_assertion(array);
                UART__putc(status);
                if (!status)
                    send_command(array, 56);
            }
            break;
        case COMMAND_RESET:
            if (!get_consent())
                UART__putc(STATUS_ERR_APPROVAL);
            else
            {
                if (UCSR0A & (1 << RXC0))
                    UART__putc(STATUS_ERR_BAD_PARAMETER);
                else
                {
                    count = eeprom_read_byte((uint8_t *)0);
                    for (int i = 0; i < 56; i++)
                        array[i] = 0;
                    for (int i = 0; i < count; i++) // Set all bytes in EEPROM to 0
                    {
                        eeprom_update_block((void *)(i * 57 + 1), (void *)array, 20);
                        eeprom_update_block((void *)(i * 57 + 1 + 20), (void *)array, 16);
                        eeprom_update_block((void *)(i * 57 + 1 + 36), (void *)array, 21);
                    }
                    eeprom_update_byte((uint8_t *)0, 0); // Update number of apps in EEPROM to 0
                    UART__putc(STATUS_OK);
                }
            }
            break;
        default:
            UART__putc(STATUS_ERR_COMMAND_UNKNOWN);
            reinit();
            break;
        }
    }

    return 0;
}
/* ---------------------------------------------------------------------------------------------- */