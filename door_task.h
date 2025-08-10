#ifndef DOOR_TASK_H
#define DOOR_TASK_H
#include <stdint.h>


void Door_Task(void *pvParameters);
void PWM0_Init(void);
void Servo_SetAngle(int angle);
void LED_Init(void);
void LED_Set(int on);
void ADC0_Init(void);
uint16_t ADC0_Read(void);

#endif
