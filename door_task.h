#ifndef DOOR_TASK_H
#define DOOR_TASK_H

#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"

// Threshold for detecting if the door is open (potentiometer reading %)
#define DOOR_OPEN_THRESHOLD_PERCENT  50  // Adjust depending on potentiometer position
#define DOOR_TASK_DELAY_MS           500 // Check every 0.5 sec

void Door_Task(void *pvParameters);

#endif