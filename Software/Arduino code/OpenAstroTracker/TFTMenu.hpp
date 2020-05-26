/*
#ifndef _TFTMENU_HPP_
#define _TFTMENU_HPP_

#include "Globals.h"
#include <Arduino.h>


#ifdef TFTscreen
#ifndef HEADLESS_CLIENT

#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <TouchScreen.h>

MCUFRIEND_kbv tft;

const int XP = 8, XM = A2, YP = A3, YM = 9; //240x320 ID=0x9595
const int TS_LEFT = 897, TS_RT = 174, TS_TOP = 81, TS_BOT = 903;
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

#define MINPRESSURE 200
#define MAXPRESSURE 1000

#define BLACK 0x0000
#define WHITE 0xFFFF
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define RED     0xF800

int pixel_x, pixel_y;
bool Touch_getXY(void)
{
  TSPoint p = ts.getPoint();
  pinMode(YP, OUTPUT);      //restore shared pins
  pinMode(XM, OUTPUT);
  digitalWrite(YP, HIGH);   //because TFT control pins
  digitalWrite(XM, HIGH);

  bool pressed = (p.z > MINPRESSURE && p.z < MAXPRESSURE);
  if (pressed) {
    //pixel_x = map(p.x, TS_TOP, TS_BOT, 0, tft.height()); // Landscape
    //pixel_y = map(p.y, TS_RT, TS_LEFT, 0, tft.width());
    pixel_x = map(p.x, TS_LEFT, TS_RT, 0, tft.width()); // Portrait
    pixel_y = map(p.y, TS_TOP, TS_BOT, 0, tft.height());
  }
  return pressed;
}



class TftMenu
{
  public:
    TftMenu(byte col, byte row, int zeuch);
    void setTFTcursor();

    void setCursor(byte col, byte row);

  private:
    MCUFRIEND_kbv _tft;
};





























#endif
#endif
#endif
*/
