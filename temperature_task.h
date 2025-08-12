#ifndef TEMPERATURE_TASK_H
#define TEMPERATURE_TASK_H

#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"

// Temperature thresholds (°C)
#define TEMP_HIGH_THRESHOLD_C     70   // Example high limit
#define TEMP_CLEAR_THRESHOLD_C    65   // Hysteresis clear point
#define TEMPERATURE_TASK_DELAY_MS 1000 // 1 second delay

void Temperature_Task(void *pvParameters);

#endif