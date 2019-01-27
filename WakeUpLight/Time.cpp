/*************************
 ** Time.cpp
 ************************/  
 
#include "Time.h"
#include <stdio.h>
#include "Literals.h"
using namespace AlarmLiterals;

char* days[] = {MONDAY,TUESDAY,WEDNESDAY,THURSDAY,FRIDAY,SATURDAY,SUNDAY};
 
Time::Time(uint8_t day,uint8_t hour,uint8_t min,uint8_t sec) {
  _day = day;
  _hour = hour;
  _min = min;
  _sec = sec;
  _millis = 0;
}

void Time::setTime(uint8_t hour,uint8_t min) {
  _hour = hour;
  _min = min;
  _sec = 0;
  _millis = 0;
}
 
void Time::setTime(uint8_t day,uint8_t hour,uint8_t min,uint8_t sec) {
  _day = day;
  _hour = hour;
  _min = min;
  _sec = sec;
  _millis = 0;
}

void Time::setDay(uint8_t day) {
  _day = day;
}
 
uint8_t Time::getDay() {
  return _day;
}
 
uint8_t Time::getHour() {
  return _hour;
}

uint8_t Time::getMin() {
  return _min;
}
  
uint8_t Time::getSec() {
  return _sec;
}

int Time::getMillis() {
  return _millis;
}

void Time::addMillis(int millis) {
  _millis += millis;
  
  _sec += _millis / 1000;
  _millis = _millis % 1000;
  
  _min += _sec / 60;
  _sec = _sec % 60;
  
  _hour += _min / 60;
  _min = _min % 60;
  
  _day += _hour / 24;
  _hour = _hour % 24;
  
  _day = _day % 7;
}

char* Time::getTimeString(bool showSeconds) {
  if(showSeconds) {
    char* t = "        ";
    //char t[] = "        ";
    sprintf(t,"%02d:%02d:%02d",_hour,_min,_sec);
    return t;
  }
  else {
    char* t = "     ";
    //char t[] = "     ";
    sprintf(t,"%02d:%02d",_hour,_min);
    return t;
  }
}

char* Time::getDayString(uint8_t day) {
  return days[day];
}

char* Time::getDayString() {
  //return "geen dag";
  return days[_day];
}

  
