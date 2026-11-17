# Smart Home Automation System

An advanced Arduino-based smart home security and environmental control system featuring secure keypad authentication, automated climate control, motion detection, and intrusion alarms.

---

## Features & Specifications

* **Secure Keypad Authentication:** 4-digit PIN entry system via a matrix keypad with LCD visual feedback (`*` masking) and servo-driven door locking mechanism.
* **Environmental Monitoring:** Real-time temperature and humidity tracking using a **DHT11** sensor.
* **Smart Climate Control:** Automatic fan activation governed by temperature thresholds dynamically adjustable via an analog potentiometer.
* **Intrusion & Security Alarm:** 
  * **Reed Switch (Magnetic Contact):** Detects unauthorized window/door openings, triggering an immediate buzzer alarm and system lock.
  * **PIR Motion Sensor:** Triggers outdoor security lighting upon motion detection.
* **Adaptive Lighting:** LDR (Light Dependent Resistor) sensor-based automated indoor lighting with manual override button control.

---

## Hardware Pinout Configuration

| Component | Pin / Port | Description |
| :--- | :--- | :--- |
| **LCD Display (I2C)** | SDA / SCL | 16x2 I2C Character Display (Address `0x27`) |
| **Matrix Keypad** | Rows: `0, 13, 2, 3`<br>Cols: `4, 5, 6` | 4x3 User Input Keypad |
| **Servo Motor** | Pin `9` | Door Lock Actuator Mechanism |
| **DHT11 Sensor** | Pin `A1` | Temperature and Humidity Sensor |
| **Potentiometer** | Pin `A3` | Temperature Threshold Adjuster |
| **PIR Motion Sensor** | Pin `8` | Motion Tracking / Security |
| **Reed Switch** | Pin `7` | Door/Window State Detector |
| **Buzzer** | Pin `11` | Security Alarm Sounder |
| **Indoor LED** | Pin `12` | Room Lighting Control |
| **Outdoor LED** | Pin `A2` | Motion-Triggered Security Light |
| **Cooling Fan** | Pin `10` | Temperature Regulation Fan |
| **Override Button** | Pin `1` | Manual Lighting Control Button |
| **LDR Sensor** | Pin `A0` | Ambient Light Level Detection |

---

## System Operation & States

1. **Lock State (`mode = 0`)**: The system boots up prompting for the PIN (`Enter the pass: `). Entering the correct password (`1234`) unlocks the door servo for 5 seconds and transitions the system to operational mode. Entering a wrong PIN triggers a warning prompt.
2. **Operational / Home Mode (`mode = 1`)**: 
   * Continuously monitors temperature and humidity on the LCD.
   * Allows adjusting the fan trigger threshold dynamically using the potentiometer.
   * Monitors security parameters (PIR motion and Reed switch state).
3. **Alarm State (`mode = -1`)**: Triggered if a security breach (e.g., opened Reed switch) occurs while armed, activating the buzzer and locking down the system interface until reset via correct credentials.

---

## Code Structure & Dependencies

Ensure you have the following Arduino libraries installed before compiling and uploading the sketch:
* `Keypad` (by Mark Stanley, Alexander Brevig)
* `Servo` (Standard Arduino Servo Library)
* `DHT` (Adafruit Unified Sensor & DHT Sensor Library)
* `LCD_I2C` (Liquid Crystal I2C Library)

---

## Getting Started

1. Clone or download this repository.
2. Open the `.ino` file in the **Arduino IDE**.
3. Install the required dependencies through the Library Manager.
4. Select your target microcontroller board and correct COM port.
5. Compile and upload the sketch to your hardware.