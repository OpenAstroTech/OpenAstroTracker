
void TFTsetup() {


#ifdef TFTscreen
#define cmdUP 1
#define cmdDOWN 2
#define cmdRIGHT 3
#define cmdLEFT 4
#define cmdSTOP 5
#define sethome 10
#define slewhome 11
#define upbtn 20
#define downbtn 21


  uint16_t ID = tft.readID();
  tft.begin(ID);
  tft.width(); //int16_t width(void);
  tft.height(); //int16_t height(void);
  tft.setRotation(1); //Portrait = 0, Landscape = 90
  tft.fillScreen(background);

  tft.setCursor(5, 100);
  tft.setTextSize(4);
  tft.print("OpenAstroTech");
  tft.setCursor(100, 140);
  tft.setTextSize(3);
  tft.print(version);
  //delay(800);
  tft.fillScreen(background);
  //                             X   Y
  coords_btn.initButtonUL(&tft,  20, 20, 120, 80, boxborder, boxfill, boxtext, "Coord.", 3);
  go_btn.initButtonUL(&tft,  160, 20, 120, 80, boxborder, boxfill, boxtext, "GoTo", 3);
  ctrl_btn.initButtonUL(&tft,  20, 120, 120, 80, boxborder, boxfill, boxtext, "Ctrl", 3);
  sett_btn.initButtonUL(&tft,  160, 120, 120, 80, boxborder, boxfill, boxtext, "Settings", 2);
  return_btn.initButtonUL(&tft,  5, 5, 35, 35, boxborder, boxfill, boxtext, "back", 1);
  up_btn.initButtonUL(&tft,  100, 65, 50, 50, boxborder, boxfill, boxtext, "UP", 3);
  down_btn.initButtonUL(&tft,  100, 125, 50, 50, boxborder, boxfill, boxtext, "DOWN", 2);
  left_btn.initButtonUL(&tft,  50, 95, 50, 50, boxborder, boxfill, boxtext, "LEFT", 2);
  right_btn.initButtonUL(&tft,  150, 95, 50, 50, boxborder, boxfill, boxtext, "RIGHT", 1);
  sethome_btn.initButtonUL(&tft,  200, 180, 120, 50, boxborder, boxfill, boxtext, "Set home", 2);
  slewhome_btn.initButtonUL(&tft,  200, 10, 120, 50, boxborder, boxfill, boxtext, "Slew Home", 2);
  confirm_btn.initButtonUL(&tft,  60, 150, 100, 50, boxborder, GREEN, boxtext, "Yes", 2);
  decline_btn.initButtonUL(&tft,  160, 150, 100, 50, boxborder, RED, boxtext, "No", 2);


  one_btn.initButtonUL(&tft,  5, tft.height() - 45, 60, 40, boxborder, GREY, WHITE, "1", 3);
  two_btn.initButtonUL(&tft,  5 + 65, tft.height() - 45, 60, 40, boxborder, GREY, WHITE, "2", 3);
  three_btn.initButtonUL(&tft,  5 + 65 + 65, tft.height() - 45, 60, 40, boxborder, GREY, WHITE, "3", 3);
  zero_btn.initButtonUL(&tft,  5 + 65 + 65 + 65, tft.height() - 45, 60, 40, boxborder, GREY, WHITE, "0", 3);
  four_btn.initButtonUL(&tft,  5, tft.height() - 45 - 45, 60, 40, boxborder, GREY, WHITE, "4", 3);
  five_btn.initButtonUL(&tft,  5 + 65, tft.height() - 45 - 45, 60, 40, boxborder, GREY, WHITE, "5", 3);
  six_btn.initButtonUL(&tft,  5 + 65 + 65, tft.height() - 45 - 45, 60, 40, boxborder, GREY, WHITE, "6", 3);
  enter_btn.initButtonUL(&tft,  5 + 65 + 65 + 65, tft.height() - 45 - 45, 60, 40, boxborder, GREYGREEN, WHITE, "+/-", 2);   //hier noch den namen ausbessern
  seven_btn.initButtonUL(&tft,  5, tft.height() - 45 - 45 - 45, 60, 40, boxborder, GREY, WHITE, "7", 3);
  eight_btn.initButtonUL(&tft,  5 + 65, tft.height() - 45 - 45 - 45, 60, 40, boxborder, GREY, WHITE, "8", 3);
  nine_btn.initButtonUL(&tft,  5 + 65 + 65, tft.height() - 45 - 45 - 45, 60, 40, boxborder, GREY, WHITE, "9", 3);
  erase_btn.initButtonUL(&tft,  5 + 65 + 65 + 65, tft.height() - 45 - 45 - 45, 60, 40, boxborder, GREYRED, WHITE, "erase", 1);
  slew_btn.initButtonUL(&tft,  5 + 65 + 65 + 65 + 65, tft.height() - 45 - 45 - 45, 50, 130, boxborder, boxfill, WHITE, "", 1);

  RA_btn.initButtonUL(&tft,  195, 0, 125, 24, boxborder, boxfill, boxtext, "", 2);
  DEC_btn.initButtonUL(&tft,  195, 25, 125, 24, boxborder, boxfill, boxtext, "", 2);
  RAhour_btn.initButtonUL(&tft,  5, 60, 40, 40, boxborder, GREYGREEN, WHITE, tftTargetRA.substring(0,2).c_str(), 2);
  RAmin_btn.initButtonUL(&tft,  60, 60, 40, 40, boxborder, GREYGREEN, WHITE, "10", 2);
  RAsec_btn.initButtonUL(&tft,  115, 60, 40, 40, boxborder, GREYGREEN, WHITE, "04", 2);
  conformRAcoord_btn.initButtonUL(&tft,  170, 60, 40, 40, boxborder, GREY, GREEN, "OK", 2);

  menuchange = 1;















#endif
}
