#include "encoder.h"

// PINS
#define LEFT_ENC_A 34
#define LEFT_ENC_B 32
#define RIGHT_ENC_A 35
#define RIGHT_ENC_B 33

volatile long rawLeft = 0;
volatile long rawRight = 0;

// logical layer
static long lastLeft = 0;
static long lastRight = 0;

static long deltaLeft = 0;
static long deltaRight = 0;

static float leftSpeed = 0;
static float rightSpeed = 0;

static float leftDistance = 0;
static float rightDistance = 0;

static int leftDir = 0;
static int rightDir = 0;

static unsigned long lastUpdate = 0;

// ISR
void IRAM_ATTR leftISR() {
  if (digitalRead(LEFT_ENC_B)) rawLeft++;
  else rawLeft--;
}

void IRAM_ATTR rightISR() {
  if (digitalRead(RIGHT_ENC_B)) rawRight++;
  else rawRight--;
}

void encoder_init() {
  pinMode(LEFT_ENC_A, INPUT);
  pinMode(LEFT_ENC_B, INPUT);
  pinMode(RIGHT_ENC_A, INPUT);
  pinMode(RIGHT_ENC_B, INPUT);

  attachInterrupt(digitalPinToInterrupt(LEFT_ENC_A), leftISR, RISING);
  attachInterrupt(digitalPinToInterrupt(RIGHT_ENC_A), rightISR, RISING);

  lastUpdate = millis();
}

// MUST be called periodically
void encoder_update() {
  unsigned long now = millis();
  float dt = (now - lastUpdate) / 1000.0;
  if (dt <= 0) return;

  long currentLeft = rawLeft;
  long currentRight = rawRight;

  deltaLeft = currentLeft - lastLeft;
  deltaRight = currentRight - lastRight;

  leftSpeed = deltaLeft / dt;
  rightSpeed = deltaRight / dt;

  leftDir = (deltaLeft > 0) ? 1 : (deltaLeft < 0 ? -1 : 0);
  rightDir = (deltaRight > 0) ? 1 : (deltaRight < 0 ? -1 : 0);

  leftDistance += deltaLeft;
  rightDistance += deltaRight;

  lastLeft = currentLeft;
  lastRight = currentRight;
  lastUpdate = now;
}

// RESET LOGICAL STATE (called when motor stops)
void reset_logical_encoder() {
  lastLeft = rawLeft;
  lastRight = rawRight;

  deltaLeft = 0;
  deltaRight = 0;

  leftSpeed = 0;
  rightSpeed = 0;

  leftDistance = 0;
  rightDistance = 0;

  leftDir = 0;
  rightDir = 0;
}

// GETTERS
long get_raw_left() { return rawLeft; }
long get_raw_right() { return rawRight; }

long get_delta_left() { return deltaLeft; }
long get_delta_right() { return deltaRight; }

float get_left_speed() { return leftSpeed; }
float get_right_speed() { return rightSpeed; }

float get_left_distance() { return leftDistance; }
float get_right_distance() { return rightDistance; }

int get_left_direction() { return leftDir; }
int get_right_direction() { return rightDir; }
