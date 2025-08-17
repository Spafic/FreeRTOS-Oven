/* 
 * adc_control.h
 * Header for ADC Control Functions
 */

#ifndef ADC_CONTROL_H
#define ADC_CONTROL_H

#include <stdint.h>
#include <stdbool.h>

/* Function prototypes */
void InitADC(void);
float ReadTemperatureSensor(void);
float ReadLightSensor(void);
float ReadBatterySensor(void);

#endif /* ADC_CONTROL_H */
