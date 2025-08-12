#ifndef DOOR_TASK_H_
#define DOOR_TASK_H_

#include <stdint.h>

void DoorTask(void *pvParameters);
void Servo_Init(void);
void Servo_SetAngle(uint8_t angle);
void ADC_Init_Door(void);

#endif