/* 
 * temperature_task.h
 * Header for Temperature Control Task
 */

#ifndef TEMPERATURE_TASK_H
#define TEMPERATURE_TASK_H

#include <stdint.h>
#include <stdbool.h>
#include "FreeRTOS.h"
#include "task.h"

/* Task Period in ticks */
#define TEMPERATURE_TASK_PERIOD     (500 / portTICK_PERIOD_MS)

/* Data structure for temperature communication */
typedef struct {
    float temperature;
    bool heaterActive;
} TemperatureData;

/* Function prototypes */
void vTemperatureControlTask(void *pvParameters);
void HEATER_ON(void);
void HEATER_OFF(void);
bool HEATER_STATUS(void);

#endif /* TEMPERATURE_TASK_H */
