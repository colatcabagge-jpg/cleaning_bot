#ifndef ENCODER_H
#define ENCODER_H

#include <Arduino.h>

void encoder_init();
void encoder_update();   // call periodically

void reset_logical_encoder();  // resets distance/speed state

long get_raw_left();
long get_raw_right();

long get_delta_left();
long get_delta_right();

float get_left_speed();   // pulses/sec
float get_right_speed();

float get_left_distance();  // in pulses (convert later)
float get_right_distance();

int get_left_direction();   // +1 / -1 / 0
int get_right_direction();

#endif
