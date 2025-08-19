/* 
 * gpio_init.c
 * GPIO Initialization for Industrial Oven Control System
 */

#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "gpio_init.h"

/* 
 * Initialize GPIO ports:
 * - Port E: ADC inputs (PE1, PE2, PE3)
 * - Port F: Digital outputs (PF1, PF2, PF3)
 * - Port B: I2C and PWM (PB2, PB3, PB6)
 */
void InitGPIO(void) {
    // Enable clock for all required GPIO ports
    SYSCTL_RCGCGPIO_R |= (SYSCTL_RCGCGPIO_R1 |   // Port B (I2C, PWM)
                          SYSCTL_RCGCGPIO_R4 |   // Port E (ADC)
                          SYSCTL_RCGCGPIO_R5);   // Port F (Digital outputs)
    
    // Wait for the GPIO ports to be ready
    while ((SYSCTL_PRGPIO_R & (SYSCTL_PRGPIO_R1 |
                              SYSCTL_PRGPIO_R4 |
                              SYSCTL_PRGPIO_R5)) !=
                              (SYSCTL_PRGPIO_R1 |
                              SYSCTL_PRGPIO_R4 |
                              SYSCTL_PRGPIO_R5)) {}
    
    // Port E: ADC Inputs (PE1, PE2, PE3)
    GPIO_PORTE_DIR_R &= ~((1 << 1) | (1 << 2) | (1 << 3)); // Set as inputs
    GPIO_PORTE_DEN_R &= ~((1 << 1) | (1 << 2) | (1 << 3)); // Disable digital functions
    GPIO_PORTE_AMSEL_R |= ((1 << 1) | (1 << 2) | (1 << 3)); // Enable analog functions
    GPIO_PORTE_AFSEL_R |= ((1 << 1) | (1 << 2) | (1 << 3)); // Enable alternate function
    
    // Port F: Digital Outputs (PF1, PF2, PF3)
    GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY;           // Unlock Port F
    GPIO_PORTF_CR_R |= ((1 << 1) | (1 << 2) | (1 << 3)); // Allow changes
    GPIO_PORTF_DIR_R |= ((1 << 1) | (1 << 2) | (1 << 3)); // Set as outputs
    GPIO_PORTF_DEN_R |= ((1 << 1) | (1 << 2) | (1 << 3)); // Enable digital functions
    GPIO_PORTF_AFSEL_R &= ~((1 << 1) | (1 << 2) | (1 << 3)); // Disable alternate functions
    GPIO_PORTF_DATA_R &= ~((1 << 1) | (1 << 2) | (1 << 3)); // Initialize to OFF
    
    // Port B: I2C (PB2, PB3)
    // Configure I2C pins
    GPIO_PORTB_AFSEL_R |= ((1 << 2) | (1 << 3)); // Enable alternate function for PB2, PB3
    GPIO_PORTB_ODR_R |= (1 << 3);                // Open drain for SDA (PB3)
    GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & ~0x0000FF00) | 0x00003300; // I2C function for PB2, PB3

    // Now set directions and enable digital functions
    GPIO_PORTB_DIR_R |= (1 << 2);                // Set PB2 as output (SCL)
    GPIO_PORTB_DIR_R &= ~(1 << 3);               // Set PB3 as input (SDA)
    GPIO_PORTB_DEN_R |= ((1 << 2) | (1 << 3)); // Enable digital functions
}
