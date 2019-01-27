/*************************
 ** AlarmController.h
 ************************/

#ifndef ALARMCONTROLLER_H
#define ALARMCONTROLLER_H

#include "Arduino.h"
#include "AlarmSettings.h"
#include "LiquidCrystal_I2C.h"
#include "Literals.h"
#include "MP3Player.h"
#include <DFRobotDFPlayerMini.h>
#include <SoftwareSerial.h>

class AlarmController {
  public:
    AlarmController(AlarmSettings* settings, LiquidCrystal_I2C* lcd, MP3Player* mp3, uint8_t ledPin, uint8_t buzzerPin);
    void initializeLCD();
    void initializeMP3();
    void printLCD();
    void doUp();
    void doDown();
    void doLeft();
    void endLeft();
    void doRight();
    void endRight();
    void doOK();
    void doCancel();
    void doSnooze();
    void setMakeLight(void (*makeLight)(uint8_t,int));
    void checkAlarmState();
    uint8_t getAlarmState();
    void controlBuzzer();
  protected:
    AlarmSettings* _settings;
    LiquidCrystal_I2C* _lcd;
    MP3Player* _mp3;
    char _lcdState;
    uint8_t _alarmState;
    bool _init;
    bool _changed;
    uint8_t _tday;
    uint8_t _thour;
    uint8_t _tmin;
    uint8_t _tvar;
    bool _tactive;
    uint8_t _alarmNo;
    uint8_t _cursor;
    uint8_t _ledPin;
    uint8_t _buzzerPin;
    uint8_t _activeAlarm;
    bool _playBuzzer;
    int _nextAlarmMin;
    uint8_t _snoozeCnt;
    bool _backlightOn;
    int _buttonPressedSec;
    void printString(char*);
    void printString(char*,char*,char*);
    void printMainScreen();
    void printMenuA();
    void printAlarmMenu();
    void printSetAlarm();
    void printMenuT();
    void printMenuU();
    void printMenuD();
    void printSetHour();
    void printSetDay();
    void printMenuI();
    void printMenuW();
    void printMenuL();
    void printSetWakeUpDur();
    void printSetWakeUpLum();
    void printTurnOffAlarm();
    void printWakeUp();
    void print1stAlarm();
    void print2ndAlarm();
    void printMenuY();
    void printSetSnooze();
    void printMenuQ();
    void printSetBacklight();
    void printMenuF();
    void printMenuG();
    void printMenuS();
    void printMenuH();
    void printMenuEqual();
    void printMenuQuestionMark();
    void printSetSound();
    void printSetAlarmVolume();
    void printSetSnoozeVolume();
    void printSetSnoozeAdvert();
    void controlBacklight();
    void (*_makeLight)(uint8_t,int);
};


#endif
