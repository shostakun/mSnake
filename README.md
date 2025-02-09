# mSnake

A small snake game for [mBot](https://www.makeblock.com/pages/mbot-robot-kit) IR remote and LED matrix (connected to port 2).

## Installing

**⚠️ WARNING: This will overwrite the mBot firmware. Make sure you know how to get it back before doing any of this! ⚠️**

1. Open the `mSnake.ino` file in the [Arduino IDE](https://www.arduino.cc/en/software).
2. Install the following libraries:
    - [IRLib2](https://github.com/cyborg5/IRLib2)
    - [Adafruit GFX](https://github.com/adafruit/Adafruit-GFX-Library)
    - [TM16xx](https://github.com/maxint-rd/TM16xx)
    - [NewTone](https://bitbucket.org/teckel12/arduino-new-tone/wiki/Home)
3. Click the verify button to make sure everything is installed and the firmware compiles.
4. Connect your mBot with USB.
5. Select the board in the Arduino IDE. The board type is Arduino Uno.
6. Click the upload button. **⚠️ WARNING: This will overwrite the mBot firmware. ⚠️**
7. Play!
    - Press the gear button on the mBot remote to start a new game
    - Press the arrows to turn.

## Skeleton

There is a branch called [skeleton](https://github.com/shostakun/mSnake/tree/skeleton) that has a template version for use as a classroom project.
