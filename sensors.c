#include "sensors.h"
#include "tm4c.h" // Tiva C MCU registers

// ADC max value (12-bit)
#define ADC_MAX_VALUE   4095
// Max battery voltage in mV (simulate full battery)
#define BATTERY_FULL_MV 3300

#define ADC_TEMP_MAX_VALUE 4095
#define TEMP_SENSOR_MV_PER_C 10   // Example: 10 mV per °C (adjust as per sensor datasheet)
#define TEMP_SENSOR_OFFSET_C 0    // Adjust if sensor has offset

void Sensors_Init(void)
{
    // Enable ADC0 and Port E clock
    SYSCTL_RCGCADC_R |= (1U << 0);   // ADC0 clock
    SYSCTL_RCGCGPIO_R |= (1U << 4);  // Port E clock
    while((SYSCTL_PRGPIO_R & (1U << 4)) == 0) {}
    while((SYSCTL_PRADC_R  & (1U << 0)) == 0) {} // wait ADC ready

    // Configure PE2 (AIN1) and PE3 (AIN0) as analog inputs
    GPIO_PORTE_AFSEL_R |= (1U << 2) | (1U << 3);   // AF on PE2, PE3
    GPIO_PORTE_DEN_R &= ~((1U << 2) | (1U << 3));  // disable digital
    GPIO_PORTE_AMSEL_R |= (1U << 2) | (1U << 3);   // enable analog

    // Configure ADC0 SS3 (use software trigger)
    ADC0_ACTSS_R &= ~(1U << 3);      // disable SS3
    ADC0_EMUX_R &= ~0xF000;          // software trigger for SS3
    ADC0_SSMUX3_R = 0;               // default to AIN0 (PE3)
    ADC0_SSCTL3_R = 0x06;            // IE0 & END0
    ADC0_ACTSS_R |= (1U << 3);       // enable SS3
}


uint16_t Sensors_ReadBatteryRaw(void)
{
    // Select AIN0 (PE3) before conversion
    ADC0_ACTSS_R &= ~(1U << 3);   // disable SS3
    ADC0_SSMUX3_R = 0;            // channel 0 = AIN0
    ADC0_ACTSS_R |= (1U << 3);    // enable SS3

    // Start conversion
    ADC0_PSSI_R = (1U << 3);
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

int16_t Sensors_ReadTemperatureC(void)
{
    // Select AIN1 (PE2)
    ADC0_ACTSS_R &= ~(1U << 3);   // disable SS3
    ADC0_SSMUX3_R = 1;            // AIN1
    ADC0_ACTSS_R |= (1U << 3);    // enable SS3

    ADC0_PSSI_R = (1U << 3);      // start SS3
    while ((ADC0_RIS_R & (1U << 3)) == 0);
    uint16_t raw = ADC0_SSFIFO3_R & 0xFFF;
    ADC0_ISC_R = (1U << 3);       // clear flag

    uint32_t mv = (raw * 3300U) / ADC_MAX_VALUE;
    int16_t temp_c = (mv / TEMP_SENSOR_MV_PER_C) + TEMP_SENSOR_OFFSET_C;
    return temp_c;
}

uint16_t Sensors_ReadDoorPot(void)
{
    ADC0_ACTSS_R &= ~(1U << 3);
    ADC0_SSMUX3_R = 2; // AIN2 for example (PE1 or another free pin)
    ADC0_ACTSS_R |= (1U << 3);

    ADC0_PSSI_R = (1U << 3);
    while ((ADC0_RIS_R & (1U << 3)) == 0);
    uint16_t result = ADC0_SSFIFO3_R & 0xFFF;
    ADC0_ISC_R = (1U << 3);
    return result;
}