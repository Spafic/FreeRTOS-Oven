# Industrial Oven Control System

## Overview

This project implements an industrial oven control system using the TM4C123GH6PM microcontroller and FreeRTOS. The system provides the following functionalities:

- Temperature control using a bang-bang (ON-OFF) controller
- Door control based on light intensity detection
- Battery level monitoring with alarm functionality
- Real-time display of system status on an LCD

## Hardware Components

- **Microcontroller**: Tiva C TM4C123GH6PM
- **Input Devices**:
  - Temperature Sensor (Potentiometer simulation on PE3)
  - Light Intensity Sensor (Potentiometer simulation on PE2)
  - Battery Level Sensor (Potentiometer simulation on PE1)
- **Output Devices**:
  - Heater (LED simulation on PF1)
  - LCD Display (16x2 I2C connected to PB2-SCL, PB3-SDA)
  - Low Battery Alarm (Buzzer on PF2)
  - Door Status LED (Green LED on PF3)

## Software Architecture

The system is built on FreeRTOS and consists of the following tasks:

1. **Temperature Control Task (Priority 3)**

   - Reads temperature sensor and controls heater (ON/OFF)
   - Period: 500ms

2. **Door Control Task (Priority 2)**

   - Reads light sensor and controls door status indication
   - Period: 300ms

3. **Battery Monitor Task (Priority 2)**

   - Monitors battery level and triggers alarm if low
   - Period: 1000ms

4. **LCD Display Task (Priority 1)**

   - Updates LCD with system information
   - Period: 1000ms

5. **Buzzer Control Task (Priority 3)**
   - Controls buzzer for low battery alarm
   - Event-driven (semaphore-based)

## Inter-Task Communication

- **Queues**:

  - `temperatureQueue`: Temperature data from Temperature Control Task to LCD Task
  - `doorQueue`: Door status from Door Control Task to LCD Task
  - `batteryQueue`: Battery level from Battery Monitor Task to LCD Task

- **Semaphores**:
  - `buzzerSemaphore`: Binary semaphore for buzzer activation
  - `adcMutex`: Mutex for ADC resource sharing between sensor tasks

## Files in the Project

- **main.c**: Main application code and task definitions
- **hardware_init.c/h**: Hardware initialization and control functions
- **lcd.c/h**: I2C LCD driver
- **startup_tm4c123.c**: Startup code with interrupt vector table
- **FreeRTOSConfig_override.h**: FreeRTOS configuration overrides
- **tm4c123gh6pm.h**: TM4C123GH6PM register definitions

## Build and Flash

1. Open the project in Keil µVision
2. Build the project (F7)
3. Flash the program to the TM4C123GH6PM microcontroller

## Control Logic

- **Temperature Control**: Bang-bang controller turns heater on below setpoint and off above setpoint
- **Door Control**: Door opens when light intensity is low (obstruction detected) and closes when path is clear
- **Battery Monitor**: Triggers alarm when battery level falls below threshold

## LCD Display Format

- Line 1: "Temp:XXX°C Batt:XX%"
- Line 2: "Door:OPEN/CLOSED Heat:ON/OFF"

## Hardware Connections

| Component      | Pin          | Function       |
| -------------- | ------------ | -------------- |
| Temp Sensor    | PE3 (AIN0)   | ADC input      |
| Light Sensor   | PE2 (AIN1)   | ADC input      |
| Battery Sensor | PE1 (AIN2)   | ADC input      |
| Heater LED     | PF1          | Digital output |
| Status LED     | PF3          | Digital output |
| Buzzer         | PF2          | Digital output |
| LCD SDA        | PB3          | I2C data       |
| LCD SCL        | PB2          | I2C clock      |
