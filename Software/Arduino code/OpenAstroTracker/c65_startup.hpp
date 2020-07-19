#pragma once

#if HEADLESS_CLIENT == 0
#if SUPPORT_GUIDED_STARTUP == 1

#include "Sidereal.hpp"

//////////////////////////////////////////////////////////////
// This file contains the Starup 'wizard' that guides you through initial setup

#define StartupIsInHomePosition 1
#define StartupSetHATime 4
#define StartupWaitForHACompletion 6
#define StartupHAConfirmed 7
#define StartupWaitForPoleCompletion 9
#define StartupPoleConfirmed 10
#define StartupCompleted 20

#define YES 1
#define NO 2
#define CANCEL 3

int startupState = StartupIsInHomePosition;
int isInHomePosition = NO;

void startupIsCompleted() {
  startupState = StartupCompleted;
  inStartup = false;

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
            startupState = StartupSetHATime;
          }
          else if (isInHomePosition == NO) {
            #if RA_Driver_TYPE == 3 && USE_AUTOHOME == 1
            mount.StartFindingHomeDEC();
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
            
            //inStartup = false;
            
              
             
                       
            //lcdMenu.setActive(Control_Menu);

            #else
            startupState = StartupWaitForPoleCompletion;
            inStartup = false;
            lcdMenu.setCursor(0, 0);
            lcdMenu.printMenu("Use ^~<> to home");
            lcdMenu.setActive(Control_Menu);

            // Skip the 'Manual control' prompt
            inControlMode = true;
            #endif
          }
          else if (isInHomePosition == CANCEL) {
            startupIsCompleted();
          }
        }
      }
    }
    break;

    case StartupSetHATime: {
      inStartup = false;
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
      /*while (Serial2.available()) {
        if (gps.location.lng() != 0) {
           ha.set(1);
           //Sidereal::calculateByGPS(&gps).getHours())
           //EPROMStore::Storage()->update(1, mount.HA().set();
          //EPROMStore::Storage()->update(1, mount.HA().getHours());
        }
      }*/
    #endif
    }
    break;

    case StartupHAConfirmed: {
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
