#ifndef DISPLAY_TASK_H
#define DISPLAY_TASK_H

#include <stdint.h>

// Optionally override in project settings
#ifndef LCD_PCF8574_ADDR
#define LCD_PCF8574_ADDR 0x27
#endif

// Optional configuration via display_config.h
#include "display_config.h"

void Display_CreateTask(void);

#endif

