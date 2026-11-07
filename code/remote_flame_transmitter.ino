/*
  Remote Flame Sensor - ESP32 ESP-NOW Transmitter
  Prototype: single flame sensor, fixed direction = CENTRE (2)

  Wiring:
    Flame sensor D0 -> GPIO 27
    Flame sensor VCC -> 3V3 (check your module's rated voltage)
    Flame sensor GND -> ESP32 GND

  IMPORTANT:
    This sketch sends the same two-byte message structure expected by
    the robot controller:
      flame_detected: 0 or 1
      direction:      1=left, 2=centre, 3=right

    Many flame sensor modules have an active-LOW digital output.
    If your module behaves oppositely, change FLAME_ACTIVE_STATE to HIGH.
*/

#include <esp_now.h>
#include <WiFi.h>

#define FLAME_SENSOR_PIN 27
#define FLAME_ACTIVE_STATE LOW
#define SEND_INTERVAL_MS 200

typedef struct {
  uint8_t flame_detected;
  uint8_t direction;
} FlameMessage;

FlameMessage message;
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

unsigned long lastSendTime = 0;

void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("ESP-NOW send status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Failed");
}

void setup() {
  Serial.begin(115200);
  pinMode(FLAME_SENSOR_PIN, INPUT);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  Serial.print("Remote ESP32 MAC Address: ");
  Serial.println(WiFi.macAddress());

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW initialization failed");
    return;
  }

  esp_now_register_send_cb(onDataSent);

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add broadcast peer");
    return;
  }

  Serial.println("Flame sensor transmitter ready");
}

void loop() {
  int sensorValue = digitalRead(FLAME_SENSOR_PIN);

  message.flame_detected =
      (sensorValue == FLAME_ACTIVE_STATE) ? 1 : 0;

  // A single sensor cannot determine direction.
  // Centre (2) is a placeholder for this prototype.
  message.direction = 2;

  unsigned long now = millis();

  if (now - lastSendTime >= SEND_INTERVAL_MS) {
    lastSendTime = now;

    esp_err_t result = esp_now_send(
      broadcastAddress,
      (uint8_t *)&message,
      sizeof(message)
    );

    if (result == ESP_OK) {
      Serial.print("Flame detected: ");
      Serial.println(message.flame_detected ? "YES" : "NO");
    } else {
      Serial.println("Error sending ESP-NOW message");
    }
  }

  delay(20);
}
