/************************
 ** WeekAlarm.cpp
 ************************/

#include "WeekAlarm.h"

WeekAlarm::WeekAlarm() {
  for(int i=0;i<7;i++) {
    _hour[i] = 0;
    _min[i] = 0;
    _active[i] = false;
  }
}
 
void WeekAlarm::setHour(int day, int hour) {
  if(day >= 0 && day <= 6) {
    _hour[day] = hour;
  }
}
 
int WeekAlarm::getHour(int day) {
  if(day >= 0 && day <= 6) {
    return _hour[day];
  }
  else {
    return 0;
  }
}

void WeekAlarm::setMin(int day,int min) {
  if(day >= 0 && day <= 6) {
    _min[day] = min;
  }
}

int WeekAlarm::getMin(int day) {
  if(day >= 0 && day <= 6) {
    return _min[day];
  }
  else {
    return 0;
  }
}

bool WeekAlarm::isActive(int day) {
  if(day >= 0 && day <= 6) {
    return _active[day];
  }
  else {
    return false;
  }
}

void WeekAlarm::setActive(int day,bool active) {
  if(day >= 0 && day <= 6) {
    _active[day] = active;
  }
}




