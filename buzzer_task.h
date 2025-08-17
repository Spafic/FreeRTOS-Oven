/* 
 * buzzer_task.h
 * Header for Buzzer Control Task
 */

#ifndef BUZZER_TASK_H
#define BUZZER_TASK_H

#include <stdint.h>
#include <stdbool.h>
#include "FreeRTOS.h"
#include "task.h"

/* Function prototypes */
void vBuzzerControlTask(void *pvParameters);
void BUZZER_ON(void);
void BUZZER_OFF(void);

#endif /* BUZZER_TASK_H */
