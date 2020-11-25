#pragma once
#include "../Configuration_adv.hpp"

#if USE_GPS == 1
#include "Sidereal.hpp"
#endif
#if USE_GYRO_LEVEL == 1
#include "Gyro.hpp"
#endif

#if DISPLAY_TYPE > 0
#if SUPPORT_GUIDED_STARTUP == 1

//////////////////////////////////////////////////////////////
// This file contains the Starup 'wizard' that guides you through initial setup

void setControlMode(bool); // In CTRL menu

#define StartupIsInHomePosition 1
#define StartupSetRoll 2
#define StartupWaitForRollCompletion 3
#define StartupRollConfirmed 4
#define StartupSetHATime 10
#define StartupWaitForHACompletion 15
#define StartupHAConfirmed 20
#define StartupWaitForPoleCompletion 25
#define StartupPoleConfirmed 30
#define StartupCompleted 35

#define YES 1
#define NO 2
#define CANCEL 3

int startupState = StartupIsInHomePosition;
int isInHomePosition = NO;

void startupIsCompleted() {
  LOGV1(DEBUG_INFO, F("STARTUP: Completed!"));

  startupState = StartupCompleted;
  inStartup = false;
  okToUpdateMenu = true;

  mount.startSlewing(TRACKING);

  // Start on the RA menu
  lcdMenu.setActive(RA_Menu);
  lcdMenu.updateDisplay();
}

bool processStartupKeys() {
  byte key;
  bool waitForRelease = false;
  switch (startupState) {
    case StartupIsInHomePosition: {
      if (lcdButtons.keyChanged(&key))
      {
        waitForRelease = true;
        if (key == btnLEFT) {
          isInHomePosition = adjustWrap(isInHomePosition, 1, YES, CANCEL);
        }
        else if (key == btnSELECT) {
          if (isInHomePosition == YES) {
            #if USE_GYRO_LEVEL == 1
              startupState = StartupSetRoll;
              LOGV1(DEBUG_INFO, F("STARTUP: State is set roll!"));
            #else
              startupState = StartupSetHATime;
            #endif
          }
          else if (isInHomePosition == NO) {
            #if RA_DRIVER_TYPE == DRIVER_TYPE_TMC2209_UART && USE_AUTOHOME == 1
            mount.startFindingHomeDEC();
            if (mount.isFindingHome()) {
              startupState = StartupWaitForPoleCompletion;            
              lcdMenu.clear();
              lcdMenu.setCursor(0, 0);
              lcdMenu.printMenu("Finding Home....");
              lcdMenu.setCursor(0, 1);
              lcdMenu.printMenu("Please Wait");              
              //break;
              
            }
            else {
              startupState = StartupSetHATime;
            }

            #else
            startupState = StartupWaitForPoleCompletion;
            inStartup = false;
            okToUpdateMenu = false;
            lcdMenu.setCursor(0, 0);
            lcdMenu.printMenu("Home with ^~<>");
            lcdMenu.setActive(Control_Menu);

            // Skip the 'Manual control' prompt
            setControlMode(true);
            #endif
          }
          else if (isInHomePosition == CANCEL) {
            startupIsCompleted();
          }
        }
      }
    }
    break;

    #if USE_GYRO_LEVEL == 1
    case StartupSetRoll : {
      inStartup = false;
      LOGV1(DEBUG_INFO, F("STARTUP: Switching to CAL menu!"));

      lcdMenu.setCursor(0, 0);
      lcdMenu.printMenu("Level front");
      lcdMenu.setActive(Calibration_Menu);

      startupState = StartupWaitForRollCompletion;
    }
    break;
    
    case StartupRollConfirmed : {
      LOGV1(DEBUG_INFO, F("STARTUP: Roll confirmed!"));
      startupState = StartupSetHATime;
    }
    break;
    #endif

    case StartupSetHATime: {
      inStartup = false;
      LOGV1(DEBUG_INFO, F("STARTUP: Switching to HA menu!"));

      #if USE_GPS == 0
        // Jump to the HA menu
        lcdMenu.setCursor(0, 0);
        lcdMenu.printMenu("Set current HA");
        lcdMenu.setActive(HA_Menu);
        startupState = StartupWaitForHACompletion;
      #else
        lcdMenu.setCursor(0, 0);
        lcdMenu.printMenu("Finding GPS...");
        lcdMenu.setActive(HA_Menu);
        startupState = StartupWaitForHACompletion;
      #endif
    }
    break;

    case StartupHAConfirmed: {
      mount.setHome(true);
      DayTime ha(mount.HA());
      mount.setHA(ha);
      mount.targetRA() = mount.currentRA();
      startupIsCompleted();
    }
    break;

    case StartupPoleConfirmed: {
      isInHomePosition = YES;

      // Ask again to confirm
      startupState = StartupIsInHomePosition;
    }
    break;
  }

  return waitForRelease;
}


void printStartupMenu() {
  switch (startupState) {
    case StartupIsInHomePosition: {
      //              0123456789012345
      String choices(" Yes  No  Cancl ");
      if (isInHomePosition == YES) {
        choices.setCharAt(0, '>');
        choices.setCharAt(4, '<');
      }

      if (isInHomePosition == NO) {
        choices.setCharAt(5, '>');
        choices.setCharAt(8, '<');
      }

      if (isInHomePosition == CANCEL) {
        choices.setCharAt(9, '>');
        choices.setCharAt(15, '<');
      }

      lcdMenu.setCursor(0, 0);
      lcdMenu.printMenu("Home position?");
      lcdMenu.setCursor(0, 1);
      lcdMenu.printMenu(choices);
    }
    break;
    
    case StartupPoleConfirmed:
    break;

    case StartupHAConfirmed:
    break;
  }
}
#endif
#endif
