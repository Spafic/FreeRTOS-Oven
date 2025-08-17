/* 
 * battery_task.h
 * Header for Battery Monitor Task
 */

#ifndef BATTERY_TASK_H
#define BATTERY_TASK_H

#include <stdint.h>
#include <stdbool.h>
#include "FreeRTOS.h"
#include "task.h"

/* Task Period in ticks */
#define BATTERY_TASK_PERIOD         (1000 / portTICK_PERIOD_MS)

/* Data structure for battery communication */
typedef struct {
    float batteryLevel;
} BatteryData;

/* Function prototypes */
void vBatteryMonitorTask(void *pvParameters);

#endif /* BATTERY_TASK_H */
