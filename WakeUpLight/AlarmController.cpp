/*************************
 ** AlarmController.cpp
 ************************/

#include "AlarmController.h"
#include "Literals.h"
#include <avr/pgmspace.h>
using namespace AlarmLiterals;

const char SPACE = ' ';
const char COLON = ':';
const char ZERO = '0';
const char* LEFT = "<-";
const char* RIGHT = "->";
const char SPACE2[] = {SPACE,SPACE,'\0'};
const char* EMPTY = "";

const uint8_t LEDOFF = 0;
const uint8_t LEDON = 1;
const uint8_t SPEAKEROFF = 2;
const uint8_t SPEAKERON = 3;
const uint8_t NEXTSONG = 4;

unsigned long buzzerStart = 0;
// buzzer pattern       x        x        x        x        x      x        x      x        x      x      x        x      x      x        x      x      x      x        x      x      x      x        x      x      x      x      x        x      x      x      x      x
const int buzzerP[] = {150,2000,150,2000,150,2000,150,2000,150,80,150,2000,150,80,150,2000,150,80,150,80,150,2000,150,80,150,80,150,2000,150,80,150,80,150,80,150,2000,150,80,150,80,150,80,150,2000,150,80,150,80,150,80,150,80,150,2000,150,80,150,80,150,80,150,80,150,1000};
const int buzzerPL = 64; //buzzer pattern length
const uint8_t buzzerL = 54; //buzzer loop
int buzzerPI = 0; //buzzer pattern index


AlarmController::AlarmController(AlarmSettings* settings, LiquidCrystal_I2C* lcd,MP3Player* mp3, uint8_t ledPin, uint8_t buzzerPin) {
  _lcd = lcd;
  _mp3 = mp3;
  _settings = settings;
  _lcdState = 'M';
  _makeLight = NULL;
  _ledPin = ledPin;
  _buzzerPin = buzzerPin;
  _playBuzzer = false;
  _activeAlarm = 8;
  _backlightOn = true;
  _buttonPressedSec = _settings->getTime()->getSec();
}

void AlarmController::initializeLCD() {
  _lcd->begin();  
  _lcd->setBacklight(1);
  _lcd->home();
  _lcd->setCursor(0,0);
  _lcd->print(F("setup lcd"));
  
  uint8_t ledOff[8] = { 0b00000,
                        0b00000,
                        0b00000,
                        0b00000,
                        0b00100,
                        0b01110,
                        0b01110,
                        0b01110 };
  uint8_t ledOn[8] = { 0b00100,
                       0b10101,
                       0b10101,
                       0b00000,
                       0b00100,
                       0b01110,
                       0b01110,
                       0b01110 };

  _lcd->createChar(LEDOFF, ledOff);
  _lcd->createChar(LEDON, ledOn);

  uint8_t speakerOff[8] = { 0b00100,
                            0b01100,
                            0b11100,
                            0b11100,
                            0b11100,
                            0b11100,
                            0b01100,
                            0b00100 };
  uint8_t speakerOn[8] = { 0b00101,
                           0b01101,
                           0b11100,
                           0b11101,
                           0b11101,
                           0b11100,
                           0b01101,
                           0b00101 };

  uint8_t nextSong[8] = { 0b00100,
                          0b01110,
                          0b10101,
                          0b00100,
                          0b00100,
                          0b00100,
                          0b00100,
                          0b00100 };

  _lcd->createChar(SPEAKEROFF, speakerOff);
  _lcd->createChar(SPEAKERON, speakerOn);
  _lcd->createChar(NEXTSONG, nextSong);
}

void AlarmController::initializeMP3() {
  _lcd->setCursor(0,0);
  _lcd->print(F("setup mp3"));
  _mp3->initialize();
  _mp3->setVolume(_settings->getAlarmVolume());
}


void AlarmController::printString(char* string, char* sleft, char* sright) {
  //strcpy_P(buffer, (char*)pgm_read_word(&(string)));
  int len = strlen(string);
  int spaces = 16 - len - strlen(sleft) - strlen(sright);
  _lcd->print(sleft);
  for(int i=0; i<spaces/2; i++) {
    _lcd->print(SPACE);
  }
  _lcd->print(string);
  for(int i=0; i<spaces/2+spaces%2; i++) {
    _lcd->print(SPACE);
  }
  _lcd->print(sright);
}

void AlarmController::printString(char* string) {
  printString(string,EMPTY,EMPTY);
}

void AlarmController::printLCD() {
  controlBacklight();
  if(_lcdState == 'M') {
    printMainScreen();
  } else if(_lcdState == 'A') {
    printMenuA();
  } else if(_lcdState == 'B') {
    printAlarmMenu();
  } else if(_lcdState == 'C') {
    printSetAlarm();
  } else if(_lcdState == 'T') {
    printMenuT();
  } else if(_lcdState == 'U') {
    printMenuU();
  } else if(_lcdState == 'D') {
    printMenuD();
  } else if(_lcdState == 'V') {
    printSetHour();
  } else if(_lcdState == 'E') {
    printSetDay();
  } else if(_lcdState == 'I') {
    printMenuI();
  } else if(_lcdState == 'W') {
    printMenuW();
  } else if(_lcdState == 'L') {
    printMenuL();
  } else if(_lcdState == 'X') {
    printSetWakeUpDur();
  } else if(_lcdState == 'N') {
    printSetWakeUpLum();
  } else if(_lcdState == 'O') {
    printWakeUp();
  } else if(_lcdState == 'P') {
    printTurnOffAlarm();
  } else if(_lcdState == 'J') {
    print1stAlarm();
  } else if(_lcdState == 'K') {
    print2ndAlarm();
  } else if(_lcdState == 'Y') {
    printMenuY();
  } else if(_lcdState == 'Z') {
    printSetSnooze();
  } else if(_lcdState == 'Q') {
    printMenuQ();
  } else if(_lcdState == 'R') {
    printSetBacklight();
  } else if(_lcdState == 'F') {
    printMenuF();
  } else if(_lcdState == '?') {
    printSetSnoozeAdvert();
  } else if(_lcdState == 'H') {
    printMenuH();
  } else if(_lcdState == '$') {
    printSetSound();
  } else if(_lcdState == 'S') {
    printMenuS();
  } else if(_lcdState == '@') {
    printSetAlarmVolume();
  } else if(_lcdState == '!') {
    printSetSnoozeVolume();
  } else if(_lcdState == '=') {
    printMenuEqual();
  } else if(_lcdState == 'G') {
    printMenuG();
  }
  controlBacklight();
}

void AlarmController::doOK() {
  _init = true;
  _lcd->noCursor();
  _buttonPressedSec = _settings->getTime()->getSec();
  if(_lcdState == 'M') {
    _lcdState = 'A';
  } else if(_lcdState == 'A') {
    _lcdState = 'B';
  } else if(_lcdState == 'B') {
    _settings->setAlarmConfigNumber(_alarmNo);
    _lcdState = 'C';
  } else if(_lcdState == 'C') {
    _settings->setAlarmHour(_thour);
    _settings->setAlarmMin(_tmin);
    _settings->setAlarmActive(_tactive);
    _settings->saveToEEPROM();
    _lcdState = 'B';
  } else if(_lcdState == 'T') {
    _lcdState = 'U';
  } else if(_lcdState == 'U') {
    _lcdState = 'V';
  } else if(_lcdState == 'D') {
    _lcdState = 'E';
  } else if(_lcdState == 'V') {
    _settings->getTime()->setTime(_thour,_tmin);
    _settings->updateRTCFromTime();
    _cursor = 6;
    _lcdState = 'U';
  } else if(_lcdState == 'E') {
    _settings->getTime()->setDay(_tday);
    _settings->updateRTCFromTime();
    _lcdState = 'D';
  } else if(_lcdState == 'I') {
    _lcdState = 'W';
  } else if(_lcdState == 'W') {
    _lcdState = 'X';
  } else if(_lcdState == 'L') {
    _lcdState = 'N';
  } else if(_lcdState == 'X') {
    _settings->setWakeUpDur(_tmin);
    _settings->saveToEEPROM();
    _lcdState = 'W';
  } else if(_lcdState == 'N') {
    _settings->setWakeUpLum(_tmin);
    _settings->saveToEEPROM();
    _makeLight(_ledPin,0);
    _lcdState = 'L';
  } else if(_lcdState == 'O') {
    _lcdState = 'P';
    _alarmState = 3;
  } else if(_lcdState == 'P') {
    _lcdState = 'M';
    _alarmState = 0;
    _mp3->stop();
    if(_activeAlarm == 7) {
      _settings->setAlarmActive(_activeAlarm,false);
      _settings->saveToEEPROM();
    }
  } else if(_lcdState == 'Y') {
    _lcdState = 'Z';
  } else if(_lcdState == 'Z') {
    _lcdState = 'Y';
    _settings->setSnoozeDur(_tmin);
    _settings->saveToEEPROM();
  } else if(_lcdState == 'Q') {
    _lcdState = 'R';
  } else if(_lcdState == 'R') {
    _lcdState = 'Q';
    _settings->setBacklightDur(_tmin);
    _settings->saveToEEPROM();
  } else if(_lcdState == 'F') {
    _lcdState = '?';
  } else if(_lcdState == 'G') {
    _lcdState = '!';
  } else if(_lcdState == '@') {
    _settings->setAlarmVolume(_tvar);
    _settings->saveToEEPROM();
    _mp3->stop();
    _lcdState = 'S';
  } else if(_lcdState == '!') {
    _settings->setSnoozeVolume(_tvar);
    if(_tvar == 0) {
      _settings->setMustPlayWhenSnoozing(false);
    } else {
      _settings->setMustPlayWhenSnoozing(true);
    }
    _settings->saveToEEPROM();
    _mp3->stop();
    _lcdState = 'G';
  } else if(_lcdState == '$') {
    _settings->setSoundNo(_tvar);
    _settings->saveToEEPROM();
    _mp3->stop();
    _lcdState = 'H';
  } else if(_lcdState == 'H') {
    _lcdState = '$';
  } else if(_lcdState == 'S') {
    _lcdState = '@';
  } else if(_lcdState == '?') {
    _settings->setSnoozeAdvert(_tvar);
    _lcdState = 'F';
  }else if(_lcdState == '=') {
    _lcdState = 'H';
  }
}

void AlarmController::doCancel() {
  _init = true;
  _lcd->noCursor();
  _buttonPressedSec = _settings->getTime()->getSec();
  if(_lcdState == 'M') {
    //nothing
  } else if(_lcdState == 'A') {
    _lcdState = 'M';
  } else if(_lcdState == 'B') {
    _alarmNo = _settings->getAlarmConfigNumber();
    _lcdState = 'A';
  } else if(_lcdState == 'C') {
    _thour = _settings->getAlarmHour();
    _tmin = _settings->getAlarmMin();
    _tactive = _settings->isAlarmActive();
    _lcdState = 'B';
  } else if(_lcdState == 'T') {
    _lcdState = 'M';
  } else if(_lcdState == 'U') {
    _lcdState = 'T';
  } else if(_lcdState == 'D') {
    _lcdState = 'T';
  } else if(_lcdState == 'V') {
    _thour = _settings->getTime()->getHour();
    _tmin = _settings->getTime()->getMin();
    _cursor = 6;
    _lcdState = 'U';
  } else if(_lcdState == 'E') {
    _tday = _settings->getTime()->getDay();
    _lcdState = 'D';
  } else if(_lcdState == 'I') {
    _lcdState = 'M';
  } else if(_lcdState == 'W') {
    _lcdState = 'I';
  } else if(_lcdState == 'L') {
    _lcdState = 'I';
  } else if(_lcdState == 'X') {
    _tmin = _settings->getWakeUpDur();
    _lcdState = 'W';
  } else if(_lcdState == 'N') {
    _tmin = _settings->getWakeUpLum();
    _makeLight(_ledPin,0);
    _lcdState = 'L';
  } else if(_lcdState == 'P') {
    _alarmState = 3;
    _lcdState = 'O';
  } else if(_lcdState == 'Y') {
    _lcdState = 'I';
  } else if(_lcdState == 'Z') {
    _lcdState = 'Y';
  } else if(_lcdState == 'Q') {
    _lcdState = 'I';
  } else if(_lcdState == 'R') {
    _lcdState = 'Q';
  } else if(_lcdState == 'F') {
    _lcdState = '=';
  } else if(_lcdState == 'G') {
    _lcdState = '=';
  } else if(_lcdState == '@') {
    _tvar = _settings->getAlarmVolume();
    _mp3->stop();
    _lcdState = 'S';
  } else if(_lcdState == '!') {
    _tvar = _settings->getSnoozeVolume();
    _mp3->stop();
    _lcdState = 'G';
  } else if(_lcdState == '$') {
    _tvar = _settings->getSoundNo();
    _mp3->stop();
    _lcdState = 'H';
  } else if(_lcdState == 'H') {
    _lcdState = '=';
  } else if(_lcdState == 'S') {
    _lcdState = '=';
  } else if(_lcdState == 'G') {
    _lcdState = '=';
  } else if(_lcdState == '=') {
    _lcdState = 'M';
  } else if(_lcdState == '?') {
    _tvar = _settings->getSnoozeAdvert();
    _lcdState = 'F';
  }
}

void AlarmController::doUp() {
  _changed = true;
  _buttonPressedSec = _settings->getTime()->getSec();
  if(_lcdState == 'C') {
    if(_cursor == 2) {
      _thour = (_thour+1) % 24;
    }
    else if(_cursor == 5){
      _tmin = (_tmin+1) % 60;
    }
    else {
      _tactive = !_tactive;
    }
  } else if(_lcdState == 'V') {
    if(_cursor == 6) {
      _thour = (_thour+1) % 24;
    }
    else {
      _tmin = (_tmin+1) % 60;
    }
  } else if(_lcdState == 'X') {
    _tmin = (_tmin+1) % 61;
  } else if(_lcdState == 'N') {
    if(_cursor == 5) {
      if(_tmin < 250) 
        _tmin+=5;
      else
        _tmin = 255;
    }
    else {
      _tactive = !_tactive;
    }
    if(_tactive && _makeLight != NULL) {
        _makeLight(_ledPin,_tmin);
    } else if(_makeLight != NULL) {
        _makeLight(_ledPin,0);
    }
  } else if(_lcdState == 'Z') {
    _tmin = (_tmin+1) % 61;
  } else if(_lcdState == 'R') {
    _tmin = (_tmin+1) % 61;
  } else if(_lcdState == '?') {
    _tvar = (_tvar+1) % 11;
  } else if(_lcdState == '$') {
    if(_cursor == 7) {
      _tvar = (_tvar+1) % 31;
      if(_tvar == 0) {
        _tactive = false;
      }
    } else if(_cursor == 11) {
      _tactive = !_tactive;
    } else {
       _tactive = true;
       _mp3->setVolume(_settings->getAlarmVolume());
       _mp3->play(_tvar);
       _mp3->next();
    }
    if(_tactive) {
      _mp3->setVolume(_settings->getAlarmVolume());
      _mp3->play(_tvar);
    } else {
      _mp3->setVolume(_settings->getAlarmVolume());
      _mp3->pause();
    }
  } else if(_lcdState == '@') {
    if(_cursor == 5) {
      if(_tvar < 30) 
        _tvar+=1;
      else
        _tvar = 30;
    }
    else {
      _tactive = !_tactive;
    }
    if(_tactive) {
      _mp3->setVolume(_tvar);
      _mp3->play(_settings->getSoundNo());
    } else {
      _mp3->setVolume(_settings->getAlarmVolume());
      _mp3->pause();
    }
  } else if(_lcdState == '!') {
    if(_cursor == 5) {
      if(_tvar < 30) 
        _tvar+=1;
      else
        _tvar = 30;
    }
    else {
      _tactive = !_tactive;
    }
    if(_tactive) {
      _mp3->setVolume(_tvar);
      _mp3->play(_settings->getSoundNo());
    } else {
      _mp3->setVolume(_settings->getSnoozeVolume());
      _mp3->pause();
    }
  }
}

void AlarmController::doDown() {
  _changed = true;
  _buttonPressedSec = _settings->getTime()->getSec();
  if(_lcdState == 'C') {
    if(_cursor == 2) {
      if(_thour > 0)
        _thour--;
      else
        _thour = 23;
    }
    else if(_cursor == 5){
      if(_tmin > 0)
        _tmin--;
      else 
        _tmin = 59;
    }
    else {
      _tactive = !_tactive;
    }
  } else if(_lcdState == 'V') {
    if(_cursor == 6) {
      if(_thour > 0)
        _thour--;
      else
        _thour = 23;
    }
    else {
      if(_tmin > 0)
        _tmin--;
      else 
        _tmin = 59;
    }
  } else if(_lcdState == 'X') {
    if(_tmin > 0) 
      _tmin--;
    else
      _tmin = 60;
  } else if(_lcdState == 'N') {
    if(_cursor == 5) {
      if(_tmin > 5) 
        _tmin-=5;
      else
        _tmin = 0;
    } else {
      _tactive = !_tactive;
    }
    if(_tactive && _makeLight != NULL) {
        _makeLight(_ledPin,_tmin);
    } else if(_makeLight != NULL) {
        _makeLight(_ledPin,0);
    }
  } else if(_lcdState == 'Z') {
    if(_tmin > 0) 
      _tmin--;
    else
      _tmin = 60;
  } else if(_lcdState == 'R') {
    if(_tmin > 0) 
      _tmin--;
    else
      _tmin = 60;
  } else if(_lcdState == '?') {
    if(_tvar > 0)
      _tvar--;
    else
      _tvar = 10;
  } else if(_lcdState == '@') {
    if(_cursor == 5) {
      if(_tvar > 0) 
        _tvar-=1;
      else
        _tvar = 0;
    }
    else {
      _tactive = !_tactive;
    }
    if(_tactive) {
      _mp3->setVolume(_tvar);
      _mp3->play(_settings->getSoundNo());
    } else {
      _mp3->setVolume(_settings->getAlarmVolume());
      _mp3->pause();
    }
  } else if(_lcdState == '$') {
    if(_cursor == 7) {
      if(_tvar > 0) {
        _tvar--;
        if(_tvar == 0) {
          _tactive = false;
        }
      } else {
        _tvar = 30;
      }
    } else if(_cursor == 11) {
      _tactive = !_tactive;
    } 
    if(_tactive) {
      _mp3->setVolume(_settings->getAlarmVolume());
      _mp3->play(_tvar);
    } else {
      _mp3->setVolume(_settings->getAlarmVolume());
      _mp3->pause();
    }
  } else if(_lcdState == '!') {
    if(_cursor == 5) {
      if(_tvar > 0) 
        _tvar-=1;
      else
        _tvar = 0;
    }
    else {
      _tactive = !_tactive;
    }
    if(_tactive) {
      _mp3->setVolume(_tvar);
      _mp3->play(_settings->getSoundNo());
    } else {
      _mp3->setVolume(_settings->getSnoozeVolume());
      _mp3->pause();
    }
  }
}

void AlarmController::doLeft() {
  _buttonPressedSec = _settings->getTime()->getSec();
  if(_lcdState == 'M') {
    _lcdState = 'J';
  } else if(_lcdState == 'T') {
    _lcdState = 'A';
  } else if(_lcdState == 'B') {
    _changed = true;
    if(_alarmNo > 0)
      _alarmNo--; // = (_alarmNo-1) % _settings->getAlarmCount();
    else
      _alarmNo = _settings->getAlarmCount() - 1;
  } else if(_lcdState == 'C') {
      if(_cursor == 5) {
        _cursor = 2;
      }
      else if(_cursor == 11){
        _cursor = 5;
      }
      _changed = true;
  } else if(_lcdState == 'E') {
    if(_tday == 0) 
      _tday = 6;
    else 
      _tday--;
    _changed = true;
  } else if(_lcdState == 'I') {
    _lcdState = '=';
  } else if(_lcdState == 'D') {
    _lcdState = 'U';
  } else if(_lcdState == 'L') {
    _lcdState = 'Q';
  } else if(_lcdState == 'N') {
    if(_cursor == 12) {  // "   100 %    X    "
      _cursor = 5;
    }
    _changed = true;
  } else if(_lcdState == 'V') {
      if(_cursor == 9) {
        _cursor = 6;
      }
      _changed = true;
  } else if(_lcdState == 'Y') {
    _lcdState = 'W';
  } else if(_lcdState == 'Q') {
    _lcdState = 'Y';
  } else if(_lcdState == 'F') {
    _lcdState = 'G';
  } else if(_lcdState == '@') {
    if(_cursor == 12) {  // "   10       X    "
      _cursor = 5;
    }
    _changed = true;
  } else if(_lcdState == '!') {
    if(_cursor == 12) {  // "   10       X    "
      _cursor = 5;
    }
    _changed = true;
  } else if(_lcdState == '$') {
    if(_cursor == 11) {
      _cursor = 7; 
    } if(_cursor == 13) {
      _cursor = 11;
    }
    _changed = true;
  } else if(_lcdState == 'S') {
    _lcdState = 'H';
  } else if(_lcdState == 'G') {
    _lcdState = 'S';
  } else if(_lcdState == '=') {
    _lcdState = 'T';
  }
}

void AlarmController::doRight() {
  _buttonPressedSec = _settings->getTime()->getSec();
  if(_lcdState == 'M') {
    _lcdState = 'K';
  } else if(_lcdState == 'A') {
    _lcdState = 'T';
  } else if(_lcdState == 'B') {
    _changed = true;
    _alarmNo = (_alarmNo+1) % _settings->getAlarmCount();
  } else if(_lcdState == 'C') {
    if(_cursor == 2) {
      _cursor = 5;
    }
    else if(_cursor == 5){
      _cursor = 11;
    }
    _changed = true;
  } else if(_lcdState == 'E' ) {
    _tday = (_tday + 1) % 7;
    _changed = true;
  } else if(_lcdState == 'N') {
    if(_cursor == 5) {
      _cursor = 12;
    }
    _changed = true;
  } else if(_lcdState == 'T') {
    _lcdState = '=';
  } else if(_lcdState == 'U') {
    _lcdState = 'D';
  } else if(_lcdState == 'W') {
    _lcdState = 'Y';
  } else if(_lcdState == 'Y') {
    _lcdState = 'Q';
  } else if(_lcdState == 'Q') {
    _lcdState = 'L';
  } else if(_lcdState == 'V') {
      if(_cursor == 6) {
        _cursor = 9;
      }
      _changed = true;
  } else if(_lcdState == 'G') {
    _lcdState = 'F';
  } else if(_lcdState == 'S') {
    _lcdState = 'G';
  } else if(_lcdState == '@') {
    if(_cursor == 5) {
      _cursor = 12;
    }
    _changed = true;
  } else if(_lcdState == '!') {
    if(_cursor == 5) {
      _cursor = 12;
    }
    _changed = true;
  } else if(_lcdState == '$') {
    if(_cursor == 7) {
      _cursor = 11;
    } else if(_cursor == 11) {
      _cursor = 13;
    }
    _changed = true;
  } else if(_lcdState == 'H') {
    _lcdState = 'S';
  } else if(_lcdState == '=') {
    _lcdState = 'I';
  }
}

void AlarmController::endLeft() {
  _buttonPressedSec = _settings->getTime()->getSec();
  if(_lcdState == 'J') {
    _lcdState = 'M';
  }
}

void AlarmController::endRight() {
  _buttonPressedSec = _settings->getTime()->getSec();
  if(_lcdState == 'K') {
    _lcdState = 'M';
  }
}

void AlarmController::doSnooze() {
  _buttonPressedSec = _settings->getTime()->getSec();
  if(_lcdState == 'O') {
    _alarmState = 3;
  }
}

void AlarmController::printMainScreen() {
  _lcd->setCursor(0,0);
  uint8_t s = 16 - strlen(_settings->getTime()->getDayString());
  for(int i=0;i<s/2;i++)
    _lcd->print(SPACE);
  _lcd->print(_settings->getTime()->getDayString());
  for(int i=0;i<s/2;i++)
    _lcd->print(SPACE);
  if(s%2==1)
    _lcd->print(SPACE);
  _lcd->setCursor(0,1); 
  _lcd->print(SPACE);
  _lcd->print(SPACE);
  _lcd->print(SPACE);
  _lcd->print(SPACE);
  _lcd->print(_settings->getTime()->getTimeString(true)); //8 or 5 chars
  _lcd->print(SPACE);
  _lcd->print(SPACE);
  _lcd->print(SPACE);
  _lcd->print(SPACE);
}

void AlarmController::printMenuA() {
  _lcd->setCursor(0,0);
  printString(MENU);
  _lcd->setCursor(0,1);
  printString(ALARM,SPACE2,RIGHT);
}

void AlarmController::printAlarmMenu() {
  if(_init) {
    _alarmNo = _settings->getAlarmConfigNumber();
    _init = false;
    _changed = true;
  }
  if(_changed) {
    _lcd->setCursor(0,0);
    _lcd->print(LEFT);
    uint8_t s = 12 - strlen(_settings->getAlarmTitle(_alarmNo));
    for(uint8_t i=0;i<s/2;i++) {
      _lcd->print(SPACE);
    }
    _lcd->print(_settings->getAlarmTitle(_alarmNo));
    for(uint8_t i=0;i<s/2;i++) {
      _lcd->print(SPACE);
    }
    if(s%2 == 1) {
      _lcd->print(SPACE);
    }
    _lcd->print(RIGHT);
    _lcd->setCursor(0,1);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    if(_settings->getAlarmHour(_alarmNo) < 10) {
      _lcd->print(ZERO);
    }
    _lcd->print(_settings->getAlarmHour(_alarmNo));
    _lcd->print(':');
    if(_settings->getAlarmMin(_alarmNo) < 10) {
      _lcd->print(ZERO);
    }
    _lcd->print(_settings->getAlarmMin(_alarmNo));
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    if(_settings->isAlarmActive(_alarmNo)) {
      _lcd->print(ON);
    }
    else {
      _lcd->print(OFF);
    }
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _changed = false;
  }
  
}

void AlarmController::printSetAlarm() {
  if(_init) {
    _thour = _settings->getAlarmHour();
    _tmin = _settings->getAlarmMin();
    _tactive = _settings->isAlarmActive();
    _cursor = 2;
    _lcd->setCursor(0,0);
    int s = 15 - strlen(_settings->getAlarmTitle());
    for(int i=0;i<s/2;i++) {
      _lcd->print(SPACE);
    }
    _lcd->print(_settings->getAlarmTitle());
    _lcd->print(COLON);
    for(int i=0;i<s/2;i++) {
      _lcd->print(SPACE);
    }
    if(s%2 == 1) {
      _lcd->print(SPACE);
    }
    _init = false;
    _changed = true;
  }
  if(_changed) {
    _lcd->setCursor(0,1);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    if(_thour < 10) {
      _lcd->print(ZERO);
    }
    _lcd->print(_thour);
    _lcd->print(':');
    if(_tmin < 10) {
      _lcd->print(ZERO);
    }
    _lcd->print(_tmin);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    if(_tactive) {
      _lcd->print(ON);
    }
    else {
      _lcd->print(OFF);
    }
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _lcd->setCursor(_cursor,1);
    _changed = false;
  }  
  if(_settings->getTime()->getMillis() % 1000 > 500) {
    _lcd->noCursor();
  }
  else {
    _lcd->cursor();
  }
}

void AlarmController::printMenuT() {
  _lcd->setCursor(0,0);
  printString(MENU);
  _lcd->setCursor(0,1);
  printString(TIJD,LEFT,RIGHT);
}

void AlarmController::printMenuU() {
  _lcd->setCursor(0,0);
  printString(TIJD);
  _lcd->setCursor(0,1);
  printString(UUR,SPACE2,RIGHT);
}

void AlarmController::printMenuD() {
  _lcd->setCursor(0,0);
  printString(TIJD);
  _lcd->setCursor(0,1);
  printString(DAG,LEFT,SPACE2);
}

void AlarmController::printSetHour() {
  if(_init) {
    _thour = _settings->getTime()->getHour();
    _tmin = _settings->getTime()->getMin();
    _init = false;
    _cursor = 6;
    _changed = true;
    _lcd->setCursor(0,0);
    printString(UUR);
  }
  if(_changed) {
    _lcd->setCursor(0,1);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    if(_thour < 10)
      _lcd->print(ZERO);
    _lcd->print(_thour);
    _lcd->print(COLON);
    if(_tmin < 10)
      _lcd->print(ZERO);
    _lcd->print(_tmin);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _lcd->setCursor(_cursor,1);
    _changed = false;
  }
  if(_settings->getTime()->getMillis() % 1000 > 500) {
    _lcd->noCursor();
  }
  else {
    _lcd->cursor();
  }
}

void AlarmController::printSetDay() {
  if(_init) {
    _tday = _settings->getTime()->getDay();
    _lcd->setCursor(0,0);
    printString(DAG);
    _init = false;
    _changed = true;
  }
  if(_changed) {
    _lcd->setCursor(0,1);
    _lcd->print(LEFT);
    uint8_t s = 12 - strlen(_settings->getTime()->getDayString(_tday));
    for(uint8_t i=0;i<s/2;i++) {
      _lcd->print(SPACE);
    }
    _lcd->print(_settings->getTime()->getDayString(_tday));
    for(uint8_t i=0;i<s/2;i++) {
      _lcd->print(SPACE);
    }
    if(s%2 == 1) {
      _lcd->print(SPACE);
    }
    _lcd->print(RIGHT);
    _changed = false;
  }
  if(_settings->getTime()->getMillis() % 1000 > 500) {
    _lcd->noCursor();
  }
  else {
    _lcd->cursor();
  }
}

void AlarmController::printMenuI() {
  _lcd->setCursor(0,0);
  printString(MENU);
  _lcd->setCursor(0,1);
  printString(INST,LEFT,SPACE2);
}

void AlarmController::printMenuW() {
  _lcd->setCursor(0,0);
  printString(INST);
  _lcd->setCursor(0,1);
  printString(WUDUR,SPACE2,RIGHT);
}

void AlarmController::printMenuL() {
  _lcd->setCursor(0,0);
  printString(INST);
  _lcd->setCursor(0,1);
  printString(WULUM,LEFT,SPACE2);
}
void AlarmController::printSetWakeUpDur() { // state = X
  if(_init) {
    _lcd->setCursor(0,0);
    printString(WUDUR);
    _tmin = _settings->getWakeUpDur();
    _changed = true;
    _init = false;
  }
  if(_changed) {
    _lcd->setCursor(0,1);
    //"     20 min     ";
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    if(_tmin < 10) 
      _lcd->print(ZERO);
    _lcd->print(_tmin);
    _lcd->print(SPACE);
    _lcd->print(MIN);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _changed = false;
  }
  if(_settings->getTime()->getMillis() % 1000 > 500) {
    _lcd->setCursor(6,1);
    _lcd->noCursor();
  }
  else {
    _lcd->setCursor(6,1);
    _lcd->cursor();
  }
}
void AlarmController::printSetWakeUpLum() { // state = N
  if(_init) {
    _lcd->setCursor(0,0);
    printString(WULUM);
    _cursor = 5;
    _changed = true;
    _tactive = false;
    _tmin = _settings->getWakeUpLum();
    _init = false;
  }
  if(_changed) {
    _lcd->setCursor(0,1);
    //sprintf(_text2,"      %d %%      ",_wakeUpLum/2.55);
    //               "   100 %    X    "
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    uint8_t d = _tmin / 2.55;
    if(d<100)
      _lcd->print(SPACE);
    if(d<10)
      _lcd->print(SPACE);
    _lcd->print(d);
    _lcd->print(SPACE);
    _lcd->print('%');
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    if(_tactive) {
      _lcd->write(LEDON);
    }
    else {
      _lcd->write(LEDOFF);
    }
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _changed = false;
  }
  if(_settings->getTime()->getMillis() % 1000 > 500) {
    _lcd->setCursor(_cursor,1);
    _lcd->noCursor();
  }
  else {
    _lcd->setCursor(_cursor,1);
    _lcd->cursor();
  }
}

void AlarmController::printWakeUp() {
  _lcd->setCursor(0,0);
  printString(WAKEUP);
  _lcd->setCursor(0,1);
  _lcd->print(SPACE);
  _lcd->print(SPACE);
  _lcd->print(SPACE);
  _lcd->print(SPACE);
  _lcd->print(_settings->getTime()->getTimeString(true)); //8 or 5 chars
  _lcd->print(SPACE);
  _lcd->print(SPACE);
  _lcd->print(SPACE);
  _lcd->print(SPACE);
}

void AlarmController::printTurnOffAlarm() {
  _lcd->setCursor(0,0);
  printString(AL_OFF);
  _lcd->setCursor(0,1);
  printString(OK);
}

void AlarmController::print1stAlarm() {
  _lcd->setCursor(0,0);
  printString(ALARM1);
  _lcd->setCursor(0,1);
  uint8_t a = _settings->getFirstActiveAlarmNo();
  if(a==8) {
    printString(NONE);
  }
  else { 
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    if(a==7) {
      _lcd->print(SPACE);
    }
    else {
      char* t = _settings->getTime()->getDayString(a);
      _lcd->print(t[0]);
      _lcd->print(t[1]);
      _lcd->print(SPACE);
    }
    if(_settings->getAlarmHour(a) < 10)
      _lcd->print(ZERO);
    _lcd->print(_settings->getAlarmHour(a));
    _lcd->print(COLON);
    if(_settings->getAlarmMin(a) < 10) 
      _lcd->print(ZERO);
    _lcd->print(_settings->getAlarmMin(a));
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    if(a==7) {
      _lcd->print(SPACE);
      _lcd->print(SPACE);
    }
  }
}

void AlarmController::print2ndAlarm() {
  _lcd->setCursor(0,0);
  printString(ALARM2);
  _lcd->setCursor(0,1);
  uint8_t a = _settings->getSecondActiveAlarmNo();
  if(a==8) {
    printString(NONE);
  }
  else { 
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    if(a==7) {
      _lcd->print(SPACE);
    }
    else {
      char* t = _settings->getTime()->getDayString(a);
      _lcd->print(t[0]);
      _lcd->print(t[1]);
      _lcd->print(SPACE);
    }
    if(_settings->getAlarmHour(a) < 10)
      _lcd->print(ZERO);
    _lcd->print(_settings->getAlarmHour(a));
    _lcd->print(COLON);
    if(_settings->getAlarmMin(a) < 10) 
      _lcd->print(ZERO);
    _lcd->print(_settings->getAlarmMin(a));
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    if(a==7) {
      _lcd->print(SPACE);
      _lcd->print(SPACE);
    }
  }
}

void AlarmController::printMenuY() {
  _lcd->setCursor(0,0);
  printString(INST);
  _lcd->setCursor(0,1);
  printString(SNDUR,LEFT,RIGHT);
}

void AlarmController::printSetSnooze() {
  if(_init) {
    _lcd->setCursor(0,0);
    printString(SNDUR);
    _tmin = _settings->getSnoozeDur();
    _changed = true;
    _init = false;
  }
  if(_changed) {
    _lcd->setCursor(0,1);
    //"     20 min     ";
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    if(_tmin < 10) 
      _lcd->print(ZERO);
    _lcd->print(_tmin);
    _lcd->print(SPACE);
    _lcd->print(MIN);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _changed = false;
  }
  if(_settings->getTime()->getMillis() % 1000 > 500) {
    _lcd->setCursor(6,1);
    _lcd->noCursor();
  }
  else {
    _lcd->setCursor(6,1);
    _lcd->cursor();
  }
}

void AlarmController::printMenuQ() {
  _lcd->setCursor(0,0);
  printString(INST);
  _lcd->setCursor(0,1);
  printString(BLDUR,LEFT,RIGHT);
}

void AlarmController::printSetBacklight() {
  if(_init) {
    _lcd->setCursor(0,0);
    printString(BLDUR);
    _tmin = _settings->getBacklightDur();
    _changed = true;
    _init = false;
  }
  if(_changed) {
    _lcd->setCursor(0,1);
    //"     10 sec     ";
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    if(_tmin < 10) 
      _lcd->print(ZERO);
    _lcd->print(_tmin);
    _lcd->print(SPACE);
    _lcd->print(SEC);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _changed = false;
  }
  if(_settings->getTime()->getMillis() % 1000 > 500) {
    _lcd->setCursor(6,1);
    _lcd->noCursor();
  }
  else {
    _lcd->setCursor(6,1);
    _lcd->cursor();
  }
}

void AlarmController::printMenuF() {
  _lcd->setCursor(0,0);
  printString(SND);
  _lcd->setCursor(0,1);
  printString(SN_ADV,LEFT,SPACE2);
}

void AlarmController::printMenuG() {
  _lcd->setCursor(0,0);
  printString(SND);
  _lcd->setCursor(0,1);
  printString(SN_VOL,LEFT,RIGHT);
}

void AlarmController::printMenuEqual() {
  _lcd->setCursor(0,0);
  printString(MENU);
  _lcd->setCursor(0,1);
  printString(SND,LEFT,RIGHT);
}

void AlarmController::printMenuH() {
  _lcd->setCursor(0,0);
  printString(SND);
  _lcd->setCursor(0,1);
  printString(MEL,SPACE2,RIGHT);
}

void AlarmController::printMenuS() {
  _lcd->setCursor(0,0);
  printString(SND);
  _lcd->setCursor(0,1);
  printString(AL_VOL,LEFT,RIGHT);
}

void AlarmController::printSetSound() {
  if(_init) {
    _lcd->setCursor(0,0);
    printString(MEL);
    _cursor = 7;
    _tvar = _settings->getSoundNo();
    _init = false;
    _changed = true;
    _tactive = false;
  }
  if(_changed) {
    _lcd->setCursor(0,1);
    if(_tvar == 0) {
      printString(BUZZER);
    }
    else {
      _lcd->print(SPACE);
      _lcd->print(SPACE);
      _lcd->print(F("MP3:"));
      if(_tvar < 10) {
        _lcd->print(SPACE);
      }
      _lcd->print(_tvar);
      _lcd->print(SPACE);
      _lcd->print(SPACE);
      _lcd->print(SPACE);
      if(_tactive) {
        _lcd->write(SPEAKERON);
      }
      else {
        _lcd->write(SPEAKEROFF);
      }
      _lcd->print(SPACE);
      _lcd->write(NEXTSONG);
      _lcd->print(SPACE);
      _lcd->print(SPACE);
    }
    _changed = false;
  }
  _lcd->setCursor(_cursor,1);
  if(_settings->getTime()->getMillis() % 1000 > 500) {
    _lcd->noCursor();
  } else {
    _lcd->cursor();
  }
}

void AlarmController::printSetAlarmVolume() {
  if(_init) {
    _lcd->setCursor(0,0);
    printString(AL_VOL);
    _cursor = 5;
    _changed = true;
    _tactive = false;
    _tvar = _settings->getAlarmVolume();
    _init = false;
  }
  if(_changed) {
    _lcd->setCursor(0,1);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    if(_tvar<10)
      _lcd->print(SPACE);
    _lcd->print(_tvar);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    if(_tactive) {
      _lcd->write(SPEAKERON);
    }
    else {
      _lcd->write(SPEAKEROFF);
    }
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _changed = false;
  }
  if(_settings->getTime()->getMillis() % 1000 > 500) {
    _lcd->setCursor(_cursor,1);
    _lcd->noCursor();
  }
  else {
    _lcd->setCursor(_cursor,1);
    _lcd->cursor();
  }
}

void AlarmController::printSetSnoozeVolume() {
  if(_init) {
    _lcd->setCursor(0,0);
    printString(SN_VOL);
    _cursor = 5;
    _changed = true;
    _tactive = false;
    _tvar = _settings->getSnoozeVolume();
    _init = false;
  }
  if(_changed) {
    _lcd->setCursor(0,1);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    if(_tvar==0) {
      _lcd->print(OFF);
    } else {
      if(_tvar<10) {
        _lcd->print(SPACE);
      }
      _lcd->print(_tvar);
      _lcd->print(SPACE);
    }
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    if(_tactive) {
      _lcd->write(SPEAKERON);
    }
    else {
      _lcd->write(SPEAKEROFF);
    }
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _changed = false;
  }
  if(_settings->getTime()->getMillis() % 1000 > 500) {
    _lcd->setCursor(_cursor,1);
    _lcd->noCursor();
  }
  else {
    _lcd->setCursor(_cursor,1);
    _lcd->cursor();
  }
}

void AlarmController::printSetSnoozeAdvert() {
  if(_init) {
    _lcd->setCursor(0,0);
    printString(SN_ADV);
    _cursor = 8;
    _changed = true;
    _tactive = false;
    _tvar = _settings->getSnoozeAdvert();
    _init = false;
  }
  if(_changed) {  // |xxxxxx__5       |
    _lcd->setCursor(0,1);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    if(_tvar==0) {
      _lcd->print(OFF);
    } else {
      _lcd->print(SPACE);
      if(_tvar<10) {
        _lcd->print(SPACE);
      }
      _lcd->print(_tvar);
    }
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _lcd->print(SPACE);
    _changed = false;
  }
  if(_settings->getTime()->getMillis() % 1000 > 500) {
    _lcd->setCursor(_cursor,1);
    _lcd->noCursor();
  }
  else {
    _lcd->setCursor(_cursor,1);
    _lcd->cursor();
  }
}

void AlarmController::setMakeLight(void (*makeLight)(uint8_t,int)) {
  _makeLight = makeLight;
}

void AlarmController::checkAlarmState() {
  uint8_t a = _settings->getFirstActiveAlarmNo();
  int am = _settings->getAlarmHour(a) * 60 + _settings->getAlarmMin(a);
  int tm = _settings->getTime()->getHour() * 60 + _settings->getTime()->getMin();
  _playBuzzer = false;
  if(_alarmState == 0) { // normal
    if( ( a == 7 || a == _settings->getTime()->getDay() ) && 
        ( abs(am - tm) <= _settings->getWakeUpDur() ) &&
        ( _settings->getTime()->getSec() < 2 ) ) {
      _alarmState++;
      _activeAlarm = a;
    }
  } 
  else if(_alarmState == 1) { // waking up
    if(_activeAlarm == a) {
      //increase light
      _makeLight(_ledPin,map(tm,am-_settings->getWakeUpDur(),am,0,_settings->getWakeUpLum()));
      //time to wake up?
      if(a < 8) {
        if(tm >= am) {
           _alarmState++;
           _nextAlarmMin = am + _settings->getSnoozeDur();
           _snoozeCnt = 0;
        }
      }
    }
    else {
      _alarmState--;
    }
  } 
  else if(_alarmState == 2) { // wake up
    //make some noise
    _makeLight(_ledPin,_settings->getWakeUpLum());
    if(_settings->getSoundNo() == 0) {
      _playBuzzer = true;
    } else {
      //play mp3
      _mp3->setVolume(_settings->getAlarmVolume());
      _mp3->play(_settings->getSoundNo());

    }
    _lcdState = 'O';
  } 
  else if(_alarmState == 3) { // snoozing
    _makeLight(_ledPin,_settings->getWakeUpLum());
    if(_settings->getSoundNo() > 0) {
      //MP3 is selected as alarm (not the buzzer)
      if(_settings->mustPlayWhenSnoozing()) {
        _mp3->setVolume(_settings->getSnoozeVolume());
      } else {
        _mp3->pause();
      }
    }
    if(tm >= _nextAlarmMin) {
      _alarmState--;
      _nextAlarmMin += _settings->getSnoozeDur();
      _snoozeCnt++;
      if(_settings->getSnoozeAdvert() > 0 && _snoozeCnt > 0) {
        if(_settings->getSnoozeAdvert() >= _snoozeCnt) {
          //there are "advertisements" left over!
          _mp3->setVolume(_settings->getAlarmVolume());
          _mp3->advertise(_snoozeCnt);
        } else {
          //max number of snoozes for the "advertisements"
          _mp3->setVolume(_settings->getAlarmVolume());
          _mp3->advertise(_settings->getSnoozeAdvert());
        }
      }
    }
  }
  //make noise?
  controlBuzzer();
}

void AlarmController::controlBacklight() {
  if(_lcdState == 'M') {
    if(_buttonPressedSec >= 0) {
      uint8_t delta;
      if(_buttonPressedSec > _settings->getTime()->getSec()) { //overflow
        delta = _settings->getTime()->getSec() + 60 - _buttonPressedSec;
      }
      else {
        delta = _settings->getTime()->getSec() - _buttonPressedSec;
      }
      if(delta <= _settings->getBacklightDur()) {
        _lcd->setBacklight(1);
      }
      else {
        _lcd->setBacklight(0);
        _buttonPressedSec = -1;
      }
    }
  }
  else {
    _lcd->setBacklight(1);
  }
}

void AlarmController::controlBuzzer() {
  if(_playBuzzer) {
    if(buzzerPI % 2 == 0) {
      digitalWrite(_buzzerPin, HIGH);
    }
    else {
      digitalWrite(_buzzerPin, LOW);
    }
    if(abs(millis() - buzzerStart) > buzzerP[buzzerPI]) {
      buzzerPI++;
      if(buzzerPI == buzzerPL)
        buzzerPI = buzzerL;
      buzzerStart = millis();
    } 
  }
  else {
    buzzerPI = 0;
    digitalWrite(_buzzerPin, LOW);
  }
}


uint8_t AlarmController::getAlarmState() {
  return _alarmState;
}
