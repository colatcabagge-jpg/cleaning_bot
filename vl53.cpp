#include "vl53.h"
#include <Wire.h>
#include <VL53L0X.h>

#define SDA_PIN 21
#define SCL_PIN 22

#define DETECT_MM 65
#define I2C_CLOCK 100000
#define RECONNECT_INTERVAL 1 // ms

// XSHUT pins and addresses
static uint8_t xshut[VL53_COUNT] = {4, 16, 17, 5};
static uint8_t addr [VL53_COUNT] = {0x30, 0x31, 0x32, 0x33};

static VL53L0X sensor[VL53_COUNT];

static bool connected[VL53_COUNT];
static uint16_t lastDist[VL53_COUNT];
static unsigned long lastSeen[VL53_COUNT];
static unsigned long lastReconnectTry[VL53_COUNT];

static void hard_disable_all() {
  for (int i = 0; i < VL53_COUNT; i++) {
    pinMode(xshut[i], OUTPUT);
    digitalWrite(xshut[i], LOW);
    connected[i] = false;
  }
}

static bool connect_one(uint8_t i) {
  digitalWrite(xshut[i], HIGH);
  delay(80);

  sensor[i].setTimeout(200);

  if (!sensor[i].init()) {
    digitalWrite(xshut[i], LOW);
    return false;
  }

  sensor[i].setAddress(addr[i]);
  sensor[i].setMeasurementTimingBudget(200000);
  sensor[i].startContinuous(100);

  connected[i] = true;
  lastSeen[i] = millis();
  return true;
}

void vl53_init() {
  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(I2C_CLOCK);

  hard_disable_all();
  delay(200);

  for (int i = 0; i < VL53_COUNT; i++) {
    lastDist[i] = 0;
    lastReconnectTry[i] = 0;
  }

  // sequential bring-up
  for (int i = 0; i < VL53_COUNT; i++) {
    if (connect_one(i)) {
      Serial.print("✓ VL53 ");
      Serial.print(i);
      Serial.println(" connected");
    } else {
      Serial.print("✗ VL53 ");
      Serial.print(i);
      Serial.println(" failed");
    }
  }

  Serial.println("VL53 system ready (robust mode)");
}

void vl53_task() {
  unsigned long now = millis();

  for (int i = 0; i < VL53_COUNT; i++) {

    // ── If connected → try reading
    if (connected[i]) {
      uint16_t d = sensor[i].readRangeContinuousMillimeters();

      if (sensor[i].timeoutOccurred() || d >= 8190) {
        connected[i] = false;
        digitalWrite(xshut[i], LOW);
        lastReconnectTry[i] = now;
        continue;
      }

      lastDist[i] = d;
      lastSeen[i] = now;
    }

    // ── If disconnected → retry occasionally
    else {
      if (now - lastReconnectTry[i] >= RECONNECT_INTERVAL) {
        lastReconnectTry[i] = now;
        connect_one(i);
      }
    }
  }
}

bool vl53_is_connected(uint8_t id) {
  if (id >= VL53_COUNT) return false;
  return connected[id];
}

uint16_t vl53_get_distance(uint8_t id) {
  if (id >= VL53_COUNT) return 0;
  return lastDist[id];
}

unsigned long vl53_last_seen(uint8_t id) {
  if (id >= VL53_COUNT) return 0;
  return lastSeen[id];
}

bool vl53_detected(uint8_t id) {
  if (id >= VL53_COUNT) return false;
  return connected[id] && lastDist[id] > 0 && lastDist[id] <= DETECT_MM;
}
