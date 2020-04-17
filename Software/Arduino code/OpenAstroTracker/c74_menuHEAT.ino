#ifndef HEADLESS_CLIENT
#ifdef SUPPORT_HEATING

bool processHeatKeys() {
  byte key;
  switch (key) {
    case btnUP:
    case btnDOWN:
    case btnSELECT: {
        if (heatselect == 0) {
          RAheat = 1 - RAheat ;
        }
        if (heatselect == 1) {
          DECheat = 1 - DECheat ;
        }
      }
      break;

    case btnLEFT: {
        heatselect = 1 - heatselect ;
      }
      break;

    case btnRIGHT: {
        if (RAheat == 0) {
          tracking = 1;
          digitalWrite(motorPin1, LOW);
          digitalWrite(motorPin2, LOW);
          digitalWrite(motorPin3, LOW);
          digitalWrite(motorPin4, LOW);
          stepperRA.disableOutputs();
        }
        else {
          tracking = 0;
          stepperRA.enableOutputs();
          digitalWrite(motorPin1, HIGH);
          digitalWrite(motorPin2, HIGH);
          digitalWrite(motorPin3, HIGH);
          digitalWrite(motorPin4, HIGH);
        }
        if (DECheat == 0) {
          digitalWrite(motorPin11, LOW);
          digitalWrite(motorPin12, LOW);
          digitalWrite(motorPin13, LOW);
          digitalWrite(motorPin14, LOW);
          stepperDEC.disableOutputs();
        }
        else {
          stepperDEC.enableOutputs();
          digitalWrite(motorPin11, HIGH);
          digitalWrite(motorPin12, HIGH);
          digitalWrite(motorPin13, HIGH);
          digitalWrite(motorPin14, HIGH);
        }
        lcdMenu.setNextActive();
      }
      break;
  }
  return true;
}

void printHeatSubmenu() {
  String menu = format("RA%c%s%c DEC%c%s%c  ",
                       heatselect == 0 ? ">" : ":",
                       RAheat == 0 ? "Off" : "On ",
                       heatselect == 0 ? "<" : " ",
                       heatselect == 1 ? ">" : ":",
                       DECheat == 0 ? "Off" : "On ",
                       heatselect == 1 ? "<" : " "
                      );
  lcdMenu.printMenu(menu);
}

#endif
#endif
