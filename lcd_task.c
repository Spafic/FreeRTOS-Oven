/* 
 * lcd_task.c
 * LCD Display Task for Industrial Oven Control System
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
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
    char line0[17]; // Full 16-char line buffer + NUL
    char line1[17];
    
    /* Initialize default values */
    tempData.temperature = 0.0f;
    doorData.doorOpen = false;
    batteryData.batteryLevel = 0.0f;
    tempData.heaterActive = false;
    
    /* Initialize the LCD display once */
    /* Show welcome screen first, then continue to sensor UI */
    LCD_ShowWelcome();
    LCD_Clear();
    
    /* Initialize display with placeholders (will be overwritten) */
    memset(line0, ' ', sizeof(line0)); line0[16] = '\0';
    memset(line1, ' ', sizeof(line1)); line1[16] = '\0';
    LCD_SetCursor(0,0);
    LCD_Print(line0);
    LCD_SetCursor(0,1);
    LCD_Print(line1);

    /* Previous state values to detect changes (use ints for stability) */
    int prevTempInt = INT_MIN;
    int prevBattInt = INT_MIN;
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
            
            /* Compute integer representations for stable comparisons */
            int tempInt = (int)(tempData.temperature + 0.5f);
            int battInt = (int)(batteryData.batteryLevel + 0.5f);

            bool tempChanged = (tempInt != prevTempInt);
            bool battChanged = (battInt != prevBattInt);
            bool doorChanged = (prevDoor != doorData.doorOpen) || doorData.sensorError;
            bool heaterChanged = (prevHeater != tempData.heaterActive);

            /* If any relevant state changed, rebuild the full lines and write them.
             * Writing the full 16-char line ensures leftover characters are cleared.
             */
            if (tempChanged || battChanged) {
                /* Format: T:9C  B:95% (no leading padding before numbers) */
                snprintf(line0, sizeof(line0), "T:%dC  B:%d%%", tempInt, battInt);
                /* Pad to 16 chars */
                size_t len0 = strlen(line0);
                if (len0 < 16) {
                    memset(line0 + len0, ' ', 16 - len0);
                    line0[16] = '\0';
                }
                LCD_SetCursor(0,0);
                LCD_Print(line0);

                prevTempInt = tempInt;
                prevBattInt = battInt;
            }

            if (doorChanged || heaterChanged) {
                /* Door string preference: show full words; show ERROR if sensor fails */
                const char *doorStr = doorData.sensorError ? "ERR" : (doorData.doorOpen ? "OPEN" : "CLOSED");
                snprintf(line1, sizeof(line1), "D:%s H:%s", doorStr, tempData.heaterActive ? "ON" : "OFF");
                /* Pad to 16 chars */
                size_t len1 = strlen(line1);
                if (len1 < 16) {
                    memset(line1 + len1, ' ', 16 - len1);
                    line1[16] = '\0';
                }
                LCD_SetCursor(0,1);
                LCD_Print(line1);

                prevDoor = doorData.doorOpen;
                prevHeater = tempData.heaterActive;
            }
        }
        
        /* Delay for the task period */
        vTaskDelay(LCD_TASK_PERIOD);
    }
}
