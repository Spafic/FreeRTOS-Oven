#include "door_task.h"
#include "sensors.h"
#include "hardware.h"

void Door_Task(void *pvParameters)
{
    (void) pvParameters;
    uint16_t pot_raw;
    uint8_t door_open = 0;

    for (;;)
    {
        // Read potentiometer raw value
        pot_raw = Sensors_ReadDoorPot();

        // Convert raw ADC to percentage
        uint8_t percent = (pot_raw * 100) / 4095;

        // Door logic
        if (!door_open && percent > DOOR_OPEN_THRESHOLD_PERCENT)
        {
            // Door is now open
            Hardware_DoorLedOn();
            door_open = 1;
        }
        else if (door_open && percent <= DOOR_OPEN_THRESHOLD_PERCENT)
        {
            // Door closed
            Hardware_DoorLedOff();
            door_open = 0;
        }

        vTaskDelay(pdMS_TO_TICKS(DOOR_TASK_DELAY_MS));
    }
}