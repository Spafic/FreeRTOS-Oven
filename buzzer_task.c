/* 
 * buzzer_task.c
 * Buzzer Control Task for Industrial Oven Control System
 */

#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "buzzer_task.h"

/* External semaphore handle */
extern SemaphoreHandle_t buzzerSemaphore;

/* Buzzer Control Task - Sound buzzer when battery is low */
void vBuzzerControlTask(void *pvParameters) {
    (void) pvParameters; // Prevent unused parameter warning
    for (;;) {
        /* Wait for buzzer semaphore */
        if (xSemaphoreTake(buzzerSemaphore, portMAX_DELAY) == pdTRUE) {
            /* Buzzer semaphore received, sound the buzzer */
            BUZZER_ON();
            vTaskDelay(500 / portTICK_PERIOD_MS); // Buzzer on for 500ms
            BUZZER_OFF();
            vTaskDelay(500 / portTICK_PERIOD_MS); // Buzzer off for 500ms
            BUZZER_ON();
            vTaskDelay(500 / portTICK_PERIOD_MS); // Buzzer on for 500ms
            BUZZER_OFF();
        }
    }
}

/* Buzzer control functions */
void BUZZER_ON(void) {
    GPIO_PORTF_DATA_R |= (1 << 2); // PF2 = 1
}

void BUZZER_OFF(void) {
    GPIO_PORTF_DATA_R &= ~(1 << 2); // PF2 = 0
}
