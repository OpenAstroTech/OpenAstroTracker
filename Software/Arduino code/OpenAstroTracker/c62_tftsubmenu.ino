#ifdef TFTscreen

void drawmenuHome() {
  coords_btn.drawButton(false);
  go_btn.drawButton(false);
  ctrl_btn.drawButton(false);
  sett_btn.drawButton(false);
}

void drawmenu1() {
  return_btn.drawButton(false);
  RA_btn.drawButton(true);
  DEC_btn.drawButton(false);
  RAhour_btn.drawButton(false);
  RAmin_btn.drawButton(false);
  RAsec_btn.drawButton(false);
  conformRAcoord_btn.drawButton(false);  
  tft.setTextColor(WHITE);
  tft.setCursor(47,72);
  tft.print("h");
  tft.setCursor(102,72);
  tft.print("m");
  tft.setCursor(157,72);
  tft.print("s");
  tft.setTextSize(1);
  tft.setCursor(10,50);
  tft.print("Set RA coordinates:");
  tft.setTextSize(2);
  tft.setCursor(55, 5);
  tft.print("target RA:  ");
  tft.print(mount.RAString(COMPACT_STRING | CURRENT_STRING).substring(0, 2) + "h");
  tft.print(mount.RAString(COMPACT_STRING | CURRENT_STRING).substring(2, 4) + "m");
  tft.print(mount.RAString(COMPACT_STRING | CURRENT_STRING).substring(4, 6) + "s");
  tft.setCursor(55, 30);
  tft.print("current DEC:");
  tft.print(mount.DECString(COMPACT_STRING | CURRENT_STRING).substring(0, 1));
  tft.print(mount.DECString(COMPACT_STRING | CURRENT_STRING).substring(1, 3) + "d");
  tft.print(mount.DECString(COMPACT_STRING | CURRENT_STRING).substring(3, 5) + "m");
  tft.print(mount.DECString(COMPACT_STRING | CURRENT_STRING).substring(5, 7) + "s");
  

  
}

void drawmenu2() {
  return_btn.drawButton(false);

}

void drawmenu3() {
  return_btn.drawButton(false);
  up_btn.drawButton(false);
  down_btn.drawButton(false);
  left_btn.drawButton(false);
  right_btn.drawButton(false);
  sethome_btn.drawButton(false);
  slewhome_btn.drawButton(false);
}




void handleHomeButtons() {
  if (coords_btn.justPressed() && menuchange == 0) {
    coords_btn.drawButton(true);
    tft.fillScreen(background);
    menuchange = 1;
    TFTmenu = 1;
  }
  if (go_btn.justPressed() && menuchange == 0) {
    go_btn.drawButton(true);
    tft.fillScreen(background);
    menuchange = 1;
    TFTmenu = 2;
  }
  if (ctrl_btn.justPressed() && menuchange == 0) {
    ctrl_btn.drawButton(true);
    tft.fillScreen(background);
    menuchange = 1;
    TFTmenu = 3;
  }
}

void handleMenu1buttons() {
  if (RAhour_btn.justPressed()) {
    RAhour_btn.drawButton(true);
  }
}
void handleMenu2buttons() {

}

void handleMenu3buttons() {

  if (return_btn.justPressed() && menuchange == 0) {
    tft.fillScreen(background);
    menuchange = 1;
    TFTmenu = 0;
  }

  if (up_btn.justPressed()) {
    mount.delay(150);
    btnpress += 1;
    if (btnpress > 1) {
      btnpress = 0;
    }
    if (btnpress == 0) {
      up_btn.drawButton(true);
      processDirButton(cmdUP);
    }
    if (btnpress == 1) {
      up_btn.drawButton(false);
      processDirButton(cmdSTOP);
    }
  }
  if (down_btn.justPressed()) {
    mount.delay(150);
    btnpress += 1;
    if (btnpress > 1) {
      btnpress = 0;
    }
    if (btnpress == 0) {
      down_btn.drawButton(true);
      processDirButton(cmdDOWN);
    }
    if (btnpress == 1) {
      down_btn.drawButton(false);
      processDirButton(cmdSTOP);
    }
  }

  if (left_btn.justPressed()) {
    mount.delay(150);
    btnpress += 1;
    if (btnpress > 1) {
      btnpress = 0;
    }
    if (btnpress == 0) {
      left_btn.drawButton(true);
      processDirButton(cmdLEFT);
    }
    if (btnpress == 1) {
      left_btn.drawButton(false);
      processDirButton(cmdSTOP);
    }
  }

  if (right_btn.justPressed()) {
    mount.delay(150);
    btnpress += 1;
    if (btnpress > 1) {
      btnpress = 0;
    }
    if (btnpress == 0) {
      right_btn.drawButton(true);
      left_btn.drawButton(false);
      processDirButton(cmdRIGHT);
    }
    if (btnpress == 1) {
      right_btn.drawButton(false);
      processDirButton(cmdSTOP);
    }
  }

  if (sethome_btn.justPressed()) {
    sethome_btn.drawButton(true);
    drawConfirmBox("Set Home Point?");
    fnct = sethome;           //what should the confirm button do?
    lastmenu = 3;

  }
  if (sethome_btn.justReleased()) {
    //sethome_btn.drawButton(false);
  }
  if (slewhome_btn.justPressed()) {
    slewhome_btn.drawButton(true);
    drawConfirmBox("Go Home now?");
    fnct = slewhome;           //what should the confirm button do?
    lastmenu = 3;
  }
}






void drawNumpad() {
  one_btn.drawButton(false);
  two_btn.drawButton(false);
  three_btn.drawButton(false);
  four_btn.drawButton(false);
  five_btn.drawButton(false);
  six_btn.drawButton(false);
  seven_btn.drawButton(false);
  eight_btn.drawButton(false);
  nine_btn.drawButton(false);
  zero_btn.drawButton(false);
  enter_btn.drawButton(false);
  erase_btn.drawButton(false);
  slew_btn.drawButton(false);
  tft.setTextSize(3);
  tft.setCursor(282,115);
  tft.write("S");
  tft.setCursor(282,145);
  tft.write("L");
  tft.setCursor(282,175);
  tft.write("E");
  tft.setCursor(282,205);
  tft.write("W");
}


void handleConfirmButtons() {
  if (confirm_btn.justPressed()) {
    confirm_btn.drawButton(true);
    menuchange = 1;
    TFTmenu = lastmenu;
    overlayactive = false;
    if (fnct == sethome) {
      mount.setHome();
      mount.startSlewing(TRACKING);
    }
    if (fnct == slewhome) {
      mount.setTargetToHome();
      mount.startSlewingToTarget();
    }

  }
  if (decline_btn.justPressed()) {
    decline_btn.drawButton(true);
    menuchange = 1;
    TFTmenu = lastmenu;
    overlayactive = false;
  }
}

void drawConfirmBox(String text) { //, int color, int textsize) {
  overlayactive = true;
  tft.fillRoundRect(60, 60, 200, 140, 10, BLACK);
  tft.drawRoundRect(60, 60, 200, 140, 10, WHITE);
  int textposx = 160 - text.length() * 6; // 6 pixel per letter
  tft.setCursor(textposx , 70);
  tft.print(text);
  confirm_btn.drawButton(false);
  decline_btn.drawButton(false);
}

void togglebuttons(int btn) {
  if (btn == upbtn) {
    if (btnpress == 0) {
      up_btn.drawButton(true);
      processDirButton(cmdUP);

    }
    if (btnpress == 1) {
      up_btn.drawButton(false);
      processDirButton(cmdSTOP);
    }
  }
}

bool processDirButton(int cmd) {
  if (cmd == cmdUP) {
    mount.startSlewing(NORTH);
  }
  else  if (cmd == cmdDOWN) {
    mount.startSlewing(SOUTH);
  }
  else  if (cmd == cmdRIGHT) {
    mount.startSlewing(WEST);
  }
  else  if (cmd == cmdLEFT) {
    mount.startSlewing(EAST);
  }
  else if (cmd == cmdSTOP)
  {
    mount.stopSlewing(ALL_DIRECTIONS);
  }
}

#endif
