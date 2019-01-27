/*************************
 ** AlarmSettings.cpp
 ************************/

#include "AlarmSettings.h"
#include <EEPROM.h>
#include "Literals.h"
using namespace AlarmLiterals;

AlarmSettings::AlarmSettings() {
  _wakeUpDur = 30;
  _wakeUpLum = 255;
  _snoozeDur = 5;
  _backlightDur = 5;
  _time = new Time(0,0,0,00);
  _dayAlarm = new Alarm();
  _weekAlarm = new WeekAlarm();
  _alarmCfgNo = 7;
  _soundNo = 0;
  _alarmVolume = 20;
  _snoozeVolume = 0;
  _mustPlayWhenSnoozing = false;
}

void AlarmSettings::setWakeUpDur(uint8_t wakeUpDur) {
  _wakeUpDur = wakeUpDur;
}

uint8_t AlarmSettings::getWakeUpDur() {
  return _wakeUpDur;
}

Time* AlarmSettings::getTime() {
  return _time;
}

void AlarmSettings::setWakeUpLum(uint8_t wakeUpLum) {
  _wakeUpLum = wakeUpLum;
}

uint8_t AlarmSettings::getWakeUpLum() {
  return _wakeUpLum;
}

Alarm* AlarmSettings::getDayAlarm() {
  return _dayAlarm;
}

WeekAlarm* AlarmSettings::getWeekAlarm() {
  return _weekAlarm;
}

uint8_t AlarmSettings::getAlarmConfigNumber() {
  return _alarmCfgNo;
}

void AlarmSettings::setAlarmConfigNumber(uint8_t no) {
  _alarmCfgNo = no;
}

char* AlarmSettings::getAlarmTitle() {
  return getAlarmTitle(_alarmCfgNo);
}

char* AlarmSettings::getAlarmTitle(uint8_t no) {
  if(no == 7) {
    return TOMORROW;
  }
  else {
    return _time->getDayString(no); //_days[no];
  }
}

uint8_t AlarmSettings::getAlarmHour() {
  return getAlarmHour(_alarmCfgNo);
}

uint8_t AlarmSettings::getAlarmHour(uint8_t no) {
  if(no == 7) {
    return _dayAlarm->getHour();
  }
  else {
    return _weekAlarm->getHour(no);
  }
}

void AlarmSettings::setAlarmHour(uint8_t hour) {
  setAlarmHour(_alarmCfgNo,hour);
}

void AlarmSettings::setAlarmHour(uint8_t no, uint8_t hour) {
  if(no == 7) {
    _dayAlarm->setHour(hour);
  }
  else {
    _weekAlarm->setHour(no,hour);
  }
}

uint8_t AlarmSettings::getAlarmMin() {
  return getAlarmMin(_alarmCfgNo);
}

uint8_t AlarmSettings::getAlarmMin(uint8_t no) {
  if(no == 7) {
    return _dayAlarm->getMin();
  }
  else {
    return _weekAlarm->getMin(no);
  }
}

void AlarmSettings::setAlarmMin(uint8_t min) {
  setAlarmMin(_alarmCfgNo,min);
}

void AlarmSettings::setAlarmMin(uint8_t no,uint8_t min) {
  if(no == 7) {
    _dayAlarm->setMin(min);
  }
  else {
    _weekAlarm->setMin(no,min);
  }
}

bool AlarmSettings::isAlarmActive() {
  return isAlarmActive(_alarmCfgNo);
}

bool AlarmSettings::isAlarmActive(uint8_t no) {
  if(no == 7) {
    return _dayAlarm->isActive();
  }
  else {
    return _weekAlarm->isActive(no);
  }
}

void AlarmSettings::setAlarmActive(bool active) {
  setAlarmActive(_alarmCfgNo,active);
}

void AlarmSettings::setAlarmActive(uint8_t no,bool active) {
  if(no == 7) {
    _dayAlarm->setActive(active);
  }
  else {
    _weekAlarm->setActive(no,active);
  }
}

uint8_t AlarmSettings::getAlarmCount() {
  return 8;
}

uint8_t AlarmSettings::getFirstActiveAlarmNo() {
  uint8_t d = _time->getDay();
  int tm = _time->getHour() * 60 + _time->getMin();
  int alarms[8];
  
  //save for every alarm the time in minutes (if not active, simply save -1)
  for(int i=0;i<7;i++) {
    if(_weekAlarm->isActive(i)) {
      alarms[i] = _weekAlarm->getHour(i) * 60 + _weekAlarm->getMin(i);
    }
    else {
      alarms[i] = -1;
    }
  }
  if(_dayAlarm->isActive()) {
    alarms[7] = _dayAlarm->getHour() * 60 + _dayAlarm->getMin();
  }
  else {
    alarms[7] = -1;
  }
  
  //calculate for every alarm the amount of minutes left to go off
  for(int i=0;i<7;i++) {
    if(alarms[i] >= 0) {
      if(i == d) {
        if( (_weekAlarm->getHour(d) >  _time->getHour() ) ||
            (_weekAlarm->getHour(d) == _time->getHour() && _weekAlarm->getMin(d)  >= _time->getMin()) ) {
          //alarm will go off today
          alarms[i] = alarms[i]-tm;
        }
        else {
          //alarm will go off next week
          //alarms[i] = alarms[i]-tm + 10080; // (60*24*7);
          alarms[i] = -1; //do not take this alarm into account today
        }
      }
      else if(i < d) {
        //alarm will go off next week; we have to count 7-d+i days extra (= days left this week (7-d) + days of next week 
        alarms[i] = alarms[i]-tm + (7-d+i) * 1440; // (60*24)
      }
      else { // (i > d)
        //alarm will go off this week; we have to count i-d days extra (=days left to this day)
        alarms[i] = alarms[i]-tm + (i-d) * 1440; // (60*24)
      }
    }
  }
  if(alarms[7] >= 0) {
    if( ( _dayAlarm->getHour() >  _time->getHour() ) || 
        ( _dayAlarm->getHour() == _time->getHour() && _dayAlarm->getMin()  >= _time->getMin() ) ) {
      //dayalarm will be active today
      alarms[7] = alarms[7]-tm;
    }
    else {
      //dayalarm will go off tomorrow
      alarms[7] = alarms[7]-tm + 1440;
    }
  }
  
  //find the lowest value in alarms[] (but not -1)
  uint8_t no = 8;
  for(int i=0;i<8;i++) {
    if(alarms[i] >= 0 && (no == 8 || alarms[i]<alarms[no])) {
      no = i;
    }
  }
  
  return no;
}

uint8_t AlarmSettings::getSecondActiveAlarmNo() {
  uint8_t no = getFirstActiveAlarmNo();
  if(no == 8) {
    //seek no more
    return no;
  }
  
  uint8_t d = _time->getDay();
  int tm = _time->getHour() * 60 + _time->getMin();
  int alarms[8];
  
  //save for every alarm the time in minutes (if not active, simply save -1)
  for(int i=0;i<7;i++) {
    if(_weekAlarm->isActive(i)) {
      alarms[i] = _weekAlarm->getHour(i) * 60 + _weekAlarm->getMin(i);
    }
    else {
      alarms[i] = -1;
    }
  }
  if(_dayAlarm->isActive()) {
    alarms[7] = _dayAlarm->getHour() * 60 + _dayAlarm->getMin();
  }
  else {
    alarms[7] = -1;
  }
  
  //we're not interested in the first alarm to go off
  alarms[no] = -1;
  
  //calculate for every alarm the amount of minutes left to go off
  for(int i=0;i<7;i++) {
    if(alarms[i] >= 0) {
      if(i == d) {
        if( (_weekAlarm->getHour(d) >  _time->getHour() ) ||
            (_weekAlarm->getHour(d) == _time->getHour() && _weekAlarm->getMin(d)  >= _time->getMin()) ) {
          //alarm will go off today
          alarms[i] = alarms[i]-tm;
        }
        else {
          //alarm will go off next week
          alarms[i] = alarms[i]-tm + 10080; // (60*24*7);
        }
      }
      else if(i < d) {
        //alarm will go off next week; we have to count 7-d+i days extra (= days left this week (7-d) + days of next week 
        alarms[i] = alarms[i]-tm + (7-d+i) * 1440; // (60*24)
      }
      else { // (i > d)
        //alarm will go off this week; we have to count i-d days extra (=days left to this day)
        alarms[i] = alarms[i]-tm + (i-d) * 1440; // (60*24)
      }
    }
  }
  if(alarms[7] >= 0) {
    if( ( _dayAlarm->getHour() >  _time->getHour() ) || 
        ( _dayAlarm->getHour() == _time->getHour() && _dayAlarm->getMin()  >= _time->getMin() ) ) {
      //dayalarm will be active today
      alarms[7] = alarms[7]-tm;
    }
    else {
      //dayalarm will go off tomorrow
      alarms[7] = alarms[7]-tm + 1440;
    }
  }
  
  //find the lowest value in alarms[] (but not -1)
  no = 8;
  for(int i=0;i<8;i++) {
    if(alarms[i] >= 0 && (no == 8 || alarms[i]<alarms[no])) {
      no = i;
    }
  }
  
  return no;
}

uint8_t AlarmSettings::getSnoozeDur() {
  return _snoozeDur;
}

void AlarmSettings::setSnoozeDur(uint8_t dur) {
  _snoozeDur = dur;
}

uint8_t AlarmSettings::getBacklightDur() {
  return _backlightDur;
}

void AlarmSettings::setBacklightDur(uint8_t dur) {
  _backlightDur = dur;
}

uint8_t AlarmSettings::getSoundNo() {
  return _soundNo;
}

void AlarmSettings::setSoundNo(uint8_t n) {
  _soundNo = n;
}

uint8_t AlarmSettings::getAlarmVolume() {
  return _alarmVolume;
}

void AlarmSettings::setAlarmVolume(uint8_t vol) {
  _alarmVolume = vol;
}

uint8_t AlarmSettings::getSnoozeVolume() {
  return _snoozeVolume;
}

void AlarmSettings::setSnoozeVolume(uint8_t vol) {
  _snoozeVolume = vol;
}

bool AlarmSettings::mustPlayWhenSnoozing() {
  return _mustPlayWhenSnoozing;
}

void AlarmSettings::setMustPlayWhenSnoozing(bool mustPlay) {
  _mustPlayWhenSnoozing = mustPlay;
}

void AlarmSettings::setSnoozeAdvert(uint8_t n) {
  _snoozeAdvert = n;
}

uint8_t AlarmSettings::getSnoozeAdvert() {
  return _snoozeAdvert;
}

bool AlarmSettings::updateTimeFromRTC() {
  //safe to update?
  int a = getFirstActiveAlarmNo();
  int am = getAlarmHour(a) * 60 + getAlarmMin(a);
  struct ts t;
  DS3231_get(&t);
  int tm = t.hour * 60 + t.min;
  //safe to update?
  if(( a == 7 || a == _time->getDay()) && (abs(am - tm) <= getWakeUpDur())) {
    //not safe, we might miss an alarm!
    return false;
  } else {
    //safe! update the alarm!
    _time->setTime(t.wday,t.hour,t.min,t.sec);
    return true;
  }
}

void AlarmSettings::updateRTCFromTime() {
  struct ts t;
  t.sec = _time->getSec();
  t.min = _time->getMin();
  t.hour = _time->getHour();
  t.wday = _time->getDay();
  t.mday = 1;
  t.mon = 1;
  t.year = 1;
  DS3231_set(t);
}

const char SETTINGS_VERSION[5] = "2.02";

struct eeprom_settings {
  char version[5];
  uint8_t wakeUpDur;
  uint8_t wakeUpLum;
  uint8_t snoozeDur;
  uint8_t backlightDur;
  uint8_t dayAlarmHhour;
  uint8_t dayAlarmMin;
  bool dayAlarmActive;
  int weekAlarmHour[7];
  int weekAlarmMin[7];
  bool weekAlarmActive[7];
  uint8_t soundNo;        //0 for buzzer, 1 for first MP3 folder, 2 for second MP3 folder, ... up to 9
  uint8_t alarmVolume;
  uint8_t snoozeVolume;
  bool mustPlayWhenSnoozing;
  uint8_t snoozeAdvert;
};

bool AlarmSettings::loadFromEEPROM() {
  struct eeprom_settings s;
  EEPROM.get(0,s);
  if(strcmp(s.version,SETTINGS_VERSION) == 0) {
    //version match!
    _wakeUpDur = s.wakeUpDur;
    _wakeUpLum = s.wakeUpLum;
    _snoozeDur = s.snoozeDur;
    _backlightDur = s.backlightDur;
    _dayAlarm->setHour(s.dayAlarmHhour);
    _dayAlarm->setMin(s.dayAlarmMin);
    _dayAlarm->setActive(s.dayAlarmActive);
    for(int i=0;i<7;i++) {
      _weekAlarm->setHour(i, s.weekAlarmHour[i]);
      _weekAlarm->setMin(i, s.weekAlarmMin[i]);
      _weekAlarm->setActive(i, s.weekAlarmActive[i]);
    }
    _soundNo = s.soundNo;
    _alarmVolume = s.alarmVolume;
    _snoozeVolume = s.snoozeVolume;
    _mustPlayWhenSnoozing = s.mustPlayWhenSnoozing;
    _snoozeAdvert = s.snoozeAdvert;
    return true;
  } else {
    return false;
  }
}

void AlarmSettings::saveToEEPROM() {
  struct eeprom_settings s;
  strcpy(s.version,SETTINGS_VERSION);
  s.wakeUpDur = _wakeUpDur;
  s.wakeUpLum = _wakeUpLum;
  s.snoozeDur = _snoozeDur;
  s.backlightDur = _backlightDur;
  s.dayAlarmHhour = _dayAlarm->getHour();
  s.dayAlarmMin = _dayAlarm->getMin();
  s.dayAlarmActive = _dayAlarm->isActive();
  for(int i=0;i<7;i++) {
    s.weekAlarmHour[i] = _weekAlarm->getHour(i);
    s.weekAlarmMin[i] = _weekAlarm->getMin(i);
    s.weekAlarmActive[i] = _weekAlarm->isActive(i);
  }
  s.soundNo = _soundNo;
  s.alarmVolume =_alarmVolume;
  s.snoozeVolume = _snoozeVolume;
  s.mustPlayWhenSnoozing = _mustPlayWhenSnoozing;
  s.snoozeAdvert = _snoozeAdvert;
  EEPROM.put(0,s);
}
