#include "temperature_task.h"
#include "sensors.h"
#include "hardware.h"

void Temperature_Task(void *pvParameters)
{
    (void) pvParameters;
    int16_t temp_celsius;
    uint8_t alarm_active = 0;

    for (;;)
    {
        // Read temperature in °C
        temp_celsius = Sensors_ReadTemperatureC();

        // Later: hook to LCD display
        Hardware_DisplayTemperature(temp_celsius);

        // Temperature alarm logic with hysteresis
        if (!alarm_active && temp_celsius >= TEMP_HIGH_THRESHOLD_C)
        {
            Hardware_TempLedOn();
            alarm_active = 1;
        }
        else if (alarm_active && temp_celsius <= TEMP_CLEAR_THRESHOLD_C)
        {
            Hardware_TempLedOff();
            alarm_active = 0;
        }

        vTaskDelay(pdMS_TO_TICKS(TEMPERATURE_TASK_DELAY_MS));
    }
}
