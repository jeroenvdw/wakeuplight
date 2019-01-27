/************************
 ** Alarm.cpp
 ************************/
 
#include "Alarm.h"

Alarm::Alarm() {
  _hour = 0;
  _min = 0;
  _active = false;
}
 
void Alarm::setHour(uint8_t hour) {
  _hour = hour;
}
 
uint8_t Alarm::getHour() {
 return _hour;
}

void Alarm::setMin(uint8_t min) {
  _min = min;
}

uint8_t Alarm::getMin() {
  return _min;
}

bool Alarm::isActive() {
  return _active;
}

void Alarm::setActive(bool active) {
  _active = active;
}




