// Simple I2C LCD display task for TM4C123 + PCF8574 backpack
#include <stdint.h>
#include "tm4c.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "shared_data.h"
#include "display_config.h"
#include <stdint.h>
extern uint32_t SystemCoreClock; // from system_TM4C123.c
static uint8_t s_lcd_addr = LCD_PCF8574_ADDR;

// Reserve I2C0 on PB2 (SCL), PB3 (SDA) for LCD
// Ensure other modules do not use PB2/PB3.

#define I2C0_BASE_EN() do { \
	SYSCTL_RCGCGPIO_R |= (1U<<1); /* Port B */ \
	SYSCTL_RCGCI2C_R  |= (1U<<0); /* I2C0 */ \
	(void)SYSCTL_RCGCGPIO_R; (void)SYSCTL_RCGCI2C_R; \
	while ((SYSCTL_PRGPIO_R & (1U<<1)) == 0) {} \
} while(0)

static void I2C0_Init(void)
{
	I2C0_BASE_EN();
	// Configure PB2 SCL, PB3 SDA alt func
	GPIO_PORTB_AFSEL_R |= (1U<<2) | (1U<<3);
	GPIO_PORTB_DEN_R   |= (1U<<2) | (1U<<3);
	GPIO_PORTB_ODR_R   |= (1U<<3); // SDA open-drain
	// PCTL: assign I2C (0x3) to PB2,PB3
	GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & ~(0xFF << 8)) | (0x33 << 8);

	// Reset and configure I2C0 master
	I2C0_MASTER_MCR_R = 0x10; // Master enable
	// TPR = (SysClk/(2*(SCL_LP+SCL_HP)*SCL_CLK)) - 1; with LP+HP=10 for standard
	uint32_t sysclk = (uint32_t)SystemCoreClock;
	if (sysclk == 0) sysclk = 16000000U; // fallback
	uint32_t tpr = (sysclk / (20U * (uint32_t)I2C0_SPEED_HZ));
	if (tpr > 0) tpr -= 1U;
	if (tpr > 127U) tpr = 127U; // TPR is 7 bits
	I2C0_MASTER_MTPR_R = tpr;
}

// PCF8574 address (commonly 0x27 or 0x3F). Use 0x27 by default.
#ifndef LCD_PCF8574_ADDR
#define LCD_PCF8574_ADDR 0x27
#endif

// Minimal LCD over I2C write (4-bit)
static int I2C0_WriteByte(uint8_t addr7, uint8_t data)
{
	// mutex must be held by caller
	I2C0_MASTER_MSA_R = (addr7 << 1) | 0; // write
	I2C0_MASTER_MDR_R = data;
	I2C0_MASTER_MCS_R = 0x07; // START + RUN + STOP
	uint32_t guard = 0; 
	while ((I2C0_MASTER_MCS_R & 0x01) && guard++ < 100000U) { }
	if (I2C0_MASTER_MCS_R & 0x02) return -1; // error
	if (I2C0_MASTER_MCS_R & 0x04) return -2; // NACK
	return 0;
}

static void LCD_Pulse(uint8_t data)
{
	I2C0_WriteByte(s_lcd_addr, data | 0x04); // E=1
	vTaskDelay(pdMS_TO_TICKS(1));
	I2C0_WriteByte(s_lcd_addr, data & ~0x04); // E=0
}

static void LCD_Send4(uint8_t nibble, uint8_t ctrl)
{
	uint8_t d = (nibble & 0xF0) | ctrl | 0x08; // backlight on
	LCD_Pulse(d);
}

static void LCD_SendCmd(uint8_t cmd)
{
	LCD_Send4(cmd & 0xF0, 0x00);
	LCD_Send4((cmd << 4) & 0xF0, 0x00);
	vTaskDelay(pdMS_TO_TICKS(2));
}

static void LCD_SendData(uint8_t ch)
{
	LCD_Send4(ch & 0xF0, 0x01); // RS=1
	LCD_Send4((ch << 4) & 0xF0, 0x01);
}

static void LCD_Init(void)
{
	// Assumes I2C0_Init already called and mutex created
	vTaskDelay(pdMS_TO_TICKS(50));
	LCD_Send4(0x30, 0x00); vTaskDelay(pdMS_TO_TICKS(5));
	LCD_Send4(0x30, 0x00); vTaskDelay(pdMS_TO_TICKS(5));
	LCD_Send4(0x20, 0x00); vTaskDelay(pdMS_TO_TICKS(5)); // 4-bit mode
	LCD_SendCmd(0x28); // 2 lines, 5x8 dots
	LCD_SendCmd(0x0C); // display on
	LCD_SendCmd(0x06); // entry mode
	LCD_SendCmd(0x01); vTaskDelay(pdMS_TO_TICKS(2)); // clear
}

static void LCD_SetCursor(uint8_t row, uint8_t col)
{
	uint8_t addr = (row ? 0x40 : 0x00) + col;
	LCD_SendCmd(0x80 | addr);
}

static void LCD_PrintStr(const char *s)
{
	while (*s) { LCD_SendData((uint8_t)*s++); }
}

static void Display_Render(const DisplayMsg_t *msg)
{
	if (xSemaphoreTake(gI2CMutex, pdMS_TO_TICKS(20)) != pdTRUE) return;
	static uint8_t bat_alert = 0; // 0/1
	static uint8_t temp_alert = 0; // 0/1
	switch (msg->field) {
		case DISP_FIELD_BATTERY: {
			LCD_SetCursor(LCD_POS_BATTERY_ROW, LCD_POS_BATTERY_COL);
			char buf[16];
			int pct = (int)msg->value;
			// Minimal itoa
			buf[0] = 'B'; buf[1] = 'a'; buf[2] = 't'; buf[3] = ':'; buf[4] = ' ';
			int i = 5;
			if (pct >= 100) { buf[i++]='1'; buf[i++]='0'; buf[i++]='0'; }
			else {
				if (pct >= 10) { buf[i++] = '0' + (pct/10); }
				buf[i++] = '0' + (pct%10);
			}
			buf[i++] = '%'; buf[i]=0;
			LCD_PrintStr("                ");
			LCD_SetCursor(LCD_POS_BATTERY_ROW, LCD_POS_BATTERY_COL);
			LCD_PrintStr(buf);
			break;
		}
		case DISP_FIELD_TEMPERATURE: {
			LCD_SetCursor(LCD_POS_TEMP_ROW, LCD_POS_TEMP_COL);
			int t = (int)msg->value; // C as integer
			char buf[16];
			buf[0]='T';buf[1]='e';buf[2]='m';buf[3]='p';buf[4]=':';buf[5]=' ';
			int i=6;
			if (t >= 100) { buf[i++]='1'; buf[i++]='0'; buf[i++]='0'; }
			else {
				if (t >= 10) { buf[i++]='0'+(t/10); }
				buf[i++]='0'+(t%10);
			}
			buf[i++]='C'; buf[i]=0;
			LCD_PrintStr("                ");
			LCD_SetCursor(LCD_POS_TEMP_ROW, LCD_POS_TEMP_COL);
			LCD_PrintStr(buf);
			break;
		}
		case DISP_FIELD_DOOR: {
			LCD_SetCursor(LCD_POS_DOOR_ROW, LCD_POS_DOOR_COL);
			if (msg->value) LCD_PrintStr("Door:Open ");
			else LCD_PrintStr("Door:Close");
			break;
		}
		case DISP_FIELD_ALERT: {
			// Alert protocol: 1=set BAT, 101=clear BAT, 2=set TEMP, 102=clear TEMP
			if (msg->value == 1) bat_alert = 1;
			else if (msg->value == 101) bat_alert = 0;
			else if (msg->value == 2) temp_alert = 1;
			else if (msg->value == 102) temp_alert = 0;
			// Re-render alert region based on current state
			LCD_SetCursor(LCD_POS_ALERT_ROW, LCD_POS_ALERT_COL);
			if (bat_alert) { LCD_PrintStr("BAT!"); }
			else if (temp_alert) { LCD_PrintStr("TEMP!"); }
			else { LCD_PrintStr("    "); }
			break;
		}
		default: break;
	}
	xSemaphoreGive(gI2CMutex);
}

static void Display_Task(void *pv)
{
	(void)pv;
	I2C0_Init();
	// Ensure mutex exists
	if (gI2CMutex == NULL) gI2CMutex = xSemaphoreCreateMutex();
	if (xSemaphoreTake(gI2CMutex, pdMS_TO_TICKS(50)) == pdTRUE) {
		// Try default address, then alternate, to avoid blank displays
		int addrCandidates[2] = { LCD_PCF8574_ADDR, (LCD_PCF8574_ADDR==0x27?0x3F:0x27) };
		for (int i=0; i<2; ++i) {
			uint8_t probe = 0x08; // backlight only
			int rc = I2C0_WriteByte((uint8_t)addrCandidates[i], probe);
			if (rc == 0) { s_lcd_addr = (uint8_t)addrCandidates[i]; break; }
		}
		LCD_Init();
		xSemaphoreGive(gI2CMutex);
	}
	DisplayMsg_t msg;
	for (;;)
	{
		if (xQueueReceive(gDisplayQueue, &msg, portMAX_DELAY) == pdTRUE) {
			Display_Render(&msg);
		}
	}
}

// Expose a creator function
void Display_CreateTask(void)
{
	xTaskCreate(Display_Task, "Display", STACK_DISPLAY_TASK, NULL, PRIO_DISPLAY_TASK, NULL);
}

