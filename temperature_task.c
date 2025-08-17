/* 
 * temperature_task.c
 * Temperature Control Task for Industrial Oven Control System
 */

#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "temperature_task.h"
#include "adc_control.h"

/* External queue handle */
extern QueueHandle_t temperatureQueue;
extern SemaphoreHandle_t adcMutex;

/* Thresholds */
#define TEMP_SETPOINT_LOW           25.0f  // Temperature in °C
#define TEMP_SETPOINT_HIGH          30.0f  // Temperature in °C

/* Temperature Control Task - Read temperature sensor, control heater */
void vTemperatureControlTask(void *pvParameters) {
    (void) pvParameters; // Prevent unused parameter warning
    TemperatureData data;
    float temperature;
    bool heaterStatus = false;
    
    for (;;) {
        /* Take ADC mutex to access ADC */
        if (xSemaphoreTake(adcMutex, portMAX_DELAY) == pdTRUE) {
            /* Read temperature from ADC channel 0 (PE3) */
            temperature = ReadTemperatureSensor();
            xSemaphoreGive(adcMutex);
            
            /* Bang-bang controller logic */
            if (temperature > TEMP_SETPOINT_HIGH) {
                /* Turn off heater - temperature too high */
                HEATER_OFF();
                heaterStatus = false;
                
                /* Turn on red LED as warning when temperature exceeds high threshold */
                GPIO_PORTF_DATA_R |= (1 << 1); // PF1 (Red LED) = 1
            } else if (temperature < TEMP_SETPOINT_LOW) {
                /* Turn on heater - temperature too low */
                HEATER_ON();
                heaterStatus = true;
                
                /* Turn off red LED - temperature is normal or low */
                GPIO_PORTF_DATA_R &= ~(1 << 1); // PF1 (Red LED) = 0
            }
            
            /* Prepare data to send to LCD task */
            data.temperature = temperature;
            data.heaterActive = heaterStatus;
            
            /* Send temperature data to LCD task */
            xQueueSend(temperatureQueue, &data, 0);
            
            /* Delay for the task period */
            vTaskDelay(TEMPERATURE_TASK_PERIOD);
        }
    }
}

/* Heater control functions */
void HEATER_ON(void) {
    GPIO_PORTF_DATA_R |= (1 << 1); // PF1 = 1
}

void HEATER_OFF(void) {
    GPIO_PORTF_DATA_R &= ~(1 << 1); // PF1 = 0
}

bool HEATER_STATUS(void) {
    return (GPIO_PORTF_DATA_R & (1 << 1)) ? true : false;
}
