/* 
 * door_task.h
 * Header for Door Control Task
 */

#ifndef DOOR_TASK_H
#define DOOR_TASK_H

#include <stdint.h>
#include <stdbool.h>
#include "FreeRTOS.h"
#include "task.h"

/* Task Period in ticks */
#define DOOR_TASK_PERIOD            (300 / portTICK_PERIOD_MS)

/* Data structure for door communication */
typedef struct {
    bool doorOpen;
} DoorData;

/* Function prototypes */
void vDoorControlTask(void *pvParameters);
void DOOR_STATUS_LED_ON(void);
void DOOR_STATUS_LED_OFF(void);

#endif /* DOOR_TASK_H */
