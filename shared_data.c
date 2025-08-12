#include "shared_data.h"

QueueHandle_t gDisplayQueue = NULL;
SemaphoreHandle_t gI2CMutex = NULL;
SemaphoreHandle_t gADCMutex = NULL;

void Shared_Init(void)
{
    if (gDisplayQueue == NULL) {
        gDisplayQueue = xQueueCreate(8, sizeof(DisplayMsg_t));
    }
    if (gI2CMutex == NULL) {
        gI2CMutex = xSemaphoreCreateMutex();
    }
    if (gADCMutex == NULL) {
        gADCMutex = xSemaphoreCreateMutex();
    }
}
