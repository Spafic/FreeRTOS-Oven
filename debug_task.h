#ifndef DEBUG_TASK_H
#define DEBUG_TASK_H

#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"

// Debug
void Hardware_DebugLedToggle(void);
void Hardware_DebugLedOn(void);
void Hardware_DebugLedOff(void);
void Debug_Task(void *pvParameters);
#endif