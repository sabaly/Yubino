#include "authenticator.h"

/* ------------------------------------------- AUTHENTICATOR ------------------------------------ */
int get_consent(void)
{
#if (!TEST)
    blinkLedUntilTimeoutorButton();
    if (!timeout)
    {
        timeout = TMT;
        return 0; // Refusal
    }
    timeout = TMT;
#endif
    return 1; // Consent
}

static int search_app_in_eeprom(uint8_t *app_id)
{
    uint8_t nbApps;
    int address = 0;
    uint8_t tmp_id[20] = {0};
    nbApps = eeprom_read_byte((uint8_t *)0); // Get number of apps from EEPROM
    if (nbApps == 0)
        return address;
    for (int i = 1; i < nbApps * 57 + 1; i += 57)
    {
        eeprom_read_block((void *)tmp_id, (void *)i, 20); // Read app_id from EEPROM to tmp_id
        if (memcmp(tmp_id, app_id, 20) == 0)              // Compare app_id with the one in EEPROM
        {
            address = i;
            break;
        }
    }
    return address; // Return address of app_id in EEPROM (0 if not exist)
}

int make_credentials(uint8_t *array)
{
    uint8_t nbApps;
    uint8_t public_key[40], private_key[21];

    int add = search_app_in_eeprom(array);
    if (add) // If app_id already exists in EEPROM
    {
        // Read credential_id from EEPROM to array
        eeprom_read_block((void *)array, (void *)(add + 20), 16);
        return STATUS_OK;
    }
    nbApps = eeprom_read_byte((uint8_t *)0); // Get number of apps from EEPROM
    if (nbApps >= 17)
        return STATUS_ERR_STORAGE_FULL;
    if (!get_consent())
        return STATUS_ERR_APPROVAL;
    eeprom_update_block((void *)array, (void *)(nbApps * 57 + 1), 20); // Write app_id from array to EEPROM

    if (!uECC_make_key(public_key, private_key, uECC_secp160r1()))
    {
        return 1;
    }

    get_alea_from_Timer0(array, 16);
    eeprom_update_block((void *)array, (void *)(nbApps * 57 + 1 + 20), 16);       // Write credential_id to EEPROM
    eeprom_update_block((void *)private_key, (void *)(nbApps * 57 + 1 + 36), 21); // Write private_key to EEPROM

    eeprom_update_byte((uint8_t *)0, nbApps + 1); // Update number of apps in EEPROM

    memcpy(array + 16, public_key, 40); // Write public_key to array
    return 0;
}

int get_assertion(uint8_t *array)
{
    uint8_t private_key[21], signature[40], client_hash_data[20];
    int add = search_app_in_eeprom(array);
    if (!add)
        return STATUS_ERR_NOT_FOUND;
    if (!get_consent())
        return STATUS_ERR_APPROVAL;

    eeprom_read_block((void *)private_key, (void *)(add + 36), 21);
    memcpy(client_hash_data, array + 20, 20); // Get client_hash_data from array

    if (!uECC_sign(private_key, client_hash_data, 20, signature, uECC_secp160r1()))
        return STATUS_ERR_CRYPTO_FAILED;

    eeprom_read_block((void *)array, (void *)(add + 20), 16); // Read credential_id from EEPROM to array
    memcpy(array + 16, signature, 40);                        // Write signature to array
    return STATUS_OK;
}
/* ---------------------------------------------------------------------------------------------- */