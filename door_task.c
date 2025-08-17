/* 
 * door_task.c
 * Door Control Task for Industrial Oven Control System
 */

#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "door_task.h"
#include "adc_control.h"
#include "pwm_control.h"

/* External queue handle */
extern QueueHandle_t doorQueue;
extern SemaphoreHandle_t adcMutex;

/* Thresholds */
#define LIGHT_THRESHOLD             50.0f  // Light intensity percentage

/* Door Control Task - Read light sensor, control door */
void vDoorControlTask(void *pvParameters) {
    (void) pvParameters; // Prevent unused parameter warning
    DoorData data;
    float lightIntensity;
    bool doorOpen = false;
    
    for (;;) {
        /* Take ADC mutex to access ADC */
        if (xSemaphoreTake(adcMutex, portMAX_DELAY) == pdTRUE) {
            /* Read light intensity from ADC channel 1 (PE2) */
            lightIntensity = ReadLightSensor();
            xSemaphoreGive(adcMutex);
            
            /* Error handling - if light sensor reading is invalid (reading failed) */
            if (lightIntensity < 0.0f) {
                /* Safety measure: In case of sensor failure, keep door open */
                SetServoPosition(0);  // 0° = fully open (safety position)
                doorOpen = true;
                DOOR_STATUS_LED_ON();  // Indicate door is open
                
                /* Set a flag or error indicator if needed */
                data.doorOpen = doorOpen;
                data.sensorError = true;  // Indicate sensor error
                
                /* Send door status to LCD task */
                xQueueSend(doorQueue, &data, 0);
                
                /* Delay for the task period */
                vTaskDelay(DOOR_TASK_PERIOD);
                continue;  // Skip normal processing
            }
            
            /* Door control logic */
            if (lightIntensity < LIGHT_THRESHOLD) {
                /* Open door - obstruction detected (low light intensity) */
                SetServoPosition(0);  // 0° = fully open (rotate servo to 0 degrees)
                doorOpen = true;
                DOOR_STATUS_LED_ON();  // PF3 - Green LED on to indicate door is open
            } else {
                /* Close door - clear pathway (high light intensity) */
                SetServoPosition(90); // 90° = fully closed (rotate servo by 90 degrees)
                doorOpen = false;
                DOOR_STATUS_LED_OFF(); // PF3 - Green LED off when door is closed
            }
            
            /* Prepare data to send to LCD task */
            data.doorOpen = doorOpen;
            data.sensorError = false;  // No error for normal operation
            
            /* Send door status to LCD task */
            xQueueSend(doorQueue, &data, 0);
            
            /* Delay for the task period */
            vTaskDelay(DOOR_TASK_PERIOD);
        }
    }
}

/* Door status LED control functions */
void DOOR_STATUS_LED_ON(void) {
    GPIO_PORTF_DATA_R |= (1 << 3); // PF3 = 1
}

void DOOR_STATUS_LED_OFF(void) {
    GPIO_PORTF_DATA_R &= ~(1 << 3); // PF3 = 0
}
