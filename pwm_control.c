/* 
 * pwm_control.c
 * PWM Control Functions for Servo Motor
 */

#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "pwm_control.h"

/* PWM Constants */
#define PWM_FREQUENCY_HZ          50        // 50Hz for servo control (20ms period)
#define PWM_PERIOD                (80000000 / 64 / PWM_FREQUENCY_HZ) // Clock/prescaler/Frequency
#define SERVO_MIN_PULSE           (PWM_PERIOD * 0.05)  // 1ms pulse (0 degrees)
#define SERVO_MAX_PULSE           (PWM_PERIOD * 0.1)   // 2ms pulse (180 degrees)

/*
 * Initialize PWM for servo control
 */
void InitPWM(void) {
    // Enable PWM0 clock
    SYSCTL_RCGCPWM_R |= SYSCTL_RCGCPWM_R0;
    
    // Wait for PWM0 to be ready
    while ((SYSCTL_PRPWM_R & SYSCTL_PRPWM_R0) != SYSCTL_PRPWM_R0) {}
    
    // Configure PWM clock (80MHz / 64 = 1.25MHz)
    // This gives a good resolution for the servo control
    SYSCTL_RCC_R = (SYSCTL_RCC_R & ~SYSCTL_RCC_PWMDIV_M) |
                   SYSCTL_RCC_USEPWMDIV |
                   SYSCTL_RCC_PWMDIV_64;
    
    // Disable PWM0 Generator 0 during configuration
    PWM0_0_CTL_R = 0;
    
    // Configure PWM0 Generator 0 for down count mode
    PWM0_0_GENA_R = PWM_0_GENA_ACTCMPAD_ZERO | PWM_0_GENA_ACTLOAD_ONE;
    
    // Set PWM period (50Hz = 20ms)
    PWM0_0_LOAD_R = PWM_PERIOD - 1;
    
    // Set initial pulse width (0 degrees)
    PWM0_0_CMPA_R = PWM_PERIOD - (uint32_t)SERVO_MIN_PULSE;
    
    // Enable PWM0 Generator 0
    PWM0_0_CTL_R |= PWM_0_CTL_ENABLE;
    
    // Enable PWM0 output
    PWM0_ENABLE_R |= PWM_ENABLE_PWM0EN;
}

/*
 * Set servo position
 * angle: Angle in degrees (0-180)
 */
void SetServoPosition(uint8_t angle) {
    uint32_t pulseWidth;
    
    // Limit angle to 0-180 degrees
    if (angle > 180) {
        angle = 180;
    }
    
    // Calculate pulse width based on angle
    // 0 degrees = 1ms pulse (SERVO_MIN_PULSE)
    // 180 degrees = 2ms pulse (SERVO_MAX_PULSE)
    pulseWidth = SERVO_MIN_PULSE + ((SERVO_MAX_PULSE - SERVO_MIN_PULSE) * angle / 180);
    
    // Update PWM compare value
    // Note: The PWM counter counts down, so we need to subtract from PWM_PERIOD
    PWM0_0_CMPA_R = PWM_PERIOD - (uint32_t)pulseWidth;
}
