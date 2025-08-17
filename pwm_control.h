/* 
 * pwm_control.h
 * Header for PWM Control Functions
 */

#ifndef PWM_CONTROL_H
#define PWM_CONTROL_H

#include <stdint.h>
#include <stdbool.h>

/* Function prototypes */
void InitPWM(void);
void SetServoPosition(uint8_t angle);

#endif /* PWM_CONTROL_H */
