/* 
 * adc_control.c
 * ADC Control Functions for Industrial Oven Control System
 */

#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "adc_control.h"

/* ADC Scaling Constants */
#define ADC_MAX_VALUE             4095      // 12-bit ADC
#define TEMPERATURE_MIN_C         0.0f      // Minimum temperature in °C
#define TEMPERATURE_MAX_C         100.0f    // Maximum temperature in °C

/* 
 * Initialize ADC for temperature, light and battery sensors
 */
void InitADC(void) {
    // Enable ADC0 clock
    SYSCTL_RCGCADC_R |= SYSCTL_RCGCADC_R0;
    
    // Wait for ADC0 to be ready
    while ((SYSCTL_PRADC_R & SYSCTL_PRADC_R0) != SYSCTL_PRADC_R0) {}
    
    // Disable ADC0 sequencers during configuration
    ADC0_ACTSS_R &= ~(ADC_ACTSS_ASEN0 | ADC_ACTSS_ASEN1 | ADC_ACTSS_ASEN2);
    
    // Configure ADC0 sample sequencer priorities
    // SS0 - highest priority (temperature)
    // SS1 - medium priority (light)
    // SS2 - lowest priority (battery)
    ADC0_SSPRI_R = 0x0123;
    
    // Configure Sequencer 0 for temperature (PE3)
    ADC0_SSMUX0_R = 0;  // Channel 0 (PE3)
    ADC0_SSCTL0_R = ADC_SSCTL0_IE0 | ADC_SSCTL0_END0; // Generate interrupt, end of sequence
    
    // Configure Sequencer 1 for light intensity (PE2)
    ADC0_SSMUX1_R = 1;  // Channel 1 (PE2)
    ADC0_SSCTL1_R = ADC_SSCTL1_IE0 | ADC_SSCTL1_END0; // Generate interrupt, end of sequence
    
    // Configure Sequencer 2 for battery level (PE1)
    ADC0_SSMUX2_R = 2;  // Channel 2 (PE1)
    ADC0_SSCTL2_R = ADC_SSCTL2_IE0 | ADC_SSCTL2_END0; // Generate interrupt, end of sequence
    
    // Configure ADC0 sample averaging
    // Use hardware averaging to improve accuracy
    ADC0_SAC_R = ADC_SAC_AVG_8X;  // 8x hardware averaging
    
    // Configure ADC0 sample rate
    // Use maximum sample rate
    ADC0_PC_R = ADC_PC_SR_1M;  // 1MSps
    
    // Enable sequencers
    ADC0_ACTSS_R |= (ADC_ACTSS_ASEN0 | ADC_ACTSS_ASEN1 | ADC_ACTSS_ASEN2);
}

/*
 * Read temperature from ADC channel 0 (PE3)
 * Returns: Temperature in degrees Celsius
 */
float ReadTemperatureSensor(void) {
    uint32_t adcValue;
    
    // Start ADC conversion
    ADC0_PSSI_R = ADC_PSSI_SS0;
    
    // Wait for conversion to complete
    while (!(ADC0_RIS_R & ADC_RIS_INR0)) {}
    
    // Read ADC value
    adcValue = ADC0_SSFIFO0_R;
    
    // Clear the interrupt flag
    ADC0_ISC_R = ADC_ISC_IN0;
    
    // Convert ADC value to temperature
    return ((float)adcValue / ADC_MAX_VALUE) * (TEMPERATURE_MAX_C - TEMPERATURE_MIN_C) + TEMPERATURE_MIN_C;
}

/*
 * Read light intensity from ADC channel 1 (PE2)
 * Returns: Light intensity as percentage (0-100%)
 */
float ReadLightSensor(void) {
    uint32_t adcValue;
    
    // Start ADC conversion
    ADC0_PSSI_R = ADC_PSSI_SS1;
    
    // Wait for conversion to complete
    while (!(ADC0_RIS_R & ADC_RIS_INR1)) {}
    
    // Read ADC value
    adcValue = ADC0_SSFIFO1_R;
    
    // Clear the interrupt flag
    ADC0_ISC_R = ADC_ISC_IN1;
    
    // Convert ADC value to percentage
    return ((float)adcValue / ADC_MAX_VALUE) * 100.0f;
}

/*
 * Read battery level from ADC channel 2 (PE1)
 * Returns: Battery level as percentage (0-100%)
 */
float ReadBatterySensor(void) {
    uint32_t adcValue;
    
    // Start ADC conversion
    ADC0_PSSI_R = ADC_PSSI_SS2;
    
    // Wait for conversion to complete
    while (!(ADC0_RIS_R & ADC_RIS_INR2)) {}
    
    // Read ADC value
    adcValue = ADC0_SSFIFO2_R;
    
    // Clear the interrupt flag
    ADC0_ISC_R = ADC_ISC_IN2;
    
    // Convert ADC value to percentage
    return ((float)adcValue / ADC_MAX_VALUE) * 100.0f;
}
