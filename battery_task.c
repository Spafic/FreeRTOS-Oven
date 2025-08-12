#include "battery_task.h"
#include "sensors.h"
#include "hardware.h"
#include "shared_data.h"
#include "display_config.h"

void Battery_Task(void *pvParameters)
{
    (void) pvParameters;
    uint8_t battery_percent;
    uint8_t alarm_active = 0;

    for (;;)
    {
        // Read battery level in percentage
        battery_percent = Sensors_ReadBatteryPercent();

    // Send to display (non-blocking)
    DisplayMsg_t msg = { .field = DISP_FIELD_BATTERY, .value = battery_percent };
    Shared_TrySendDisplay(&msg);

        // Battery alarm logic with hysteresis
        if (!alarm_active && battery_percent <= BATTERY_LOW_THRESHOLD_PERCENT)
        {
            Hardware_BuzzerOn();
            alarm_active = 1;
            DisplayMsg_t alert = { .field = DISP_FIELD_ALERT, .value = 1 };
            Shared_TrySendDisplay(&alert);
        }
        else if (alarm_active && battery_percent >= BATTERY_CLEAR_THRESHOLD_PERCENT)
        {
            Hardware_BuzzerOff();
            alarm_active = 0;
            DisplayMsg_t alert = { .field = DISP_FIELD_ALERT, .value = 101 }; // clear battery alert only
            Shared_TrySendDisplay(&alert);
        }

        vTaskDelay(pdMS_TO_TICKS(BATTERY_TASK_DELAY_MS));
    }
}