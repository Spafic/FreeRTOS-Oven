#ifndef BATTERY_TASK_H
#define BATTERY_TASK_H

#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"

// Battery alarm threshold
#define BATTERY_LOW_THRESHOLD_PERCENT  20
#define BATTERY_CLEAR_THRESHOLD_PERCENT 25
#define BATTERY_TASK_DELAY_MS          1000

void Battery_Task(void *pvParameters);

#endif