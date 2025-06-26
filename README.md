# iot-car-arduino

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![GitHub Stars](https://img.shields.io/github/stars/Angelmmc/iot-car-arduino.svg)](https://github.com/Angelmmc/iot-car-arduino/stargazers)
[![GitHub Issues](https://img.shields.io/github/issues/Angelmmc/iot-car-arduino.svg)](https://github.com/Angelmmc/iot-car-arduino/issues)

This repository contains two Arduino programs for an IoT-based robotic car: 

-One version follows a predetermined routine and autonomously avoids obstacles using a HC-SR04 sensor placed on the front.

-The other version is based on the previous one but connects to the internet and is remotely controlled via API.

## ⚙️ Required Hardware

- ESP32 
- HC-SR04 ultrasonic sensor
- L298N motor driver
- 4 DC motors + wheels
- Chassis
- 2 Li-ion battery with holder (motors power supply)
- 9v battery (esp32 power supply)
- Color leds (optional)

## ⚙️ Getting started 

1. Open the `.ino` file in Arduino IDE, you can get it [here](https://www.arduino.cc/en/software/).
2. Select your board and port, to set up the ESP32 board for the first time follow this [tutorial](https://www.youtube.com/watch?v=wsKTmlipQOE).
3. Install required libraries (e.g., `HTTPClient`).
4. Upload to your ESP32.
5. Open Serial Monitor to verify sensor readings and WiFi status.

## 🖼️ Pirctures

<img src="https://github.com/Angelmmc/iot-car-arduino/blob/main/assets/car-front.jpg" width="300" />
<img src="https://github.com/Angelmmc/iot-car-arduino/blob/main/assets/car-rear.jpg" width="300" />
<img src="https://github.com/Angelmmc/iot-car-arduino/blob/main/assets/car-up.jpg" width="300" />
