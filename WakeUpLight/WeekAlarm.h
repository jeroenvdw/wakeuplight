/************************
 ** WeekAlarm.h
 ************************/

#ifndef WEEKALARM_H
#define WEEKALARM_H

#include "Arduino.h"

class WeekAlarm {
  public:
    WeekAlarm();
    void setHour(int day, int hour);
    void setMin(int day, int min);
    int getHour(int day);
    int getMin(int day);
    bool isActive(int day);
    void setActive(int day,bool active=true);
  private:
    int _hour[7];
    int _min[7];
    bool _active[7];
};

#endif
