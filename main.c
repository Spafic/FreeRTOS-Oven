/* 
 * Industrial Oven Control System
 * Tiva C TM4C123GH6PM with FreeRTOS
 */

#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* Project headers */
#include "adc_control.h"
#include "pwm_control.h"
#include "gpio_init.h"
#include "lcd.h"
#include "temperature_task.h"
#include "door_task.h"
#include "battery_task.h"
#include "lcd_task.h"
#include "buzzer_task.h"

/* Task Priorities */
#define TEMPERATURE_TASK_PRIORITY   3  // High
#define DOOR_TASK_PRIORITY          2  // Medium-High 
#define BATTERY_TASK_PRIORITY       2  // Medium
#define LCD_TASK_PRIORITY           1  // Medium-Low
#define BUZZER_TASK_PRIORITY        3  // High

/* Stack Sizes */
#define TASK_STACK_SIZE             128

/* Queue Sizes */
#define QUEUE_SIZE                  4

/* Global queue and semaphore handles */
QueueHandle_t temperatureQueue;
QueueHandle_t doorQueue;
QueueHandle_t batteryQueue;
SemaphoreHandle_t buzzerSemaphore;
SemaphoreHandle_t adcMutex;

/* Application entry point */
int main(void) {
    /* Initialize hardware components */
    InitGPIO();      // Initialize GPIO ports
    InitADC();       // Initialize ADC
    InitPWM();       // Initialize PWM
    LCD_Init();      // Initialize LCD
    
    /* Create queues */
    temperatureQueue = xQueueCreate(QUEUE_SIZE, sizeof(TemperatureData));
    doorQueue = xQueueCreate(QUEUE_SIZE, sizeof(DoorData));
    batteryQueue = xQueueCreate(QUEUE_SIZE, sizeof(BatteryData));
    
    /* Create semaphores */
    buzzerSemaphore = xSemaphoreCreateBinary();
    adcMutex = xSemaphoreCreateMutex();
    
    /* Create tasks */
    xTaskCreate(vTemperatureControlTask, "Temperature", TASK_STACK_SIZE, NULL, TEMPERATURE_TASK_PRIORITY, NULL);
    xTaskCreate(vDoorControlTask, "Door", TASK_STACK_SIZE, NULL, DOOR_TASK_PRIORITY, NULL);
    xTaskCreate(vBatteryMonitorTask, "Battery", TASK_STACK_SIZE, NULL, BATTERY_TASK_PRIORITY, NULL);
    xTaskCreate(vLCDDisplayTask, "LCD", TASK_STACK_SIZE, NULL, LCD_TASK_PRIORITY, NULL);
    xTaskCreate(vBuzzerControlTask, "Buzzer", TASK_STACK_SIZE, NULL, BUZZER_TASK_PRIORITY, NULL);
    
    /* Start the scheduler */
    vTaskStartScheduler();
    
    /* Should never reach here unless there is insufficient RAM */
    for (;;) {}
}

/* FreeRTOS application hook functions */

void vApplicationMallocFailedHook(void) {
    /* Called if a memory allocation failed */
    for (;;) {}
}

void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName) {
    /* Called if a stack overflow is detected */
    (void) pxTask;
    (void) pcTaskName;
    for (;;) {}
}

void vApplicationTickHook(void) {
    /* Called from each tick interrupt */
}

void vApplicationIdleHook(void) {
    /* Called when the idle task is running */
}
