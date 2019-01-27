/*************************
 ** MP3Player.h
 ************************/  

#ifndef MP3PLAYER_H
#define MP3PLAYER_H

#include "Arduino.h"
#include <DFRobotDFPlayerMini.h>  // https://github.com/DFRobot/DFRobotDFPlayerMini
#include <SoftwareSerial.h>

class MP3Player {
  public:
    MP3Player(SoftwareSerial* swSerial, DFRobotDFPlayerMini* mp3Player);
    void initialize();
    void play(uint8_t);
    void pause();
    void stop();
    void next();
    void setVolume(uint8_t);
    uint8_t getVolume();
    void advertise(uint8_t);
  private:
    uint8_t _volume;
    uint8_t _status;   //0 = stopped, 1 = paused, 2 = playing
    uint8_t _currentTrack;
    uint8_t _currentAdvert;
    SoftwareSerial* _swSerial;
    DFRobotDFPlayerMini* _mp3Player;
};


#endif 
