/************************
 ** AlarmSettings.h
 ************************/

#ifndef ALARMSETTINGS_H
#define ALARMSETTINGS_H

#include "Arduino.h"
#include "Time.h"
#include "Alarm.h"
#include "WeekAlarm.h"
#include "ds3231.h"

class AlarmSettings {
  public:
    AlarmSettings();
    void setWakeUpDur(uint8_t);
    uint8_t getWakeUpDur();
    void setWakeUpLum(uint8_t);
    uint8_t getWakeUpLum();
    Time* getTime();
    void setTime(Time* time);
    Alarm* getDayAlarm();
    WeekAlarm* getWeekAlarm();
    uint8_t getAlarmConfigNumber();
    void setAlarmConfigNumber(uint8_t);
    char* getAlarmTitle();
    char* getAlarmTitle(uint8_t);
    uint8_t getAlarmHour();
    uint8_t getAlarmHour(uint8_t);
    void setAlarmHour(uint8_t);
    void setAlarmHour(uint8_t no,uint8_t hour);
    uint8_t getAlarmMin();
    uint8_t getAlarmMin(uint8_t no);
    void setAlarmMin(uint8_t);
    void setAlarmMin(uint8_t no,uint8_t hour);
    bool isAlarmActive();
    bool isAlarmActive(uint8_t);
    void setAlarmActive(bool);
    void setAlarmActive(uint8_t,bool);
    uint8_t getFirstActiveAlarmNo();
    uint8_t getSecondActiveAlarmNo();
    uint8_t getAlarmCount();
    uint8_t getSnoozeDur();
    void setSnoozeDur(uint8_t);
    uint8_t getBacklightDur();
    void setBacklightDur(uint8_t);
    void setSoundNo(uint8_t);
    uint8_t getSoundNo();
    void setAlarmVolume(uint8_t);
    uint8_t getAlarmVolume();
    void setSnoozeVolume(uint8_t);
    uint8_t getSnoozeVolume();
    void setMustPlayWhenSnoozing(bool);
    bool mustPlayWhenSnoozing();
    void setSnoozeAdvert(uint8_t);
    uint8_t getSnoozeAdvert();
    bool updateTimeFromRTC();
    void updateRTCFromTime();
    bool loadFromEEPROM();
    void saveToEEPROM();
  private:
    uint8_t _wakeUpDur;
    uint8_t _wakeUpLum;
    uint8_t _snoozeDur;
    uint8_t _backlightDur;
    Time* _time;
    Alarm* _dayAlarm;
    WeekAlarm* _weekAlarm;
    uint8_t _alarmCfgNo;     //0 for monday, 1 for tuesday, ... , 6 for sunday, 7 for tomorrow
    uint8_t _soundNo;        //0 for buzzer, 1 for first MP3 folder, 2 for second MP3 folder, ... up to 9
    uint8_t _alarmVolume;    //from 0 to 30
    uint8_t _snoozeVolume;   //from 0 to 30
    uint8_t _snoozeAdvert;
    bool _mustPlayWhenSnoozing;
};


#endif
