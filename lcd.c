/* 
 * lcd.c
 * I2C LCD Driver for 16x2 LCD Display
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "tm4c123gh6pm.h"
#include "lcd.h"

/* LCD I2C Address */
#define LCD_I2C_ADDRESS        0x27  // Typical address for PCF8574 I2C LCD backpack

/* LCD Commands */
#define LCD_CLEARDISPLAY       0x01
#define LCD_RETURNHOME         0x02
#define LCD_ENTRYMODESET       0x04
#define LCD_DISPLAYCONTROL     0x08
#define LCD_CURSORSHIFT        0x10
#define LCD_FUNCTIONSET        0x20
#define LCD_SETCGRAMADDR       0x40
#define LCD_SETDDRAMADDR       0x80

/* LCD Entry Mode Flags */
#define LCD_ENTRYRIGHT         0x00
#define LCD_ENTRYLEFT          0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

/* LCD Display Control Flags */
#define LCD_DISPLAYON          0x04
#define LCD_DISPLAYOFF         0x00
#define LCD_CURSORON           0x02
#define LCD_CURSOROFF          0x00
#define LCD_BLINKON            0x01
#define LCD_BLINKOFF           0x00

/* LCD Function Set Flags */
#define LCD_8BITMODE           0x10
#define LCD_4BITMODE           0x00
#define LCD_2LINE              0x08
#define LCD_1LINE              0x00
#define LCD_5x10DOTS           0x04
#define LCD_5x8DOTS            0x00

/* LCD Backlight Control Flags */
#define LCD_BACKLIGHT          0x08
#define LCD_NOBACKLIGHT        0x00

/* LCD Enable Bit */
#define LCD_EN                 0x04  // Enable bit
#define LCD_RW                 0x02  // Read/Write bit
#define LCD_RS                 0x01  // Register select bit

/* Private variables */
static uint8_t displayFunction;
static uint8_t displayControl;
static uint8_t displayMode;
static uint8_t backlightVal = LCD_BACKLIGHT;

/* Private function prototypes */
static void LCD_WriteI2C(uint8_t data);
static void LCD_WriteCommand(uint8_t command);
static void LCD_WriteData(uint8_t data);
static void LCD_Write4Bits(uint8_t value);
static void LCD_PulseEnable(uint8_t data);
static void LCD_Delay(uint32_t milliseconds);

/* Initialize I2C0 for LCD communication */
void InitI2C(void) {
    // Enable I2C0 clock
    SYSCTL_RCGCI2C_R |= SYSCTL_RCGCI2C_R0;
    
    // Enable GPIO Port B clock (for I2C pins)
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;
    
    // Wait for I2C0 and GPIO Port B to be ready
    while ((SYSCTL_PRI2C_R & SYSCTL_PRI2C_R0) != SYSCTL_PRI2C_R0) {}
    while ((SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R1) != SYSCTL_PRGPIO_R1) {}
    
    // Configure PB2 (SCL) and PB3 (SDA) for I2C
    GPIO_PORTB_AFSEL_R |= (1 << 2) | (1 << 3);  // Enable alternate function
    GPIO_PORTB_ODR_R |= (1 << 3);               // Enable open drain for SDA
    GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & ~0x0000FF00) | 
                        (GPIO_PCTL_PB2_I2C0SCL | GPIO_PCTL_PB3_I2C0SDA); // Configure as I2C
    GPIO_PORTB_DEN_R |= (1 << 2) | (1 << 3);    // Enable digital function
    
    // Disable I2C0 during configuration
    I2C0_MCR_R = 0;
    
    // Configure I2C0 clock
    // System clock is 80MHz, I2C clock should be 100kHz
    // TPR = (System_Clock/(2*(SCL_LP + SCL_HP)*SCL_CLK))-1
    // TPR = (80MHz/(2*(6+4)*100kHz))-1 = 39
    I2C0_MTPR_R = 39;
    
    // Enable I2C0 master function
    I2C0_MCR_R = I2C_MCR_MFE;
}

/* 
 * Initialize I2C LCD
 */
void LCD_Init(void) {
    // Initialize I2C
    InitI2C();
    
    // Initialize display variables
    displayFunction = LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS;
    displayControl = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
    displayMode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
    
    // Wait for LCD to power up
    LCD_Delay(50);
    
    // Start in 8-bit mode
    LCD_WriteI2C(0x00);
    LCD_Delay(50);
    
    // Put LCD into 4-bit mode
    LCD_Write4Bits(0x03);
    LCD_Delay(5);
    LCD_Write4Bits(0x03);
    LCD_Delay(5);
    LCD_Write4Bits(0x03);
    LCD_Delay(1);
    LCD_Write4Bits(0x02);
    
    // Set # of lines and font size
    LCD_WriteCommand(LCD_FUNCTIONSET | displayFunction);
    
    // Turn the display on with no cursor or blinking
    LCD_WriteCommand(LCD_DISPLAYCONTROL | displayControl);
    
    // Clear the display
    LCD_Clear();
    
    // Set the entry mode
    LCD_WriteCommand(LCD_ENTRYMODESET | displayMode);
    
    // Return home
    LCD_WriteCommand(LCD_RETURNHOME);
    LCD_Delay(2);
}

/*
 * Clear the LCD display
 */
void LCD_Clear(void) {
    LCD_WriteCommand(LCD_CLEARDISPLAY);
    LCD_Delay(2);  // This command takes a long time
}

/*
 * Set the cursor position
 * col: Column (0-15)
 * row: Row (0-1)
 */
void LCD_SetCursor(uint8_t col, uint8_t row) {
    static const uint8_t rowOffsets[] = {0x00, 0x40};
    
    // Keep row within bounds
    if (row > 1) {
        row = 1;
    }
    
    // Send the set DDRAM address command
    LCD_WriteCommand(LCD_SETDDRAMADDR | (col + rowOffsets[row]));
}

/*
 * Print a string to the LCD
 * str: String to print
 */
void LCD_Print(const char* str) {
    while (*str) {
        LCD_WriteData(*str++);
    }
}

/*
 * Turn the display on/off
 * on: true to turn on, false to turn off
 */
void LCD_DisplayOn(bool on) {
    if (on) {
        displayControl |= LCD_DISPLAYON;
    } else {
        displayControl &= ~LCD_DISPLAYON;
    }
    LCD_WriteCommand(LCD_DISPLAYCONTROL | displayControl);
}

/*
 * Turn the backlight on/off
 * on: true to turn on, false to turn off
 */
void LCD_BacklightOn(bool on) {
    if (on) {
        backlightVal = LCD_BACKLIGHT;
    } else {
        backlightVal = LCD_NOBACKLIGHT;
    }
    LCD_WriteI2C(backlightVal);
}

/*
 * Write a byte to the I2C LCD
 * data: Byte to write
 */
static void LCD_WriteI2C(uint8_t data) {
    // Wait until I2C module is idle
    while (I2C0_MCS_R & I2C_MCS_BUSY) {}
    
    // Set slave address and R/W bit (0 for write)
    I2C0_MSA_R = (LCD_I2C_ADDRESS << 1) & 0xFE;
    
    // Load data to transmit
    I2C0_MDR_R = data;
    
    // Generate START, TRANSMIT, STOP conditions
    I2C0_MCS_R = I2C_MCS_START | I2C_MCS_RUN | I2C_MCS_STOP;
    
    // Wait until I2C operation is complete
    while (I2C0_MCS_R & I2C_MCS_BUSY) {}
}

/*
 * Write a command to the LCD
 * command: Command to write
 */
static void LCD_WriteCommand(uint8_t command) {
    LCD_Write4Bits(command >> 4);  // Send high nibble
    LCD_Write4Bits(command);       // Send low nibble
}

/*
 * Write data to the LCD
 * data: Data to write
 */
static void LCD_WriteData(uint8_t data) {
    LCD_Write4Bits((data >> 4) | LCD_RS);  // Send high nibble with RS set
    LCD_Write4Bits(data | LCD_RS);         // Send low nibble with RS set
}

/*
 * Write 4 bits to the LCD
 * value: 4 bits to write
 */
static void LCD_Write4Bits(uint8_t value) {
    // Combine the 4 bits with the backlight value
    value = (value << 4) | backlightVal;
    LCD_PulseEnable(value);
}

/*
 * Pulse the enable pin on the LCD
 * data: Data with the enable bit to pulse
 */
static void LCD_PulseEnable(uint8_t data) {
    // Send data with EN bit low
    LCD_WriteI2C(data & ~LCD_EN);
    LCD_Delay(1);
    
    // Send data with EN bit high
    LCD_WriteI2C(data | LCD_EN);
    LCD_Delay(1);
    
    // Send data with EN bit low again
    LCD_WriteI2C(data & ~LCD_EN);
    LCD_Delay(1);
}

/*
 * Delay function using SysTick
 * milliseconds: Number of milliseconds to delay
 */
static void LCD_Delay(uint32_t milliseconds) {
    // Simple busy-wait delay function
    // This is not very accurate and in a real system, you should use a timer
    volatile uint32_t ui32Loop;
    for (ui32Loop = 0; ui32Loop < milliseconds * 16000; ui32Loop++) {}
}
