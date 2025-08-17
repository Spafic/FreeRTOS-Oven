/*
 * lcd.h
 * I2C LCD Driver Header for 16x2 LCD Display
 */

#ifndef LCD_H
#define LCD_H

#include <stdint.h>
#include <stdbool.h>

/* Function prototypes */
void InitI2C(void);
void LCD_Init(void);
void LCD_Clear(void);
void LCD_SetCursor(uint8_t col, uint8_t row);
void LCD_Print(const char* str);
void LCD_DisplayOn(bool on);
void LCD_BacklightOn(bool on);

#endif /* LCD_H */
