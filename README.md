# autonomous-fire-fighting-robot
ESP32-based autonomous fire-fighting robot prototype using flame detection, ESP-NOW communication, sensor integration and motor control.

## Working Principle

1. The remote fire detection unit monitors for fire using a flame sensor.
2. ESP32 #1 sends fire detection information and flame direction to the robot through ESP-NOW.
3. ESP32 #2 receives the wireless message.
4. The robot follows a predefined path using its IR sensor array.
5. When a fire alert is received, the robot stops.
6. The servo motor adjusts the water nozzle according to the received flame direction.
7. The water pump activates for a predefined duration of 5 seconds.
8. The pump switches off automatically after the 5-second interval.
9. The robot resumes line-following operation.

## System Architecture

### Node 1: Remote Fire Detection Unit
- ESP32 #1
- Flame Sensor 1
- ESP-NOW wireless communication

### Node 2: Mobile Fire-Fighting Robot
- ESP32 #2 (main controller)
- Onboard Flame Sensor 2
- 5-channel IR sensor for line following
- Motor driver and DC motors
- Water pump for fire suppression

### Communication
ESP-NOW enables wireless communication between the remote fire detection unit and the mobile robot without requiring a Wi-Fi router.

## Hardware Components

| Component | Quantity | Purpose |
|---|---:|---|
| ESP32 Development Board | 2 | Wireless communication and robot control |
| Flame Sensor | 2 | Remote and onboard fire detection |
| 5-Channel IR Sensor | 1 | Line-following navigation |
| L298N Motor Driver | 1 | DC motor control |
| DC Geared Motors | 2 | Robot movement |
| SG90 Servo Motor | 1 | Nozzle direction control |
| Relay Module | 1 | Water pump switching |
| Water Pump | 1 | Fire suppression |
| Buck Converter | 1 | Voltage regulation |
| Battery | 1 | Power supply |

## Software and Communication
- Arduino IDE
- Embedded C / Arduino framework
- ESP-NOW wireless communication

## Project Prototype

![Autonomous Fire Fighting Robot](images/WhatsApp%20Image%202026-11-07%20at%2012.30.19%20AM.jpeg)

*Physical prototype of the ESP32-based autonomous fire-fighting robot.*
## Circuit Diagram

![Circuit Diagram](images/circuit-diagram.png)

## Pin Connections

| ESP32 GPIO | Connected Component | Function |
|---|---|---|
| GPIO 18 | L298N IN1 | Motor A Direction 1 |
| GPIO 19 | L298N IN2 | Motor A Direction 2 |
| GPIO 21 | L298N IN3 | Motor B Direction 1 |
| GPIO 22 | L298N IN4 | Motor B Direction 2 |
| GPIO 23 | L298N ENA | Motor A Speed |
| GPIO 25 | L298N ENB | Motor B Speed |
| GPIO 26 | IRF540N Gate | Water Pump ON/OFF |
| GPIO 27 | SG90 Servo Signal | Nozzle Angle Control |
| GPIO 32 | Flame Sensor D-OUT | Digital Fire Detection |
| GPIO 33 | Flame Sensor A-OUT | Analog Flame Intensity |
| VIN (5V) | Buck Converter OUT+ | ESP32 Power Supply |
| 3V3 | Flame Sensor VCC | Sensor Power Supply |
| GND | Common Ground | Common Reference |
