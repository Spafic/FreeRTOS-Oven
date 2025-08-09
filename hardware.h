#ifndef HARDWARE_H
#define HARDWARE_H

#include <stdint.h>

void Hardware_Init(void);
void Hardware_BuzzerOn(void);
void Hardware_BuzzerOff(void);
void Hardware_DisplayBattery(uint8_t percent);

#endif