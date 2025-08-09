#include "sensors.h"
#include "tm4c.h" // Tiva C MCU registers

// ADC max value (12-bit)
#define ADC_MAX_VALUE   4095
// Max battery voltage in mV (simulate full battery)
#define BATTERY_FULL_MV 3300

void Sensors_Init(void)
{
    // Enable ADC0 and Port E (PE3 = AIN0 example)
    SYSCTL_RCGCADC_R |= (1U << 0);   // ADC0 clock
    SYSCTL_RCGCGPIO_R |= (1U << 4);  // Port E clock
    while((SYSCTL_PRGPIO_R & (1U << 4)) == 0) {}

    GPIO_PORTE_AFSEL_R |= (1U << 3); // Alternate function PE3
    GPIO_PORTE_DEN_R &= ~(1U << 3);  // Disable digital
    GPIO_PORTE_AMSEL_R |= (1U << 3); // Enable analog

    ADC0_ACTSS_R &= ~(1U << 3);      // Disable SS3
    ADC0_EMUX_R &= ~0xF000;          // Software trigger
    ADC0_SSMUX3_R = 0;               // AIN0 (PE3)
    ADC0_SSCTL3_R = 0x06;            // IE0 & END0
    ADC0_ACTSS_R |= (1U << 3);       // Enable SS3
}

uint16_t Sensors_ReadBatteryRaw(void)
{
    ADC0_PSSI_R = (1U << 3);              // Start SS3
    while((ADC0_RIS_R & (1U << 3)) == 0); // Wait
    uint16_t result = ADC0_SSFIFO3_R & 0xFFF;
    ADC0_ISC_R = (1U << 3);               // Clear flag
    return result;
}

uint8_t Sensors_ReadBatteryPercent(void)
{
    uint16_t raw = Sensors_ReadBatteryRaw();
    uint32_t mv = (raw * BATTERY_FULL_MV) / ADC_MAX_VALUE;
    uint8_t percent = (mv * 100) / BATTERY_FULL_MV;
    if (percent > 100) percent = 100;
    return percent;
}