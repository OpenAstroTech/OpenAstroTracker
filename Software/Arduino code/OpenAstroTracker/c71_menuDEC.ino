bool movingToTarget = false;

void processDECKeys(int key) {

  switch (key) {
    case btnUP: {
        if (DECselect == 0) degreeDEC = north ? adjustClamp(degreeDEC, 1, -180, 0) : adjustClamp(degreeDEC, 1, 0, 180);
        if (DECselect == 1) minDEC = adjustMinute(minDEC, 1);
        if (DECselect == 2) secDEC = adjustSecond(secDEC, 1);
        // slow down key repetitions
        delay(150);
        waitForButtonRelease = false;
      }
      break;

    case btnDOWN: {
        if (DECselect == 0) degreeDEC = north ? adjustClamp(degreeDEC, -1, -180, 0) : adjustClamp(degreeDEC, -1, 0, 180);
        if (DECselect == 1) minDEC = adjustMinute(minDEC, -1);
        if (DECselect == 2) secDEC = adjustSecond(secDEC, -1);
        // slow down key repetitions
        delay(150);
        waitForButtonRelease = false;
      }
      break;

    case btnLEFT: {
        DECselect = adjustWrap(DECselect, 1, 0, 2);
      }
      break;

    case btnSELECT: {
        if (movingToTarget) {
          stopSteppers();
        }
        else {
          startMoveSteppersToTargetAsync();
        }
        movingToTarget = !movingToTarget ;
      }
      break;

    case btnRIGHT: {
        lcdMenu.setNextActive();
      }
      break;
  }

  if (movingToTarget) {
    if (!moveSteppersToTargetAsync()) {
      movingToTarget = false;
    }
  }
}

void printDECSubmenu() {
  if (!movingToTarget) {
    if (DECselect == 0) {
      lcdMenu.printMenuArg(">%d%c %d%c %d%s", printdegDEC, 0, minDEC, 1, secDEC, "\"");
    }
    if (DECselect == 1) {
      lcdMenu.printMenuArg(" %d%c>%d%c %d%s", printdegDEC, 0, minDEC, 1, secDEC, "\"");
    }
    if (DECselect == 2) {
      lcdMenu.printMenuArg(" %d%c %d%c>%d%s", printdegDEC, 0, minDEC, 1, secDEC, "\"");
    }
  }
}
