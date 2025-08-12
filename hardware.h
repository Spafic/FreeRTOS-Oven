#ifndef HARDWARE_H
#define HARDWARE_H

#include <stdint.h>

// Buzzer control
void Hardware_BuzzerOn(void);
void Hardware_BuzzerOff(void);

// Battery display (stub)
void Hardware_DisplayBattery(uint8_t percent);

// Green LED control (Temperature alarm)
void Hardware_TempLedOn(void);
void Hardware_TempLedOff(void);

// Temperature display (stub)
void Hardware_DisplayTemperature(int16_t temp_c);

// Initialization
void Hardware_Init(void);

#endif
