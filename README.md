# Industrial Oven Control System

<div align="center">

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](https://github.com/Spafic/FreeRTOS-Oven/blob/main/LICENSE)
![Platform](https://img.shields.io/badge/platform-ARM%20Cortex--M4-green.svg)
![IDE](https://img.shields.io/badge/IDE-Keil%20%C2%B5Vision-orange.svg)
![FreeRTOS](https://img.shields.io/badge/FreeRTOS-v10.4.1-red.svg)
![Language](https://img.shields.io/github/languages/top/Spafic/FreeRTOS-Oven?color=informational)
[![Stars](https://img.shields.io/github/stars/Spafic/FreeRTOS-Oven?style=social)](https://github.com/Spafic/FreeRTOS-Oven/stargazers)
![Last Commit](https://img.shields.io/github/last-commit/Spafic/FreeRTOS-Oven)

**A comprehensive industrial oven control system using FreeRTOS**  
_CSE411s RTOS Course - Summer 2025_

Real-Time Control • Multi-Task Architecture • Industrial Safety

</div>

---

This project implements a comprehensive industrial oven control system using the TM4C123GH6PM microcontroller and FreeRTOS real-time operating system. The system provides precise temperature control, intelligent door management, proactive battery monitoring, and real-time status display through a multi-task architecture.

> [!CAUTION]
> **KEIL µVision FreeRTOSConfig.h Version Issue**: KEIL5 may install FreeRTOSConfig.h version 2021 by default, which can cause system instability and hangs. It is strongly recommended to use the **2019 version** of FreeRTOSConfig.h for stable operation with this project.

## Project Overview

The Industrial Oven Control System demonstrates advanced concepts in real-time systems design, embedded programming, and industrial automation. The system is designed to be scalable, maintainable, and suitable for industrial applications where reliability and real-time performance are critical.

### Key Features

- **Multi-task Real-time Operation**: 5 concurrent tasks with priority-based scheduling
- **Precise Temperature Control**: Bang-bang controller with hysteresis band
- **Safety Monitoring**: Light-based door obstruction detection with fail-safe operation
- **Power Management**: Proactive battery level monitoring with audible alarms
- **User Interface**: Real-time system status display on 16x2 I2C LCD
- **Resource Protection**: Mutex-protected ADC resource sharing
- **Inter-task Communication**: Queue-based data exchange between tasks
- **Event-driven Architecture**: Semaphore-based alarm system

## System Architecture

### Overall Design

The system follows a modular architecture based on five key design principles:

1. **Separation of Concerns**: Each task handles specific functionality
2. **Priority-Based Scheduling**: Critical tasks receive higher priorities
3. **Resource Sharing**: Mutex protection for shared ADC access
4. **Event-Driven Architecture**: Semaphore-based buzzer control
5. **Data Encapsulation**: Structured data types for task communication

### Task Design and Priorities

| Task Name | Priority | Period (ms) | Stack Size | Purpose |
|-----------|----------|-------------|------------|---------|
| Temperature Control | 3 (High) | 500 | 128 words | Critical temperature monitoring and heater control |
| Buzzer Control | 3 (High) | Event-driven | 128 words | Safety alarm response |
| Door Control | 2 (Medium) | 300 | 128 words | Door safety control and obstruction detection |
| Battery Monitor | 2 (Medium) | 1000 | 128 words | Power level monitoring and alerts |
| LCD Display | 1 (Low) | 1000 | 128 words | User interface updates |

### Communication Architecture

```
Temperature Task ──┐
                   ├─► temperatureQueue ──┐
Door Task ─────────┼─► doorQueue ─────────┼─► LCD Display Task
                   ├─► batteryQueue ──────┘
Battery Task ──────┼─► buzzerSemaphore ──► Buzzer Task
                   └─► adcMutex (shared resource protection)
```

### FreeRTOS Configuration

Key configuration parameters for optimal performance:

```c
#define configCPU_CLOCK_HZ              (SystemCoreClock)
#define configTICK_RATE_HZ              ((TickType_t)1000)
#define configTOTAL_HEAP_SIZE          ((size_t)10*1024)
#define configMAX_PRIORITIES           5
#define configUSE_PREEMPTION           1
#define configUSE_MUTEXES              1
#define configUSE_COUNTING_SEMAPHORES  1
```

## Hardware Requirements

### Component List

| Component | Part Number/Type | Quantity | Purpose |
|-----------|------------------|----------|---------|
| Microcontroller | TM4C123GH6PM | 1 | Main control unit |
| Temperature Sensor | Potentiometer (10kΩ) | 1 | Temperature simulation |
| Light Sensor | Potentiometer (10kΩ) | 1 | Light intensity simulation |
| Battery Sensor | Potentiometer (10kΩ) | 1 | Battery level simulation |
| LCD Display | 16x2 I2C LCD | 1 | Status display |
| LEDs | Standard 5mm LEDs | 3 | Status indicators |
| Buzzer | Active Buzzer | 1 | Low battery alarm |
| Resistors | 220Ω | 3 | Current limiting |

### Pin Assignment

| Function | TM4C123 Pin | GPIO/Peripheral | Connection |
|----------|-------------|-----------------|------------|
| Temperature ADC | PE3 | AIN0 | Potentiometer wiper |
| Light ADC | PE2 | AIN1 | Potentiometer wiper |
| Battery ADC | PE1 | AIN2 | Potentiometer wiper |
| Heater LED | PF1 | GPIO Output | Red LED + 220Ω resistor |
| Battery LED | PF2 | GPIO Output | Blue LED + 220Ω resistor |
| Buzzer | PF2 | GPIO Output | Active buzzer |
| Door LED | PF3 | GPIO Output | Green LED + 220Ω resistor |
| LCD SDA | PB3 | I2C0SDA | I2C data line |
| LCD SCL | PB2 | I2C0SCL | I2C clock line |

## Quick Start Guide

### Prerequisites

1. **Development Environment**:
   - Keil µVision MDK-ARM 5.37 or later
   - TM4C123GH6PM device support pack
   - ARM Compiler 6 (recommended)

2. **Hardware Setup**:
   - TM4C123GH6PM LaunchPad
   - Breadboard and connecting wires
   - Components listed in hardware requirements

### Installation and Setup

```bash
# 1. Clone the repository
git clone https://github.com/Spafic/FreeRTOS-Oven.git
cd FreeRTOS-Oven

# 2. Open project in Keil µVision
# Open: Oven.uvprojx
```

### Configuration Steps

1. **FreeRTOS Version Setup** (Critical):
   - In Keil µVision: Project → Manage → Run-Time Environment
   - Select **CMSIS-RTOS2 → FreeRTOS version 2019**
   - Uncheck any 2021 versions
   - Apply changes and regenerate project files

2. **Hardware Connections**:
   - Connect potentiometers to ADC pins (PE1, PE2, PE3)
   - Wire LEDs to GPIO outputs (PF1, PF2, PF3) with 220Ω resistors
   - Connect I2C LCD to PB2 (SCL) and PB3 (SDA)
   - Connect active buzzer to PF2

3. **Build and Flash**:
   - Build project (F7)
   - Flash and debug (Ctrl+F5)
   - Verify system initialization and task execution

## Control Logic Implementation

### Temperature Control

The system implements a bang-bang controller with hysteresis to prevent oscillations:

```c
#define TEMP_SETPOINT_LOW   25.0f  // Turn heater ON below this
#define TEMP_SETPOINT_HIGH  30.0f  // Turn heater OFF above this
```

**Control Characteristics**:
- Setpoint Range: 25°C - 30°C (5°C hysteresis band)
- Response Time: < 500ms
- Visual feedback through status LED

### Door Control

Obstruction detection based on light intensity:

```c
#define LIGHT_THRESHOLD 50.0f  // 50% light intensity threshold
```

**Safety Features**:
- Fail-safe operation: Door opens when sensor fails
- Visual status indication
- Error detection and reporting

### Battery Monitoring

Low battery detection with audible alarm:

```c
#define BATTERY_LOW_THRESHOLD 20.0f  // 20% battery level
```

**Alarm System**:
- Event-driven activation via binary semaphore
- Two-tone alarm pattern
- Immediate response to low battery conditions

## File Structure

```
FreeRTOS-Oven/
├── main.c                    # Main application and task creation
├── hardware_init.c/.h        # Hardware initialization
├── gpio_init.c/.h            # GPIO configuration
├── adc_control.c/.h          # ADC driver and sensor reading
├── lcd.c/.h                  # I2C LCD driver
├── temperature_task.c/.h     # Temperature control implementation
├── door_task.c/.h            # Door control implementation
├── battery_task.c/.h         # Battery monitoring implementation
├── lcd_task.c/.h             # LCD display task
├── buzzer_task.c/.h          # Buzzer control implementation
└── RTE/RTOS/                 # FreeRTOS configuration
    └── FreeRTOSConfig.h
```

## Advanced Features

### Critical Section Protection

The system uses mutex-based protection for shared ADC resource:

```c
// ADC access pattern
if (xSemaphoreTake(adcMutex, portMAX_DELAY) == pdTRUE) {
    sensorValue = ReadSensor();
    xSemaphoreGive(adcMutex);
}
```

### Inter-Task Communication

Structured data types for reliable task communication:

```c
typedef struct {
    float temperature;
    bool heaterActive;
} TemperatureData;

typedef struct {
    bool doorOpen;
    bool sensorError;
} DoorData;

typedef struct {
    float batteryLevel;
} BatteryData;
```

### Event-Driven Architecture

Binary semaphores for immediate alarm response:

```c
// Battery task triggers alarm
if (batteryLevel < BATTERY_LOW_THRESHOLD) {
    xSemaphoreGive(buzzerSemaphore);
}

// Buzzer task responds immediately
if (xSemaphoreTake(buzzerSemaphore, portMAX_DELAY) == pdTRUE) {
    // Execute alarm sequence
}
```

## Troubleshooting

### Common Issues

#### FreeRTOS Configuration
- **System Hangs**: Ensure FreeRTOS 2019 version is used
- **Task Creation Fails**: Check heap size and stack allocation
- **Scheduler Issues**: Verify priority assignments

#### Hardware Issues
- **ADC Reading Errors**: Check potentiometer connections and voltage levels
- **LCD Display Problems**: Verify I2C connections and pull-up resistors
- **LED/Buzzer Issues**: Check GPIO configuration and current limiting resistors

#### Resource Contention
- **Data Inconsistency**: Ensure proper mutex usage for ADC access
- **Priority Inversion**: Verify task priorities and mutex timeout values
- **Memory Issues**: Monitor stack usage and heap allocation

### Debug Tips

- Use breakpoints strategically to understand task execution flow
- Monitor queue and semaphore states during runtime
- Utilize memory windows to inspect task stacks
- Enable FreeRTOS trace functionality for advanced debugging

## Educational Value

### Learning Objectives

This project demonstrates:

1. **Real-Time Systems Design**: Understanding of timing constraints and deterministic behavior
2. **Multi-Task Programming**: Task creation, priority assignment, and scheduling
3. **Resource Management**: Mutex usage and critical section protection
4. **Inter-Process Communication**: Queues and semaphores for task coordination
5. **Hardware Integration**: ADC, GPIO, and I2C peripheral programming
6. **Control Systems**: Implementation of feedback control loops
7. **Safety Systems**: Fail-safe design and error handling

### Advanced Concepts

- Priority inheritance and deadlock prevention
- Memory management in embedded systems
- Industrial automation principles
- System integration and testing methodologies

## Contributing

### For Students

This project serves as an educational reference. Students should:

1. Study the implementation to understand RTOS concepts
2. Follow academic integrity policies of their institution
3. Use as a learning tool rather than a direct solution
4. Understand the underlying principles before implementation

### Development Guidelines

1. Fork the repository and create feature branches
2. Maintain code quality and documentation standards
3. Test thoroughly on actual hardware
4. Submit pull requests with detailed descriptions

## Future Enhancements

### Short-term Improvements
- Watchdog timer implementation for system monitoring
- EEPROM data logging for historical analysis
- UART communication interface for external monitoring

### Medium-term Features
- PID controller for enhanced temperature control
- Predictive maintenance algorithms
- Energy optimization techniques

### Long-term Vision
- IoT integration with cloud connectivity
- Machine learning for adaptive control
- Distributed multi-oven control system

## Course Information

**Course**: CSE411s RTOS Course  
**Semester**: Summer 2025  
**University**: Faculty of Engineering, Ain Shams University  
**Supervision**: Dr. Omar Al-Kelany and Eng. Shahd Mohsen

## References

1. **FreeRTOS Documentation** - Real Time Engineers Ltd.
2. **TM4C123GH6PM Technical Documentation** - Texas Instruments
3. **Real-Time Systems and Programming Languages** - Burns & Wellings
4. **Embedded Systems Design** - Valvano

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- Course instructors and TAs at ASU Computer and Systems Engineering Department ([Shahd-AbouHashem](https://github.com/Shahd-AbouHashem))
- Texas Instruments for comprehensive microcontroller documentation
- FreeRTOS community for excellent RTOS implementation
- ARM for development tools and architecture documentation

---

**Last Updated**: August 2025  
**Maintained by**: 
[Spafic](https://github.com/Spafic), 
[Lucifer3224](https://github.com/Lucifer3224), 
[ShahdAshraff](https://github.com/ShahdAshraff)
