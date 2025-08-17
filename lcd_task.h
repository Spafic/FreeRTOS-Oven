/* 
 * lcd_task.h
 * Header for LCD Display Task
 */

#ifndef LCD_TASK_H
#define LCD_TASK_H

#include <stdint.h>
#include <stdbool.h>
#include "FreeRTOS.h"
#include "task.h"

/* Task Period in ticks */
#define LCD_TASK_PERIOD             (200 / portTICK_PERIOD_MS)  // 200ms for more responsive updates

/* Function prototypes */
void vLCDDisplayTask(void *pvParameters);

#endif /* LCD_TASK_H */
