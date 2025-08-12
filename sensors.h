#ifndef SENSORS_H
#define SENSORS_H

void Sensors_Init(void);
float Sensors_ReadTemperatureC(void);
uint8_t Sensors_ReadBatteryPercent(void);
uint16_t Sensors_ReadDoorLightRaw(void);
#endif