#include "hardware.h"
#include "FreeRTOS.h"
#include "task.h"
#include "debug_task.h"

void Debug_Task(void *pvParameters)
{
    (void)pvParameters;
    for (;;)
    {
        Hardware_DebugLedToggle();
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}