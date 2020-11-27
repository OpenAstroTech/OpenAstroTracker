#pragma once

#if DISPLAY_TYPE > 0
#if SUPPORT_MANUAL_CONTROL == 1

bool setZeroPoint = true;

#define HIGHLIGHT_MANUAL 1
#define HIGHLIGHT_SERIAL 2

#define MANUAL_CONTROL_MODE 10
#define MANUAL_CONTROL_CONFIRM_HOME 11
#define SERIAL_DISPLAY_MODE 20

int ctrlState = HIGHLIGHT_MANUAL;

#define LOOPS_TO_CONFIRM_KEY 10
byte loopsWithKeyPressed = 0;
byte keyPressed = btnNONE;

void setControlMode(bool state)
{
  ctrlState = state ? MANUAL_CONTROL_MODE : HIGHLIGHT_MANUAL;
}

bool processKeyStateChanges(int key, int dir)
{
  bool ret = false;
  if (keyPressed != key)
  {
    loopsWithKeyPressed = 0;
    keyPressed = key;
  }
  else
  {
    if (loopsWithKeyPressed == LOOPS_TO_CONFIRM_KEY)
    {
      mount.stopSlewing(ALL_DIRECTIONS);
      mount.waitUntilStopped(ALL_DIRECTIONS);
      if (dir != 0)
      {
        mount.startSlewing(dir);
      }
      loopsWithKeyPressed++;
      ret = true;
    }
    else if (loopsWithKeyPressed < LOOPS_TO_CONFIRM_KEY)
    {
      loopsWithKeyPressed++;
    }
  }

  return ret;
}

bool processControlKeys()
{
  byte key;
  bool waitForRelease = false;

  // User must use SELECT to enter manual control.
  switch (ctrlState)
  {
  case HIGHLIGHT_MANUAL:
  {
    if (lcdButtons.keyChanged(&key))
    {
      waitForRelease = true;
      if (key == btnSELECT)
      {
        ctrlState = MANUAL_CONTROL_MODE;
        mount.stopSlewing(ALL_DIRECTIONS);
      }
      else if ((key == btnDOWN) || (key == btnUP))
      {
        ctrlState = HIGHLIGHT_SERIAL;
      }
      else if (key == btnRIGHT)
      {
        lcdMenu.setNextActive();
      }
    }
  }
  break;

  case HIGHLIGHT_SERIAL:
  {
    if (lcdButtons.keyChanged(&key))
    {
      waitForRelease = true;
      if (key == btnSELECT)
      {
        inSerialControl = !inSerialControl;
      }
      else if ((key == btnDOWN) || (key == btnUP))
      {
        ctrlState = HIGHLIGHT_MANUAL;
      }
      else if (key == btnRIGHT)
      {
        inSerialControl = false;
        lcdMenu.setNextActive();
      }
    }
  }
  break;

  case MANUAL_CONTROL_CONFIRM_HOME:
  {
    if (lcdButtons.keyChanged(&key))
    {
      waitForRelease = true;
      if (key == btnSELECT)
      {
        if (setZeroPoint)
        {
          // Leaving Control Menu, so set stepper motor positions to zero.
          LOGV1(DEBUG_GENERAL, F("CTRL menu: Calling setHome(true)!"));
          mount.setHome(true);
          LOGV3(DEBUG_GENERAL, F("CTRL menu: setHome(true) returned: RA Current %s, Target: %f"), mount.RAString(CURRENT_STRING | COMPACT_STRING).c_str(), mount.RAString(TARGET_STRING | COMPACT_STRING).c_str());
          mount.startSlewing(TRACKING);
        }

#if SUPPORT_GUIDED_STARTUP == 1
        if (startupState == StartupWaitForPoleCompletion)
        {
          startupState = StartupPoleConfirmed;
          inStartup = true;
        }
        else
#endif
        {
          lcdMenu.setNextActive();
        }

        ctrlState = HIGHLIGHT_MANUAL;
        okToUpdateMenu = true;
        setZeroPoint = true;
      }
      else if (key == btnLEFT)
      {
        setZeroPoint = !setZeroPoint;
      }
    }
  }
  break;

  case MANUAL_CONTROL_MODE:
  {
    key = lcdButtons.currentState();
    switch (key)
    {
    case btnUP:
    {
      processKeyStateChanges(btnUP, NORTH);
    }
    break;

    case btnDOWN:
    {
      processKeyStateChanges(btnDOWN, SOUTH);
    }
    break;

    case btnLEFT:
    {
      processKeyStateChanges(btnLEFT, WEST);
    }
    break;

    case btnRIGHT:
    {
      processKeyStateChanges(btnRIGHT, EAST);
    }
    break;

    case btnSELECT:
    {
      if (processKeyStateChanges(btnSELECT, 0))
      {
#if SUPPORT_GUIDED_STARTUP == 1
        if (startupState == StartupWaitForPoleCompletion)
        {
          startupState = StartupPoleConfirmed;
          ctrlState = HIGHLIGHT_MANUAL;
          waitForRelease = true;
          inStartup = true;
        }
        else
#endif
        {
          okToUpdateMenu = false;
          lcdMenu.setCursor(0, 0);
          lcdMenu.printMenu("Set home pos?");
          ctrlState = MANUAL_CONTROL_CONFIRM_HOME;
          waitForRelease = true;
        }
      }
    }
    break;

    case btnNONE:
    {
      processKeyStateChanges(btnNONE, 0);
    }
    break;
    }
  }
  break;
  }

  return waitForRelease;
}

void printControlSubmenu()
{
  switch (ctrlState)
  {
  case HIGHLIGHT_MANUAL:
  {
    lcdMenu.printMenu(">Manual slewing");
  }
  break;
  case HIGHLIGHT_SERIAL:
  {
    lcdMenu.printMenu(">Serial display");
  }
  break;
  case MANUAL_CONTROL_CONFIRM_HOME:
  {
    String disp = " Yes  No  ";
    disp.setCharAt(setZeroPoint ? 0 : 5, '>');
    disp.setCharAt(setZeroPoint ? 4 : 8, '<');
    lcdMenu.printMenu(disp);
  }
  break;
  default:
  {
    mount.displayStepperPositionThrottled();
  }
  break;
  }
}

#endif
#endif
