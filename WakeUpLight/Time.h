/*************************
 ** Time.h
 ************************/  

#ifndef TIME_H
#define TIME_H

#include "Arduino.h"

class Time {
  public:
    Time(uint8_t day,uint8_t hour,uint8_t min,uint8_t sec);
    void setDay(uint8_t day);
    void setTime(uint8_t day,uint8_t hour,uint8_t min,uint8_t sec);
    void setTime(uint8_t hour,uint8_t min);
    uint8_t getDay();
    uint8_t getHour();
    uint8_t getMin();
    uint8_t getSec();
    int getMillis();
    void addMillis(int millis);
    char* getTimeString(bool showSeconds=false);
    char* getDayString();
    char* getDayString(uint8_t);
  private:
    uint8_t _day;
    uint8_t _hour;
    uint8_t _min;
    uint8_t _sec;
    int _millis;
    //static char* _days[];
};


#endif 
