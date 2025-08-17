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
#include "semphr.h"
#include "lcd_task.h"
#include "lcd.h"
#include "temperature_task.h"
#include "door_task.h"
#include "battery_task.h"

/* External queue handles */
extern QueueHandle_t temperatureQueue;
extern QueueHandle_t doorQueue;
extern QueueHandle_t batteryQueue;
extern SemaphoreHandle_t adcMutex;

/* LCD Display Task - Update LCD with system information */
void vLCDDisplayTask(void *pvParameters) {
    (void) pvParameters; // Prevent unused parameter warning
    TemperatureData tempData;
    DoorData doorData;
    BatteryData batteryData;
    char buffer[10]; // Buffer for dynamic values
    
    /* Initialize default values */
    tempData.temperature = 0.0f;
    doorData.doorOpen = false;
    batteryData.batteryLevel = 0.0f;
    tempData.heaterActive = false;
    
    /* Initialize the LCD display once */
    LCD_Clear();
    
    /* First line - Temperature and Battery */
    LCD_SetCursor(0, 0);
    LCD_Print("T:");
    LCD_SetCursor(9, 0);
    LCD_Print("B:");
    
    /* Second line - Door State and Heater */
    LCD_SetCursor(0, 1);
    LCD_Print("D:");
    LCD_SetCursor(9, 1);
    LCD_Print("H:");
    
    /* Previous state values to detect changes */
    float prevTemp = -1.0f;
    int prevBatt = -1;
    bool prevDoor = !doorData.doorOpen;
    bool prevHeater = !tempData.heaterActive;
    
    /* Main task loop */
    for (;;) {
        /* Get ADC mutex to ensure consistent sensor readings */
        if (xSemaphoreTake(adcMutex, pdMS_TO_TICKS(10)) == pdTRUE) {
            /* Fast retrieval of all sensor data without blocking */
            xQueueReceive(temperatureQueue, &tempData, 0);
            xQueueReceive(doorQueue, &doorData, 0);
            xQueueReceive(batteryQueue, &batteryData, 0);
            
            /* Release the ADC mutex immediately after reading */
            xSemaphoreGive(adcMutex);
            
            /* Update temperature if changed */
            if (prevTemp != tempData.temperature) {
                LCD_SetCursor(2, 0);
                
                /* Highlight temperature based on thresholds */
                if (tempData.temperature < 25.0f) {
                    sprintf(buffer, "%5.1fC", tempData.temperature); /* Below setpoint */
                } else if (tempData.temperature > 30.0f) {
                    sprintf(buffer, "%5.1fC", tempData.temperature); /* Above setpoint */
                } else {
                    sprintf(buffer, "%5.1fC", tempData.temperature); /* Within range */
                }
                
                LCD_Print(buffer);
                prevTemp = tempData.temperature;
            }
            
            /* Update battery percentage if changed */
            if (prevBatt != (int)batteryData.batteryLevel) {
                LCD_SetCursor(11, 0);
                
                /* Highlight battery level when low */
                if (batteryData.batteryLevel < 20.0f) {
                    sprintf(buffer, "%.1f%%", batteryData.batteryLevel); /* Low battery */
                } else {
                    sprintf(buffer, "%.1f%%", batteryData.batteryLevel); /* Normal battery */
                }
                
                LCD_Print(buffer);
                prevBatt = (int)batteryData.batteryLevel;
            }
            
            /* Update door status if changed */
            if (prevDoor != doorData.doorOpen || doorData.sensorError) {
                LCD_SetCursor(2, 1);
                if (doorData.sensorError) {
                    LCD_Print("ERROR");  // Display error if sensor fails
                } else {
                    LCD_Print(doorData.doorOpen ? "OPEN " : "CLSD ");
                }
                prevDoor = doorData.doorOpen;
            }
            
            /* Update heater status if changed */
            if (prevHeater != tempData.heaterActive) {
                LCD_SetCursor(11, 1);
                LCD_Print(tempData.heaterActive ? "ON " : "OFF");
                prevHeater = tempData.heaterActive;
            }
        }
        
        /* Delay for the task period */
        vTaskDelay(LCD_TASK_PERIOD);
    }
}
