void tftloop() {

#ifdef TFTscreen

  bool down = Touch_getXY();

  if (menuchange == 1) {
    tft.fillScreen(background);
  }

  if (TFTmenu == 0) {               //add buttonpresses here
    coords_btn.press(down && coords_btn.contains(pixel_x, pixel_y));
    go_btn.press(down && go_btn.contains(pixel_x, pixel_y));
    ctrl_btn.press(down && ctrl_btn.contains(pixel_x, pixel_y));
    sett_btn.press(down && sett_btn.contains(pixel_x, pixel_y));
    handleHomeButtons();
  }

  if (TFTmenu == 0 && menuchange == 1) {
    menuchange = 0;
    drawmenuHome();

  }

  if (TFTmenu == 1) {               //add buttonpresses here
    if (!overlayactive) {
      return_btn.press(down && return_btn.contains(pixel_x, pixel_y));
      RAhour_btn.press(down && RAhour_btn.contains(pixel_x, pixel_y));
      RAmin_btn.press(down && RAmin_btn.contains(pixel_x, pixel_y));
      RAsec_btn.press(down && RAsec_btn.contains(pixel_x, pixel_y));
      handleMenu1buttons();
    }
    if (return_btn.justPressed() && menuchange == 0) {
      tft.fillScreen(background);
      menuchange = 1;
      TFTmenu = 0;
    }
  }
  if (TFTmenu == 1 && menuchange == 1) {
    menuchange = 0;
    drawmenu1();
    drawNumpad();
  }


  if (TFTmenu == 2) {               //add buttonpresses here
    return_btn.press(down && return_btn.contains(pixel_x, pixel_y));
    if (return_btn.justPressed() && menuchange == 0) {
      tft.fillScreen(background);
      menuchange = 1;
      TFTmenu = 0;
    }
  }
  if (TFTmenu == 2 && menuchange == 1) {
    menuchange = 0;
    drawmenu2();
  }

  if (TFTmenu == 3) {               //CTRL menu, add buttonpresses here
    if (!overlayactive) {
      return_btn.press(down && return_btn.contains(pixel_x, pixel_y));
      up_btn.press(down && up_btn.contains(pixel_x, pixel_y));
      down_btn.press(down && down_btn.contains(pixel_x, pixel_y));
      left_btn.press(down && left_btn.contains(pixel_x, pixel_y));
      right_btn.press(down && right_btn.contains(pixel_x, pixel_y));
      sethome_btn.press(down && sethome_btn.contains(pixel_x, pixel_y));
      slewhome_btn.press(down && slewhome_btn.contains(pixel_x, pixel_y));
      handleMenu3buttons();
    }
  }
  if (TFTmenu == 3 && menuchange == 1) {
    menuchange = 0;
    drawmenu3();
  }

  if (overlayactive == true) {
    confirm_btn.press(down && confirm_btn.contains(pixel_x, pixel_y));
    decline_btn.press(down && decline_btn.contains(pixel_x, pixel_y));
    handleConfirmButtons();
  }


#endif
}
