#ifndef HEADLESS_CLIENT
#ifdef SUPPORT_GUIDED_STARTUP
//////////////////////////////////////////////////////////////
// This file contains the Starup 'wizard' that guides you through initial setup

#define StartupIsPointedAtPole 1
#define StartupSetHATime 4
#define StartupWaitForHACompletion 6
#define StartupHAConfirmed 7
#define StartupWaitForPoleCompletion 9
#define StartupPoleConfirmed 10
#define StartupCompleted 20

#define YES 1
#define NO 2
#define CANCEL 3

int startupState = StartupIsPointedAtPole;
int isAtPole = NO;

void startupIsCompleted() {
  startupState = StartupCompleted;
  inStartup = false;

  // Start on the RA menu
  lcdMenu.setActive(RA_Menu);
  lcdMenu.updateDisplay();
}

bool processStartupKeys() {
  byte key;
  bool waitForRelease = true;
  switch (startupState )
  {
    case StartupIsPointedAtPole:
      {
        if (lcdButtons.keyChanged(key))
        {
          if (key == btnLEFT)    {
            isAtPole = adjustWrap(isAtPole, 1, YES, CANCEL);
          }
          else if (key == btnSELECT)    {
            if (isAtPole == YES) {
              startupState = StartupSetHATime;
            }
            else if (isAtPole == NO) {
              startupState = StartupWaitForPoleCompletion;
              inStartup = false;
              lcdMenu.setCursor(0, 0);
              lcdMenu.printMenu("Use ^~<> to home");
              lcdMenu.setActive(Control_Menu);

              // Skip the 'Manual control' prompt
              inControlMode = true;
            }
            else if (isAtPole == CANCEL) {
              startupIsCompleted();
            }
          }
        }
      }
      break;

    case StartupSetHATime :
      {
        inStartup = false;

        // Jump to the HA menu
        lcdMenu.setCursor(0, 0);
        lcdMenu.printMenu("Set current HA");
        lcdMenu.setActive(HA_Menu);
        startupState = StartupWaitForHACompletion;
      }
      break;

    case StartupHAConfirmed: {
        startupIsCompleted();
      }
      break;

    case StartupPoleConfirmed: {
        isAtPole = YES;

        // Ask again to confirm
        startupState = StartupIsPointedAtPole;
      }
      break;
  }
  return waitForRelease;
}


void prinStartupMenu() {

  switch (startupState)
  {
    case StartupIsPointedAtPole:
      {
        //              0123456789012345
        String choices(" Yes  No  Cancl ");
        if (isAtPole == YES) {
          choices.setCharAt(0, '>');
          choices.setCharAt(4, '<');
        }
        if (isAtPole == NO) {
          choices.setCharAt(5, '>');
          choices.setCharAt(8, '<');
        }
        if (isAtPole == CANCEL) {
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
