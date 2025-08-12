#ifndef SHARED_DATA_H
#define SHARED_DATA_H

#include <stdint.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

// Display message types
typedef enum {
    DISP_FIELD_BATTERY,
    DISP_FIELD_TEMPERATURE,
    DISP_FIELD_DOOR,
    DISP_FIELD_ALERT
} DisplayField_t;

typedef struct {
    DisplayField_t field;
    uint32_t value; // percentage for battery; temperature in C x1; door: 0=closed,1=open; alert code
} DisplayMsg_t;

// Handles shared across modules
extern QueueHandle_t gDisplayQueue;
extern SemaphoreHandle_t gI2CMutex;
extern SemaphoreHandle_t gADCMutex;

// Init all shared RTOS objects
void Shared_Init(void);

// Convenience send helper (non-blocking)
static inline BaseType_t Shared_TrySendDisplay(const DisplayMsg_t *msg)
{
    if (gDisplayQueue == NULL) return pdFAIL;
    return xQueueSend(gDisplayQueue, msg, 0);
}

// Task priorities (highest numeric = highest priority)
#define PRIO_DOOR_TASK         (3)
#define PRIO_TEMPERATURE_TASK  (2)
#define PRIO_BATTERY_TASK      (1)
#define PRIO_DISPLAY_TASK      (1)

// Stack sizes
#define STACK_DEFAULT          (configMINIMAL_STACK_SIZE)
#define STACK_DISPLAY_TASK     (configMINIMAL_STACK_SIZE + 128)

#endif
