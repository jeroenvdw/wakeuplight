/************************
 ** Alarm.h
 ************************/

#ifndef ALARM_H
#define ALARM_H

#include "Arduino.h"

class Alarm {
  public:
    Alarm();
    void setHour(uint8_t hour);
    void setMin(uint8_t min);
    uint8_t getHour();
    uint8_t getMin();
    bool isActive();
    void setActive(bool active=true);
  private:
    uint8_t _hour;
    uint8_t _min;
    bool _active;
};

#endif
