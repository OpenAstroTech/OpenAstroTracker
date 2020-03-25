void processRAKeys(int key) {
  switch (key) {
    case btnUP: {
        if (RAselect == 0) RATime.addHours(1);
        if (RAselect == 1) RATime.addMinutes(1);
        if (RAselect == 2) RATime.addSeconds(1);
        
        // slow down key repetitions
        delay(150);
        waitForButtonRelease = false;
      }
      break;

    case btnDOWN: {
        if (RAselect == 0) RATime.addHours(-1);
        if (RAselect == 1) RATime.addMinutes(-1);
        if (RAselect == 2) RATime.addSeconds(-1);

        // slow down key repetitions
        delay(150);
        waitForButtonRelease = false;
      }
      break;

    case btnLEFT: {
        RAselect = adjustWrap(RAselect, 1, 0, 2);
      }
      break;

    case btnSELECT: {
        moveSteppersToTarget() ;
      }
      break;

    case btnRIGHT: {
        lcdMenu.setNextActive();
      }
      break;
  }
}

void printRASubmenu(){

    //Serial.println("HA:" + HATime.ToString() + "  HA(corr):" + HACorrection.ToString() + "  RA:" + RATime.ToString() + "  RA(corr):" + RADisplayTime.ToString());
    if (RAselect == 0) {
      lcdMenu.printMenuArg(">%dh %dm %ds", RADisplayTime.getHours(), RADisplayTime.getMinutes(), RADisplayTime.getSeconds());

      //lcd.print(int(diamCorrection));   //for debugging
      //lcd.print("");
      //lcd.print(trackingspeed);
      //lcd.print("   ");
    }
    
    if (RAselect == 1) {
      lcdMenu.printMenuArg(" %dh>%dm %ds", RADisplayTime.getHours(), RADisplayTime.getMinutes(), RADisplayTime.getSeconds());
    }
    
    if (RAselect == 2) {
      lcdMenu.printMenuArg(" %dh %dm>%ds", RADisplayTime.getHours(), RADisplayTime.getMinutes(), RADisplayTime.getSeconds());
    }
  }
