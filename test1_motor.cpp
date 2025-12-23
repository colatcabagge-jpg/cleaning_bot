#include "motor.h"

// L298N pins
#define ENA 25
#define IN1 26
#define IN2 27

#define ENB 14
#define IN3 12
#define IN4 13

// PWM config
#define PWM_CH_LEFT   0
#define PWM_CH_RIGHT  1
#define PWM_FREQ      1000
#define PWM_RES       8   // 0–255

void motor_init() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  ledcSetup(PWM_CH_LEFT, PWM_FREQ, PWM_RES);
  ledcSetup(PWM_CH_RIGHT, PWM_FREQ, PWM_RES);

  ledcAttachPin(ENA, PWM_CH_LEFT);
  ledcAttachPin(ENB, PWM_CH_RIGHT);

  motor_stop();
}

void motor_forward(uint8_t speed) {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  ledcWrite(PWM_CH_LEFT, speed);
  ledcWrite(PWM_CH_RIGHT, speed);
}

void motor_backward(uint8_t speed) {
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
  ledcWrite(PWM_CH_LEFT, speed);
  ledcWrite(PWM_CH_RIGHT, speed);
}

void motor_left(uint8_t speed) {
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);   // left backward
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);   // right forward
  ledcWrite(PWM_CH_LEFT, speed);
  ledcWrite(PWM_CH_RIGHT, speed);
}

void motor_right(uint8_t speed) {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);   // left forward
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);   // right backward
  ledcWrite(PWM_CH_LEFT, speed);
  ledcWrite(PWM_CH_RIGHT, speed);
}

void motor_stop() {
  ledcWrite(PWM_CH_LEFT, 0);
  ledcWrite(PWM_CH_RIGHT, 0);
}
