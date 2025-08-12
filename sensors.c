// Sensors module centralizes all sensor initializations and reads
#include <stdint.h>
#include "tm4c.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "shared_data.h"

void Sensors_Init(void) {
    // Enable clocks for Port E and ADC0
    SYSCTL_RCGCGPIO_R |= (1U << 4);  // Port E
    SYSCTL_RCGCADC_R  |= (1U << 0);  // ADC0
    while ((SYSCTL_PRGPIO_R & (1U << 4)) == 0);
    while ((SYSCTL_PRADC_R & (1U << 0)) == 0);

    // Enable analog on PE1/PE2/PE3 (AIN2/AIN1/AIN0)
    GPIO_PORTE_AFSEL_R |= (1U << 1) | (1U << 2) | (1U << 3);
    GPIO_PORTE_DEN_R   &= ~((1U << 1) | (1U << 2) | (1U << 3));
    GPIO_PORTE_AMSEL_R |= (1U << 1) | (1U << 2) | (1U << 3);

    // Configure ADC0 SS2 to sample 3 channels: AIN0(PE3)->AIN1(PE2)->AIN2(PE1)
    ADC0_ACTSS_R  &= ~(1U << 2);
    ADC0_EMUX_R   &= ~(0xF << 8);  // SS2 processor trigger
    ADC0_SSMUX2_R  = (0 << 0) | (1 << 4) | (2 << 8); // MUX0=AIN0, MUX1=AIN1, MUX2=AIN2
    // SSCTL2: 1st and 2nd samples IE=0, END=0; third sample IE=1, END=1
    ADC0_SSCTL2_R  = (0 << 0) | (0 << 1) | (0 << 2) | (0 << 3)  // sample 0
                   | (0 << 4) | (0 << 5) | (0 << 6) | (0 << 7)  // sample 1
                   | (1 << 10) | (1 << 11);                     // sample 2: IE2=1, END2=1
    ADC0_ACTSS_R  |= (1U << 2);
}

static void adc_acquire_triplet(uint16_t *ain0, uint16_t *ain1, uint16_t *ain2)
{
    if (gADCMutex) xSemaphoreTake(gADCMutex, portMAX_DELAY);
    ADC0_PSSI_R = (1U << 2);
    while ((ADC0_RIS_R & (1U << 2)) == 0) { }
    // Read three samples in order (FIFO is 32-bit; we mask to 12-bit)
    uint32_t s0 = ADC0_SSFIFO2_R & 0xFFF;
    uint32_t s1 = ADC0_SSFIFO2_R & 0xFFF;
    uint32_t s2 = ADC0_SSFIFO2_R & 0xFFF;
    ADC0_ISC_R = (1U << 2);
    if (gADCMutex) xSemaphoreGive(gADCMutex);
    if (ain0) *ain0 = (uint16_t)s0;
    if (ain1) *ain1 = (uint16_t)s1;
    if (ain2) *ain2 = (uint16_t)s2;
}

float Sensors_ReadTemperatureC(void) {
    uint16_t a0; adc_acquire_triplet(&a0, NULL, NULL);
    return (a0 / 4095.0f) * 300.0f;
}

uint8_t Sensors_ReadBatteryPercent(void) {
    uint16_t a1; adc_acquire_triplet(NULL, &a1, NULL);
    uint32_t pct = (a1 * 100U) / 4095U;
    if (pct > 100U) pct = 100U;
    return (uint8_t)pct;
}

uint16_t Sensors_ReadDoorLightRaw(void) {
    uint16_t a2; adc_acquire_triplet(NULL, NULL, &a2);
    return a2;
}