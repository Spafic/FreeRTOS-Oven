#include "FreeRTOS.h"
#include "task.h"
#include "battery_task.h"
#include "sensors.h"
#include "hardware.h"
#include "door_task.h"
#include "temperature_task.h"
#include "display_task.h"
#include "shared_data.h"
#include "display_config.h"

int main(void)
{
    Hardware_Init();
    Sensors_Init();
    Shared_Init();

    // Create tasks
    xTaskCreate(Battery_Task, "Battery", STACK_DEFAULT, NULL, PRIO_BATTERY_TASK, NULL);
    xTaskCreate(Door_Task,    "Door",    STACK_DEFAULT, NULL, PRIO_DOOR_TASK, NULL);
    xTaskCreate(Temperature_Task, "Temp", STACK_DEFAULT, NULL, PRIO_TEMPERATURE_TASK, NULL);
    Display_CreateTask();

    vTaskStartScheduler();

    for (;;);
}