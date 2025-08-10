#include <stdint.h>
#include "tm4c.h"
#include "FreeRTOS.h"
#include "task.h"
#include "sensors.h"

#define TEMP_THRESHOLD 200.0f // Example threshold value

void Temperature_Task(void *pvParameters) {
    (void)pvParameters;

    // Enable clock for Port F
    SYSCTL_RCGCGPIO_R |= (1U << 5);
    while ((SYSCTL_PRGPIO_R & (1U << 5)) == 0);

    // PF2 (Blue LED) as output
    GPIO_PORTF_DIR_R |= (1U << 2);
    GPIO_PORTF_DEN_R |= (1U << 2);

    while (1) {
        float temperatureC = Sensors_ReadTemperatureC();

        if (temperatureC > TEMP_THRESHOLD) {
            GPIO_PORTF_DATA_R |= (1U << 2);  // Blue LED ON
        } else {
            GPIO_PORTF_DATA_R &= ~(1U << 2); // Blue LED OFF
        }

        vTaskDelay(pdMS_TO_TICKS(500)); // Check every 500ms
    }
}