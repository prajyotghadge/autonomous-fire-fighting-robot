# autonomous-fire-fighting-robot
ESP32-based autonomous fire-fighting robot prototype using flame detection, ESP-NOW communication, sensor integration and motor control.

## Working Principle

1. The remote fire detection unit continuously monitors its surroundings using Flame Sensor 1.
2. When fire is detected, ESP32 #1 sends a wireless alert to the robot using ESP-NOW.
3. ESP32 #2 receives the alert and activates the robot.
4. The robot follows a predefined path using its 5-channel IR sensor.
5. When the onboard Flame Sensor 2 detects fire, the robot stops.
6. The water pump activates to suppress the fire.
7. The pump continues running while the onboard flame sensor detects fire.
8. When the onboard flame sensor no longer detects fire, the pump switches off.

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
