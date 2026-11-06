// ============================================================
// Extinguisher ESP32 — Line following fire extinguisher robot
// Receives flame direction from Scout via ESP-NOW
// Stops, aims servo, pumps water, resumes line following
//
// L298N Motor Driver — direct F-F:
// GPIO21 -> ENA bottom pin
// GPIO22 -> IN1
// GPIO23 -> IN2
// GPIO19 -> ENB bottom pin
// GPIO18 -> IN3
// GPIO5  -> IN4
// GND -> L298N GND
//
// IR Sensor — direct F-F:
// GPIO26 -> S1 (left)
// GPIO33 -> S3 (centre)
// GPIO14 -> S5 (right)
// 3V3 -> VCC
// GND -> GND
//
// Servo (SG90):
// GPIO13 -> Signal (orange/yellow wire)
// 5V -> VCC (red wire) — from buck converter
// GND -> GND (brown wire)
//
// Water Pump (via MOSFET/relay):
// GPIO12 -> MOSFET gate / relay signal
// Pump powered from buck converter 5V
//
// Buck converter:
// IN+ -> Battery+
// IN- -> Battery-
// OUT+ -> Servo VCC + Pump VCC
// OUT- -> GND (common)
// ============================================================

#include <esp_now.h>
#include <WiFi.h>
#include <ESP32Servo.h>

// Motor pins
#define ENA 21
#define IN1 22
#define IN2 23
#define ENB 19
#define IN3 18
#define IN4 5

// IR sensor pins
#define S_LEFT 26
#define S_CENTRE 33
#define S_RIGHT 14

// Servo and pump pins
#define SERVO_PIN 13
#define PUMP_PIN 12

// Servo positions
#define SERVO_LEFT 45
#define SERVO_CENTRE 90
#define SERVO_RIGHT 135

// Pump run time
#define PUMP_DURATION 5000 // ms

// PWM
#define PWM_FREQ 1000
#define PWM_RES 8

// Speed
#define SPEED_NORMAL 180
#define SPEED_TURN 110
#define SPEED_FAST 220

// Sensor logic
#define SENSOR_ON_LINE LOW

// Debug
#define DEBUG 1

// State
volatile bool flameDetected = false;
volatile uint8_t flameDirection = 0; // 1=left 2=centre 3=right
bool extinguishing = false;

Servo nozzleServo;

// Message structure (must match Scout)
typedef struct {
  uint8_t flame_detected;
  uint8_t direction;
} FlameMessage;

// ============================================================
// Motor helpers
// ============================================================
void motorA_forward(int speed) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  ledcWrite(ENA, speed);
}

void motorA_brake() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, HIGH);
  ledcWrite(ENA, 255);
}

void motorA_stop() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  ledcWrite(ENA, 0);
}

void motorB_forward(int speed) {
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  ledcWrite(ENB, speed);
}

void motorB_brake() {
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, HIGH);
  ledcWrite(ENB, 255);
}

void motorB_stop() {
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  ledcWrite(ENB, 0);
}

void goForward(int speed) {
  motorA_forward(speed);
  motorB_forward(speed);
}

void turnLeft(int outerSpeed, int innerSpeed) {
  motorA_forward(innerSpeed);
  motorB_forward(outerSpeed);
}

void turnRight(int outerSpeed, int innerSpeed) {
  motorA_forward(outerSpeed);
  motorB_forward(innerSpeed);
}

void sharpLeft() {
  motorA_brake();
  motorB_forward(SPEED_FAST);
}

void sharpRight() {
  motorA_forward(SPEED_FAST);
  motorB_brake();
}

void stopAll() {
  motorA_stop();
  motorB_stop();
}

// ============================================================
// Sensor reading
// ============================================================
void readSensors(bool &L, bool &C, bool &R) {
  L = (digitalRead(S_LEFT) == SENSOR_ON_LINE);
  C = (digitalRead(S_CENTRE) == SENSOR_ON_LINE);
  R = (digitalRead(S_RIGHT) == SENSOR_ON_LINE);
}

// ============================================================
// Extinguish routine
// ============================================================
void extinguish(uint8_t direction) {
  Serial.printf("Extinguishing! Direction: %s\n",
                direction == 1 ? "LEFT" :
                direction == 2 ? "CENTRE" : "RIGHT");

  // Stop robot
  stopAll();
  delay(300);

  // Aim servo
  if (direction == 1) nozzleServo.write(SERVO_LEFT);
  else if (direction == 2) nozzleServo.write(SERVO_CENTRE);
  else nozzleServo.write(SERVO_RIGHT);
  delay(500); // wait for servo to reach position

  // Run pump
  Serial.println("Pump ON");
  digitalWrite(PUMP_PIN, HIGH);
  delay(PUMP_DURATION);
  digitalWrite(PUMP_PIN, LOW);
  Serial.println("Pump OFF");

  // Return servo to centre
  nozzleServo.write(SERVO_CENTRE);
  delay(300);

  // Reset flag
  flameDetected = false;
  flameDirection = 0;
  extinguishing = false;

  Serial.println("Resuming line following...");
  delay(500);
}

// ============================================================
// ESP-NOW receive callback
// ============================================================
void onReceive(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
  FlameMessage *msg = (FlameMessage *)data;
  if (msg->flame_detected == 1) {
    flameDetected = true;
    flameDirection = msg->direction;
    Serial.printf("ESP-NOW: Flame received! Direction: %d\n", msg->direction);
  } else {
    flameDetected = false;
    flameDirection = 0;
    Serial.println("ESP-NOW: All clear received");
  }
}

// ============================================================
// Setup
// ============================================================
void setup() {
  Serial.begin(115200);

  // Motor pins
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // PWM
  ledcAttach(ENA, PWM_FREQ, PWM_RES);
  ledcAttach(ENB, PWM_FREQ, PWM_RES);

  // Sensor pins
  pinMode(S_LEFT, INPUT_PULLUP);
  pinMode(S_CENTRE, INPUT_PULLUP);
  pinMode(S_RIGHT, INPUT_PULLUP);

  // Pump pin
  pinMode(PUMP_PIN, OUTPUT);
  digitalWrite(PUMP_PIN, LOW);

  // Servo
  nozzleServo.attach(SERVO_PIN);
  nozzleServo.write(SERVO_CENTRE);

  stopAll();

  // ESP-NOW init
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return;
  }
  esp_now_register_recv_cb(onReceive);

  Serial.println("Extinguisher ESP32 ready.");
  Serial.print("My MAC: ");
  Serial.println(WiFi.macAddress());

  delay(2000);
}

// ============================================================
// Main loop
// ============================================================
void loop() {

  // Check if flame detected — interrupt line following
  if (flameDetected && !extinguishing) {
    extinguishing = true;
    extinguish(flameDirection);
    return;
  }

  // Normal line following
  bool L, C, R;
  readSensors(L, C, R);

  if (DEBUG) {
    Serial.printf("L:%d C:%d R:%d\n", L, C, R);
  }

  if (!L && !C && !R) { stopAll(); return; }
  if (L && C && R) { goForward(SPEED_NORMAL); return; }
  if (!L && C && !R) { goForward(SPEED_NORMAL); return; }
  if (L && C && !R) { turnLeft(SPEED_NORMAL, SPEED_TURN); return; }
  if (L && !C && !R) { sharpLeft(); return; }
  if (!L && C && R) { turnRight(SPEED_NORMAL, SPEED_TURN); return; }
  if (!L && !C && R) { sharpRight(); return; }
  if (L && !C && R) { goForward(SPEED_NORMAL); return; }

  goForward(SPEED_NORMAL);
}
