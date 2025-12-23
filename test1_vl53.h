#ifndef VL53_MULTI_H
#define VL53_MULTI_H

#include <Arduino.h>

#define VL53_COUNT 4

void vl53_init();
void vl53_task();              // non-blocking task (call often)

bool vl53_is_connected(uint8_t id);
uint16_t vl53_get_distance(uint8_t id);
unsigned long vl53_last_seen(uint8_t id);
bool vl53_detected(uint8_t id);

#endif
