#include "FreeRTOS.h"
#include "task.h"
#include "battery_task.h"
#include "temperature_task.h"
#include "sensors.h"
#include "hardware.h"
#include "debug_task.h"
#include "door_task.h"

extern void DoorTask(void *pvParameters);

int main(void)
{
    Hardware_Init();
    Sensors_Init();

    xTaskCreate(Battery_Task, "Battery", 256, NULL, 2, NULL);
    xTaskCreate(Temperature_Task, "Temp", 256, NULL, 2, NULL);
    xTaskCreate(DoorTask, "Door", 256, NULL, 1, NULL);
    xTaskCreate(Debug_Task, "Debug", 128, NULL, 1, NULL);

    vTaskStartScheduler();

    for (;;);
}