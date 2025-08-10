#include "tm4c.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdint.h>
#include "door_task.h"

// Threshold for light sensor
#define LIGHT_THRESHOLD 2000

void PWM0_Init(void);
void Servo_SetAngle(int angle);
void LED_Init(void);
void LED_Set(int on);
void ADC0_Init(void);
uint16_t ADC0_Read(void);

void Door_Task(void *pvParameters) {
		(void)pvParameters;
    uint16_t lightValue;
    while(1) {
        lightValue = ADC0_Read();
        if(lightValue < LIGHT_THRESHOLD) {
            Servo_SetAngle(0);   // Door open
            LED_Set(1);          // LED ON
        } else {
            Servo_SetAngle(90);  // Door closed
            LED_Set(0);          // LED OFF
        }
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

void PWM0_Init(void) {
    SYSCTL_RCGCPWM_R |= 0x01;    // Enable PWM0
    SYSCTL_RCGCGPIO_R |= 0x02;   // Enable Port B
    SYSCTL_RCC_R |= (1<<20);     // Use PWM divider
    SYSCTL_RCC_R = (SYSCTL_RCC_R & ~0x000E0000) | 0x00060000; // /64

    GPIO_PORTB_AFSEL_R |= (1<<6);
    GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & ~0x0F000000) | 0x04000000;
    GPIO_PORTB_DEN_R |= (1<<6);

    PWM0_0_CTL_R = 0;
    PWM0_0_GENA_R = 0x0000008C;
    PWM0_0_LOAD_R = 25000 - 1;  // 50Hz
    PWM0_0_CMPA_R = 1875;       // ~1.5ms (90°)
    PWM0_0_CTL_R = 1;
    PWM0_ENABLE_R |= 0x01;
}

void Servo_SetAngle(int angle) {
    int cmp = 1250 + (angle * (2500 - 1250) / 180); // 1ms to 2ms
    PWM0_0_CMPA_R = cmp;
}

void LED_Init(void) {
    SYSCTL_RCGCGPIO_R |= 0x20;
    GPIO_PORTF_DIR_R |= (1<<1);
    GPIO_PORTF_DEN_R |= (1<<1);
}

void LED_Set(int on) {
    if(on) GPIO_PORTF_DATA_R |= (1<<1);
    else GPIO_PORTF_DATA_R &= ~(1<<1);
}

void ADC0_Init(void) {
    SYSCTL_RCGCADC_R |= 1;
    SYSCTL_RCGCGPIO_R |= 0x10;
    GPIO_PORTE_AFSEL_R |= (1<<3);
    GPIO_PORTE_DEN_R &= ~(1<<3);
    GPIO_PORTE_AMSEL_R |= (1<<3);

    ADC0_ACTSS_R &= ~8;
    ADC0_EMUX_R &= ~0xF000;
    ADC0_SSMUX3_R = 0;
    ADC0_SSCTL3_R = 6;
    ADC0_ACTSS_R |= 8;
}

uint16_t ADC0_Read(void) {
    ADC0_PSSI_R = 8;
    while((ADC0_RIS_R & 8) == 0);
    uint16_t result = ADC0_SSFIFO3_R & 0xFFF;
    ADC0_ISC_R = 8;
    return result;
}
