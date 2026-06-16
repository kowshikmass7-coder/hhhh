
# RTOS-Based Industrial Machine Health Monitoring System

## Overview

This project is an IoT-based Industrial Machine Health Monitoring System developed using ESP32 and FreeRTOS.

The system continuously monitors machine temperature and vibration levels, calculates machine health, predicts potential failures, identifies faults, and recommends maintenance schedules.

The project demonstrates the use of multitasking in FreeRTOS for real-time industrial monitoring applications.

---

## Features

- Real-time Temperature Monitoring
- Real-time Vibration Monitoring
- Machine Health Score Calculation
- Failure Risk Prediction
- Fault Detection
  - Overheating
  - Misalignment
  - Bearing Failure
- Maintenance Schedule Prediction
- Dual OLED Dashboard
- Buzzer Alert System
- FreeRTOS Multitasking

---

## Hardware Components

- ESP32 DevKit V4
- DHT22 Temperature Sensor
- Potentiometer (used as vibration simulator)
- 2 × SSD1306 OLED Displays
- Buzzer

---

## Software Used

- Arduino IDE
- ESP32 Board Package
- FreeRTOS
- Wokwi Simulator

---

## Libraries Required

- DHT Sensor Library
- Adafruit SSD1306
- Adafruit GFX Library
- Adafruit Unified Sensor

---

## System Architecture

Temperature Sensor (DHT22)
        |
        v
ESP32 + FreeRTOS Tasks
        |
        +--> Sensor Task
        +--> Prediction Task
        +--> OLED Display Task 1
        +--> OLED Display Task 2
        +--> Alarm Task
        |
        v
Health Score & Fault Prediction
        |
        v
OLED Displays + Buzzer Alerts

---

## Health Score Logic

### Temperature Impact

| Temperature | Penalty |
|------------|---------|
| >35°C | -20 |
| >45°C | -30 |

### Vibration Impact

| Vibration | Penalty |
|-----------|---------|
| >50% | -15 |
| >80% | -25 |

Health Score = 100 - Total Penalties

Failure Risk = 100 - Health Score

---

## Fault Detection

| Condition | Fault |
|------------|--------|
| Temperature >45°C | OVERHEAT |
| Vibration >80% | MISALIGNMENT |
| Temperature >45°C and Vibration >80% | BEARING FAILURE |
| Normal Values | NORMAL |

---

## Maintenance Prediction

| Health Score | Service Recommendation |
|-------------|------------------------|
| >80 | No Service Required |
| 61-80 | Service Within 30 Days |
| 41-60 | Service Within 15 Days |
| ≤40 | Immediate Maintenance |

---

## FreeRTOS Tasks

### Sensor Task
Reads temperature and vibration data.

### Prediction Task
Calculates machine health and predicts faults.

### Display Task 1
Displays temperature, vibration, and health score.

### Display Task 2
Displays failure risk and maintenance schedule.

### Alarm Task
Activates buzzer during critical conditions.

---

## Simulation

The project was designed and tested using Wokwi Simulator.

---

## Applications

- Predictive Maintenance
- Industrial Automation
- Factory Monitoring
- Smart Manufacturing
- Industry 4.0 Systems

---

## Future Improvements

- MQTT Cloud Integration
- Mobile App Monitoring
- Historical Data Logging
- AI-Based Failure Prediction
- Machine Learning Analytics

---

## Auth
