#ifndef DISPLAY_CONFIG_H
#define DISPLAY_CONFIG_H

#include <stdint.h>

// I2C LCD config
#ifndef LCD_PCF8574_ADDR
#define LCD_PCF8574_ADDR 0x27  // Change to 0x3F if needed
#endif

#ifndef I2C0_SPEED_HZ
#define I2C0_SPEED_HZ 100000U  // 100kHz
#endif

// LCD layout (16x2)
#define LCD_POS_BATTERY_ROW 0
#define LCD_POS_BATTERY_COL 0

#define LCD_POS_DOOR_ROW    0
#define LCD_POS_DOOR_COL    9  // enough space after "Bat:XX% "

#define LCD_POS_TEMP_ROW    1
#define LCD_POS_TEMP_COL    0

#define LCD_POS_ALERT_ROW   1
#define LCD_POS_ALERT_COL   10

#endif
