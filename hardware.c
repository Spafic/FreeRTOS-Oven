#include "hardware.h"
#include "tm4c.h"

// PF1 -> Buzzer
#define BUZZER_PIN   (1U << 1)

// PF3 -> Green LED
#define TEMP_LED_PIN (1U << 3)

#define DEBUG_LED_PIN (1U << 2) // PF2

#define DOOR_LED_PIN (1U << 4) // PF4


void Hardware_Init(void)
{
    // Enable Port F clock
    SYSCTL_RCGCGPIO_R |= (1U << 5);
    while ((SYSCTL_PRGPIO_R & (1U << 5)) == 0) {}

    // PF1 (buzzer), PF2 (debug), PF3 (green LED) as outputs
    GPIO_PORTF_DIR_R |= (BUZZER_PIN | TEMP_LED_PIN | DEBUG_LED_PIN | DOOR_LED_PIN);
    GPIO_PORTF_DEN_R |= (BUZZER_PIN | TEMP_LED_PIN | DEBUG_LED_PIN | DOOR_LED_PIN);

    GPIO_PORTF_DATA_R &= ~(BUZZER_PIN | TEMP_LED_PIN | DEBUG_LED_PIN | DOOR_LED_PIN);
}

// -------------------- BUZZER --------------------
void Hardware_BuzzerOn(void)
{
    GPIO_PORTF_DATA_R |= BUZZER_PIN;
}

void Hardware_BuzzerOff(void)
{
    GPIO_PORTF_DATA_R &= ~BUZZER_PIN;
}

// -------------------- BATTERY DISPLAY --------------------
void Hardware_DisplayBattery(uint8_t percent)
{
    // Stub: connect to LCD later
    (void)percent;
}

// -------------------- TEMP LED --------------------
void Hardware_TempLedOn(void)
{
    GPIO_PORTF_DATA_R |= TEMP_LED_PIN;
}

void Hardware_TempLedOff(void)
{
    GPIO_PORTF_DATA_R &= ~TEMP_LED_PIN;
}

// -------------------- TEMPERATURE DISPLAY --------------------
void Hardware_DisplayTemperature(int16_t temp_c)
{
    // Stub: connect to LCD later
    (void)temp_c;
}

void Hardware_DebugLedToggle(void)
{
    GPIO_PORTF_DATA_R ^= DEBUG_LED_PIN;
}

void Hardware_DoorLedOn(void)
{
    GPIO_PORTF_DATA_R |= DOOR_LED_PIN;
}

void Hardware_DoorLedOff(void)
{
    GPIO_PORTF_DATA_R &= ~DOOR_LED_PIN;
}