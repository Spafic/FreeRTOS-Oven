#include "battery_task.h"
#include "sensors.h"
#include "hardware.h"

void Battery_Task(void *pvParameters)
{
    (void) pvParameters;
    uint8_t battery_percent;
    uint8_t alarm_active = 0;

    for (;;)
    {
        // Read battery level in percentage
        battery_percent = Sensors_ReadBatteryPercent();

        // Show on LCD (optional later)
        Hardware_DisplayBattery(battery_percent);

        // Battery alarm logic with hysteresis
        if (!alarm_active && battery_percent <= BATTERY_LOW_THRESHOLD_PERCENT)
        {
            Hardware_BuzzerOn();
            alarm_active = 1;
        }
        else if (alarm_active && battery_percent >= BATTERY_CLEAR_THRESHOLD_PERCENT)
        {
            Hardware_BuzzerOff();
            alarm_active = 0;
        }

        vTaskDelay(pdMS_TO_TICKS(BATTERY_TASK_DELAY_MS));
    }
}