/* 
 * battery_task.c
 * Battery Monitor Task for Industrial Oven Control System
 */

#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "battery_task.h"
#include "adc_control.h"

/* External queue and semaphore handles */
extern QueueHandle_t batteryQueue;
extern SemaphoreHandle_t buzzerSemaphore;
extern SemaphoreHandle_t adcMutex;

/* Thresholds */
#define BATTERY_LOW_THRESHOLD       20.0f  // Battery percentage

/* Battery Monitor Task - Monitor battery level, trigger alarm if low */
void vBatteryMonitorTask(void *pvParameters) {
    (void) pvParameters; // Prevent unused parameter warning
    BatteryData data;
    float batteryLevel;
    
    for (;;) {
        /* Take ADC mutex to access ADC */
        if (xSemaphoreTake(adcMutex, portMAX_DELAY) == pdTRUE) {
            /* Read battery level from ADC channel 2 (PE1) */
            batteryLevel = ReadBatterySensor();
            xSemaphoreGive(adcMutex);
            
            /* Battery monitor logic */
            if (batteryLevel < BATTERY_LOW_THRESHOLD) {
                /* Battery is low, trigger buzzer */
                xSemaphoreGive(buzzerSemaphore);
            }
            
            /* Prepare data to send to LCD task */
            data.batteryLevel = batteryLevel;
            
            /* Send battery data to LCD task */
            xQueueSend(batteryQueue, &data, 0);
            
            /* Delay for the task period */
            vTaskDelay(BATTERY_TASK_PERIOD);
        }
    }
}
