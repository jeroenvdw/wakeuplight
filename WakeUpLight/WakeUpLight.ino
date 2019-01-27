#include <LiquidCrystal_I2C.h>    // https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library
#include <Bounce2.h>              // https://github.com/thomasfredericks/Bounce2
#include <DFRobotDFPlayerMini.h>  // https://github.com/DFRobot/DFRobotDFPlayerMini
#include <ds3231.h>               // https://github.com/rodan/ds3231
#include <SoftwareSerial.h>
#include <Wire.h>
#include "Time.h"
#include "AlarmController.h"
#include "AlarmSettings.h"
#include "Alarm.h"
#include "WeekAlarm.h"
#include "MP3Player.h"


const uint8_t snoozePin = 12;   //Purple 
const uint8_t leftPin = 2;      //Green
const uint8_t rightPin = 3;     //Brown
const uint8_t upPin = 4;        //Blue
const uint8_t downPin = 5;      //Orange
const uint8_t okPin = 6;        //Yellow
const uint8_t cancelPin = 7;    //Grey
const uint8_t ledPin = 9;       //White

const uint8_t RXPin = 0;
const uint8_t TXPin = 1;
const uint8_t buzzerPin = 13;

const uint8_t bounceDelay = 6;

double fadeValue = 0.0;
unsigned long timeStart = 0;
unsigned long timeHigh = 0;
unsigned long timeOn = 0;
unsigned long timeOnPrev = 0;

SoftwareSerial swSerial(RXPin, TXPin); 
DFRobotDFPlayerMini dfPlayer;
MP3Player mp3(&swSerial,&dfPlayer);

AlarmSettings set;
LiquidCrystal_I2C lcd(0x27,16,2); 
AlarmController ac(&set,&lcd,&mp3,ledPin,buzzerPin);

Bounce btnLeft = Bounce();
Bounce btnRight = Bounce();
Bounce btnUp = Bounce();
Bounce btnDown = Bounce();
Bounce btnOK = Bounce();
Bounce btnCancel = Bounce();
Bounce btnSnooze = Bounce();

uint8_t lastUpdateHour = 0;


void setup()  { 
  ac.initializeLCD();
  ac.initializeMP3();
  DS3231_init(DS3231_CONTROL_INTCN);
  set.updateTimeFromRTC();
  set.loadFromEEPROM();
  
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(leftPin, INPUT);
  pinMode(rightPin, INPUT);
  pinMode(upPin, INPUT);
  pinMode(downPin, INPUT);
  pinMode(okPin, INPUT);
  pinMode(cancelPin, INPUT);
  pinMode(snoozePin, INPUT);
  btnLeft.attach(leftPin);
  btnLeft.interval(bounceDelay);
  btnRight.attach(rightPin);
  btnRight.interval(bounceDelay);
  btnUp.attach(upPin);
  btnUp.interval(bounceDelay);
  btnDown.attach(downPin);
  btnDown.interval(bounceDelay);
  btnOK.attach(okPin);
  btnOK.interval(bounceDelay);
  btnCancel.attach(cancelPin);
  btnCancel.interval(bounceDelay);
  btnSnooze.attach(snoozePin);
  btnSnooze.interval(bounceDelay);
  
  timeStart = millis();
  timeOn = millis();
  timeOnPrev = timeOn;

  digitalWrite(buzzerPin, LOW);
  analogWrite(ledPin, 0);

  ac.setMakeLight(analogWrite);
} 

void loop()  { 
  //update time
  timeOn = millis();
  if(timeOn<timeOnPrev) { //overflow!!
    set.getTime()->addMillis((4294967295ul - timeOnPrev) + timeOn);
  }
  else{
    set.getTime()->addMillis(timeOn - timeOnPrev);
  }
  timeOnPrev = timeOn;
  //every 12 hours (at 4:30 and 16:30), update the time from RTC
  if((set.getTime()->getHour() == 4 || set.getTime()->getHour() == 16) && (set.getTime()->getMin() == 30)) {
    if(lastUpdateHour != set.getTime()->getHour())  {
      set.updateTimeFromRTC();
      lastUpdateHour = set.getTime()->getHour();
    }
  }
  
  //update AlarmState
  ac.checkAlarmState();  
  
  //update LCD
  ac.printLCD();

  //check snooze-button
  btnSnooze.update();
  if(btnSnooze.risingEdge()) {
    ac.doSnooze();
    if(ac.getAlarmState() == 0) {
      timeStart = millis();
      btnSnooze.update();
      ac.printLCD();
      fadeValue = 0.0;
      analogWrite(ledPin,round(fadeValue));
      while(btnSnooze.read() == HIGH) {
        if(abs(millis() - timeStart) > 600) {
          fadeValue += 0.5;
          if(fadeValue > 255) {
            fadeValue = 255.0;
          }
          analogWrite(ledPin,round(fadeValue));
          delay(10);
        }
        btnSnooze.update();
      }
    }
  }

  //Check other buttons
  btnLeft.update();
  btnRight.update();
  btnUp.update();
  btnDown.update();
  btnOK.update();
  btnCancel.update();

  if(btnOK.risingEdge()) {
    ac.doOK();
  }
  else if(btnCancel.risingEdge()) {
    ac.doCancel(); 
  }
  else if(btnLeft.risingEdge()) {
    ac.doLeft();
  }
  else if(btnLeft.fallingEdge()) {
    ac.endLeft();
  }
  else if(btnRight.risingEdge()) {
    ac.doRight();
  }
  else if(btnRight.fallingEdge()) {
    ac.endRight(); 
  }
  else if(btnUp.risingEdge()) {
    ac.doUp();
    btnUp.update();
    timeStart = millis();
    ac.printLCD();
    while(btnUp.read() == HIGH) {
      if(abs(millis() - timeStart) > 800) {
        ac.doUp();
        ac.printLCD();
        delay(140);
      }
      btnUp.update();
    }
  }
  else if(btnDown.risingEdge()) {
    ac.doDown();
    btnUp.update();
    timeStart = millis();
    ac.printLCD();
    while(btnDown.read() == HIGH) {
      if(abs(millis() - timeStart) > 800) {
        ac.doDown();
        ac.printLCD();
        delay(140);
      }
      btnDown.update();
    }
  }
  
}
