#include <stdint.h>
#include "tm4c.h"
#include "FreeRTOS.h"
#include "task.h"
#include "sensors.h"
#include "shared_data.h"

#define TEMP_THRESHOLD 200.0f // Example threshold value

void Temperature_Task(void *pvParameters) {
    (void)pvParameters;

    // Ensure Port F clock for LED
    SYSCTL_RCGCGPIO_R |= (1U << 5);
    while ((SYSCTL_PRGPIO_R & (1U << 5)) == 0);
    GPIO_PORTF_DIR_R |= (1U << 2);
    GPIO_PORTF_DEN_R |= (1U << 2);

    while (1) {
        float temperatureC = Sensors_ReadTemperatureC();

        if (temperatureC > TEMP_THRESHOLD) {
            GPIO_PORTF_DATA_R |= (1U << 2);  // Blue LED ON
            DisplayMsg_t alert = { .field = DISP_FIELD_ALERT, .value = 2 };
            Shared_TrySendDisplay(&alert);
        } else {
            GPIO_PORTF_DATA_R &= ~(1U << 2); // Blue LED OFF
            DisplayMsg_t alert = { .field = DISP_FIELD_ALERT, .value = 102 }; // clear temp alert only
            Shared_TrySendDisplay(&alert);
        }

    // Send integer temperature to display
    DisplayMsg_t msg = { .field = DISP_FIELD_TEMPERATURE, .value = (uint32_t)temperatureC };
    Shared_TrySendDisplay(&msg);

        vTaskDelay(pdMS_TO_TICKS(500)); // Check every 500ms
    }
}