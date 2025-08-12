#ifndef SENSORS_H
#define SENSORS_H

#include <stdint.h>

void Sensors_Init(void);
uint16_t Sensors_ReadBatteryRaw(void);
uint8_t Sensors_ReadBatteryPercent(void);

int16_t Sensors_ReadTemperatureC(void);

uint16_t Sensors_ReadDoorPot(void);

#endif