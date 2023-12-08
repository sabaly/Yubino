#include "hardware_config.h"

/* ------------------------------------------- VARIABLES ---------------------------------------- */
// LED Control Variables
volatile uint8_t timeout = TMT; // Timeout duration: 20 cycles, equivalent to 10 s (0.5s per cycle)

// Button Debouncing Variables
volatile uint8_t button_state = 0xFF;    // 0xFF represents an unpressed state for all pins
volatile uint8_t counter = 0;            // Counter for debounce timing
volatile uint8_t button_pushed_flag = 0; // Flag indicating button press after debouncing
/* ---------------------------------------------------------------------------------------------- */

/* ------------------------------------------- INTERRUPTS ---------------------------------------- */
// Watchdog Timer interrupt for handling switch bounce
ISR(WDT_vect)
{
    debounce(); // Call debounce function every 16ms
}

// Timer1 Compare Match A interrupt for LED toggling and potentiometer enabling
ISR(TIMER1_COMPA_vect)
{

    TOGGLE_LED(PORT_LVL(D), SET_PIN_LVL(D, 6)); // Toggle LED connected to pin D6
    if (timeout)
        timeout--; // Decrement timeout, if active
}
/* ---------------------------------------------------------------------------------------------- */

/* ------------------------------------------- LED ---------------------------------------------- */
void init_LED_blink(void)
{
    CLEAR_BITS(PRR, PRTIM1);          // Enable Timer1 (disable power reduction)
    TCCR1A = 0;                       // Reset Timer1A configuration
    OCR1A = 31249;                    // Set compare match value for 0.5s interval at /256 prescaler
    ASSIGN_BITS(TCCR1B, WGM12, CS12); // Configure Timer1: CTC mode, /256 prescaler
    SET_BITS(TIMSK1, OCIE1A);         // Enable Timer1 Compare Match A interrupt
}
/* ---------------------------------------------------------------------------------------------- */

/* ------------------------------------------- BUTTON ------------------------------------------- */
void debounce(void)
{
    uint8_t current_button_state = READ_PIN(B, 0); // Read the current button state

    if (current_button_state != button_state) // Check for state change
    {
        counter++; // Increment debounce counter

        if (counter >= 4) // Confirm stable state change after 4 cycles
        {
            button_state = current_button_state; // Update button state
            counter = 0;                         // Reset counter

            if (button_state == 0)
                button_pushed_flag = 1; // Set flag if button is pressed
        }
    }
    else
        counter = 0; // Reset counter if no state change
}

int is_button_pushed(void)
{
    // Return 1 if button has been pushed and reset the flag, return 0 otherwise
    int result = (button_pushed_flag) ? (button_pushed_flag = 0, 1) : 0;
    return result;
}

void init_button(void)
{
    // Note: PRR stays at 0xFF (does not affect the Watchdog Timer, which is controlled separately)

    CLEAR_BITS(PORT_DIR(B), SET_PIN_DIR(B, 0)); // Set PB0 as input
    SET_BITS(PORT_LVL(B), SET_PIN_LVL(B, 0));   // Enable pull-up resistor on PB0

    // Prevent unintended system resets by disabling the WDT (Watchdog Timer) at startup
    CLEAR_BITS(MCUSR, WDRF); // Clear the Watchdog Reset Flag in the MCU Status Register
    CLEAR_BITS(WDTCSR, WDE); // Disable the Watchdog System Reset in the WDT Control Register

    // WDCE: WD Change Enable (for changing WDE and prescaler), WDE: WD System Reset Enable
    SET_BITS(WDTCSR, WDCE, WDE); // Enable configuration changes to WDT's settings

    // Configure WDT 16ms default timeout (WDP3=0,WDP2=0,WDP1=0,WDP0=0) with interrupt mode
    ASSIGN_BITS(WDTCSR, WDIE); // Enable Watchdog Interrupt without changing the default timeout
}
/* ---------------------------------------------------------------------------------------------- */

/* ------------------------------------------- HARDWARE INTERACTION ----------------------------- */
void init_Hardware(void)
{
    cli();                     // Disable global interrupts
    CLEAR_BITS(ADCSRA, ADEN);  // Disable ADC to save power
    PRR = 0xFF;                // Power down all peripherals initially
    CLEAR_BITS(PRR, PRUSART0); // Power up USART0 by clearing PRUSART0 in PRR

    SET_BITS(PORT_DIR(D), SET_PIN_DIR(D, 6));     // Set LED pin (D6) as output
    TURN_LED_OFF(PORT_LVL(D), SET_PIN_LVL(D, 6)); // Turn off LED initially
}

void reset_config(void)
{
    CLEAR_BITS(TIMSK1, OCIE1A);                  // Disable Timer1 Compare Match A interrupt (for LED)
    CLEAR_BITS(WDTCSR, WDIE);                    // Disable Watchdog Interrupt (for button)
    OCR1A = 0;                                   // Reset Timer1 compare match register
    CLEAR_BITS(TCCR1B, WGM12, CS12, CS11, CS10); // Clear Timer1 configuration (CTC mode, prescaler)
    SET_BITS(PRR, PRTIM1, PRADC);                // Power down Timer1 and ADC for energy efficiency
}

int setup_sleep_WDT_Timer1(void)
{
    // No SLEEP_MODE_PWR_DOWN because it disables Timer1 (required for LED blinking)
    set_sleep_mode(SLEEP_MODE_IDLE); // Set MCU to idle sleep mode

    while (1)
    {
        sleep_mode(); // Enter sleep mode

        // Exit loop if button is pressed or timeout occurs
        if (is_button_pushed() || !timeout)
        {
            TURN_LED_OFF(PORT_LVL(D), SET_PIN_LVL(D, 6)); // Turn off LED
            break;                                        // Exit loop
        }
    }

    reset_config();      // Reset configuration to default
    return EXIT_SUCCESS; // Return successful completion
}

int blinkLedUntilTimeoutorButton(void)
{
    cli();            // Disable global interrupts
    init_LED_blink(); // Initialize LED for blinking
    init_button();    // Set up button
    sei();            // Enable global interrupts

    return setup_sleep_WDT_Timer1(); // Configure sleep mode
}

int get_alea_from_Timer0(uint8_t *dest, unsigned size)
{
    cli();                     // Disable global interrupts
    CLEAR_BITS(PRR, PRTIM0);   // Enable Timer0 by disabling power reduction
    ASSIGN_BITS(TCCR0B, CS00); // Configure Timer0: /1 prescaler (no prescaling)
    // The OCIE0A is used to enable an interrupt that occurs when the TCNT0 matches the value in the OCR0A register
    // So we dont need it here (we only read the TCNT0 register)

    sei();       // Enable global interrupts
    while (size) // Loop until desired size is reached
    {
        *dest = TCNT0; // Store Timer0 value in destination
        ++dest;        // Move to next byte location
        --size;        // Increment counter
    }

    SET_BITS(PRR, PRTIM0); // Disable Timer0 by enabling power reduction

    return 1; // Return 1 to indicate success (for uECC lib)
}
/* ---------------------------------------------------------------------------------------------- */