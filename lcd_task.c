/* 
 * lcd_task.c
 * LCD Display Task for Industrial Oven Control System
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "tm4c123gh6pm.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "lcd_task.h"
#include "lcd.h"
#include "temperature_task.h"
#include "door_task.h"
#include "battery_task.h"

/* External queue handles */
extern QueueHandle_t temperatureQueue;
extern QueueHandle_t doorQueue;
extern QueueHandle_t batteryQueue;

/* LCD Display Task - Update LCD with system information */
void vLCDDisplayTask(void *pvParameters) {
    (void) pvParameters; // Prevent unused parameter warning
    TemperatureData tempData;
    DoorData doorData;
    BatteryData batteryData;
    char line1[20]; // Increased buffer size
    char line2[20]; // Increased buffer size
    
    /* Initialize default values */
    tempData.temperature = 0.0f;
    doorData.doorOpen = false;
    batteryData.batteryLevel = 0.0f;
    
    for (;;) {
        /* Receive temperature data */
        if (xQueueReceive(temperatureQueue, &tempData, 0) == pdTRUE) {
            /* Temperature data received */
        }
        
        /* Receive door status */
        if (xQueueReceive(doorQueue, &doorData, 0) == pdTRUE) {
            /* Door status received */
        }
        
        /* Receive battery level */
        if (xQueueReceive(batteryQueue, &batteryData, 0) == pdTRUE) {
            /* Battery level received */
        }
        
        /* Format line 1: "Temp:XXX°C Batt:XX%" */
        sprintf(line1, "Temp:%.1fC Batt:%d%%", 
                tempData.temperature, 
                (int)batteryData.batteryLevel);
        
        /* Format line 2: "Door:OPEN/CLOSED Heat:ON/OFF" */
        sprintf(line2, "Door:%s Heat:%s",
                doorData.doorOpen ? "OPEN" : "CLOSED",
                tempData.heaterActive ? "ON" : "OFF");
        
        /* Update LCD display */
        LCD_Clear();
        LCD_SetCursor(0, 0);
        LCD_Print(line1);
        LCD_SetCursor(0, 1);
        LCD_Print(line2);
        
        /* Delay for the task period */
        vTaskDelay(LCD_TASK_PERIOD);
    }
}
