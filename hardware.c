#include "hardware.h"
#include "tm4c.h"

// Assume buzzer on PF1 (you can change pin)
#define BUZZER_PORT GPIO_PORTF_BASE
#define BUZZER_PIN  (1U << 1)

void Hardware_Init(void)
{
    // Enable Port F for buzzer
    SYSCTL_RCGCGPIO_R |= (1U << 5); // Port F clock
    while((SYSCTL_PRGPIO_R & (1U << 5)) == 0) {}

    GPIO_PORTF_DIR_R |= BUZZER_PIN;   // PF1 output
    GPIO_PORTF_DEN_R |= BUZZER_PIN;   // Enable digital
    GPIO_PORTF_DATA_R &= ~BUZZER_PIN; // Buzzer off
}

void Hardware_BuzzerOn(void)
{
    GPIO_PORTF_DATA_R |= BUZZER_PIN;
}

void Hardware_BuzzerOff(void)
{
    GPIO_PORTF_DATA_R &= ~BUZZER_PIN;
}

void Hardware_DisplayBattery(uint8_t percent)
{
    // For now, do nothing (hook to LCD later)
    (void)percent;
}