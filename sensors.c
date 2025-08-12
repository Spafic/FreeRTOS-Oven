#include <stdint.h>
#include "tm4c.h"

void Sensors_Init(void) {
    // Enable clocks for Port E and ADC0
    SYSCTL_RCGCGPIO_R |= (1U << 4);  // Port E
    SYSCTL_RCGCADC_R  |= (1U << 0);  // ADC0
    while ((SYSCTL_PRGPIO_R & (1U << 4)) == 0);
    while ((SYSCTL_PRADC_R & (1U << 0)) == 0);

    // PE3 as analog input
    GPIO_PORTE_AFSEL_R |= (1U << 3);
    GPIO_PORTE_DEN_R   &= ~(1U << 3);
    GPIO_PORTE_AMSEL_R |= (1U << 3);

    // Configure ADC0 SS3
    ADC0_ACTSS_R  &= ~(1U << 3);
    ADC0_EMUX_R   &= ~(0xF << 12);
    ADC0_SSMUX3_R  = 0; // AIN0 (PE3)
    ADC0_SSCTL3_R  = (1U << 1) | (1U << 2); // IE0, END0
    ADC0_ACTSS_R  |= (1U << 3);
}

float Sensors_ReadTemperatureC(void) {
    ADC0_PSSI_R = (1U << 3);
    while ((ADC0_RIS_R & (1U << 3)) == 0);
    uint32_t adcValue = ADC0_SSFIFO3_R & 0xFFF;
    ADC0_ISC_R = (1U << 3);

    // Map 0-4095 ADC to 0–300°C
    return (adcValue / 4095.0f) * 300.0f;
}