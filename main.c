#include "FreeRTOS.h"
#include "task.h"
#include "battery_task.h"
#include "sensors.h"
#include "hardware.h"

int main(void)
{
    Hardware_Init();
    Sensors_Init();

    xTaskCreate(Battery_Task, "Battery", 128, NULL, 2, NULL);

    vTaskStartScheduler();

    for (;;);
}