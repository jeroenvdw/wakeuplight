/*************************
 ** MP3Player.cpp
 ************************/  
 
#include "MP3Player.h"
 
MP3Player::MP3Player(SoftwareSerial* swSerial, DFRobotDFPlayerMini* mp3Player) {
  _swSerial = swSerial;
  _mp3Player = mp3Player;
  _volume = 0;
  _currentTrack = 0;
  _currentAdvert = 0;
  _status = 0;  //0 = stopped, 1 = paused, 2 = playing
}

void MP3Player::initialize() {
  _swSerial->begin(9600);
  _mp3Player->begin(*_swSerial);
  delay(300); //give some time to MP3 player
  _mp3Player->volume(_volume);
  delay(300); //give some time to MP3 player
}

void MP3Player::play(uint8_t n) {
  if(n <= 0) {
    n = 1;
  }
  if(_currentTrack != n) {
    //different track
    _mp3Player->loopFolder(n);
    _currentTrack = n;
  } else {
    //same track
    if(_status == 1) { //paused
      _mp3Player->start();
    } else if (_status == 0) { //stopped
      _mp3Player->loopFolder(n);
    }
  }
  _status = 2; //playing
}


void MP3Player::next() {
  _mp3Player->next();
  _status = 2; //playing
}

void MP3Player::pause() {
  if(_status == 2) { //playing (only from status playing, pause is allowed)
    _mp3Player->pause();
    _status = 1; //paused
  }
}

void MP3Player::stop() {
  if(_status != 0) { //not stopped (from status paused or playing: stop is allowed)
    _mp3Player->pause();
    _status = 0; //stopped
  }
}

void MP3Player::setVolume(uint8_t v) {
  if(_volume != v) {
    _mp3Player->volume(v);
    _volume = v;
  }
}

uint8_t MP3Player::getVolume() {
  return _volume;
}

void MP3Player::advertise(uint8_t a) {
  if(a <= 0) {
    a = 1;
  }
  if(a == _currentAdvert) {
    _mp3Player->stopAdvertise();
  }
  _mp3Player->advertise(a);
  _currentAdvert = a;
}
