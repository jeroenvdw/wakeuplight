# Arduino Wake Up Light
## Features
 - a normal alarm (like any alarm clock)
 - a repeating alarm, which can be configured for every day of the week
 - it can function as a night lamp
 - choose the buzzer or any MP3 to wake up with
 - time is kept with a RTC (real time clock)
 - all settings (max light intensity, wake up duration, ...) can be configured within the menu
 - when power is switched off and on again, the date, time and other settings are restored

## Parts list
 - Arduino Uno
 - RTC (I used the DS3231 for a Raspberry Pi because of the small form factor)
 - LCD; 16x2 characters, with I²C interface
 - Buzzer
 - MP3 module (I used the DFRobot DFPlayer)
 - Speaker 
 - LED strtip (12 volt)
 - NPN Transistor (I used TIP-120)
 - 7 push buttons (for Left, Right, Up, Down, OK, Cancel, Snooze)
 - 9 resistors 1K Ohm (7 pull down resistors, one for the MP3 module, one for the transistor)
 - 12 Volt power supply
 - Wires, Male/Female headers, Perfboard, ... (to solder everything together)
 
 ## The build
 The schematic to solder everything together can be found in "schematic.pdf" (schematic.fzz is the original Fritzing file)
 ![alt text](https://github.com/jeroenvdw/wakeuplight/raw/master/images/schematic.png)
 
 ## The code
 For a better understanding of the lifecycle of the alarm, have a look at "lifecycle.pdf". 
 To undesttand the menu structure, check out "screenflow.pdf" (source files are in the \*.dia files)
 
 All literals in the menu are now in Dutch. Rename the "Literals_EN.h" file to "Literals.h" to build the proejct using English literals.
 
 ## Images
 ![alt text](https://github.com/jeroenvdw/wakeuplight/raw/master/images/wakeuplight0.jpg)
 ![alt text](https://github.com/jeroenvdw/wakeuplight/raw/master/images/wakeuplight1.jpg)
 ![alt text](https://github.com/jeroenvdw/wakeuplight/raw/master/images/wakeuplight2.jpg)
 ![alt text](https://github.com/jeroenvdw/wakeuplight/raw/master/images/wakeuplight3.jpg)
 
