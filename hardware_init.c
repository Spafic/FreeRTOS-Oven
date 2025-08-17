/*
 * Hardware Initialization for the Industrial Oven Control System
 * TM4C123GH6PM
 */

#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "hardware_init.h"
#include "gpio_init.h"
#include "adc_control.h"
#include "pwm_control.h"
#include "lcd.h"

/*
 * Initialize all hardware components:
 * - GPIO ports
 * - ADC for sensors
 * - I2C for LCD
 * - PWM for servo control
 */
void HardwareInit(void) {
    // Enable clock for all required peripherals
    SYSCTL_RCGCGPIO_R |= (SYSCTL_RCGCGPIO_R0 |   // Port A
                          SYSCTL_RCGCGPIO_R1 |   // Port B
                          SYSCTL_RCGCGPIO_R4 |   // Port E
                          SYSCTL_RCGCGPIO_R5);   // Port F
    SYSCTL_RCGCADC_R |= SYSCTL_RCGCADC_R0;       // ADC0
    SYSCTL_RCGCI2C_R |= SYSCTL_RCGCI2C_R0;       // I2C0
    SYSCTL_RCGCPWM_R |= SYSCTL_RCGCPWM_R0;       // PWM Module 0
    
    // Wait for peripherals to be ready
    while ((SYSCTL_PRGPIO_R & (SYSCTL_PRGPIO_R0 |
                              SYSCTL_PRGPIO_R1 |
                              SYSCTL_PRGPIO_R4 |
                              SYSCTL_PRGPIO_R5)) !=
                              (SYSCTL_PRGPIO_R0 |
                              SYSCTL_PRGPIO_R1 |
                              SYSCTL_PRGPIO_R4 |
                              SYSCTL_PRGPIO_R5)) {}
    
    // Initialize all hardware components
    InitGPIO();  // Initialize GPIO ports
    InitADC();   // Initialize ADC
    InitI2C();   // Initialize I2C for LCD
    InitPWM();   // Initialize PWM for servo control
}
