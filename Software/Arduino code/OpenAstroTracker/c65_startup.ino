#define StartupIsPointedAtPole 1
#define StartupSetHATime 4
#define StartupWaitForHACompletion 6
#define StartupHAConfirmed 7
#define StartupWaitForPoleCompletion 9
#define StartupPoleConfirmed 10
#define StartupCompleted 20

int startupState = StartupIsPointedAtPole;
int isAtPole = 1;


void startupIsCompleted() {
  startupState = StartupCompleted;
  inStartup = false;

  // Start on the RA menu
  lcdMenu.setActive(RA_Menu);
  lcdMenu.setCursor(0, 0);
  lcdMenu.updateDisplay();
}

void processStartupKeys(int key) {
  switch (startupState )
  {
    case StartupIsPointedAtPole:
      {
        if (key == btnLEFT)    {
          isAtPole = adjustWrap(isAtPole, 1, 0, 2);
        }
        else if (key == btnSELECT)    {
          if (isAtPole == 1) { // Yes
            startupState = StartupSetHATime;
          }
          else if (isAtPole == 0) { // No
            startupState = StartupWaitForPoleCompletion;
            inStartup = false;
            lcdMenu.setCursor(0, 0);
            lcdMenu.printMenu("Use ^~<> to pole");
            lcdMenu.setActive(Control_Menu);

            // Skip the 'Manual control' prompt
            inControlMode = true;
          }
          else if (isAtPole == 2) { // Cancel
            startupIsCompleted();
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
        isAtPole = true;

        // Ask again to confirm
        startupState = StartupIsPointedAtPole;
      }
      break;
  }
}


void prinStartupMenu() {
  switch (startupState)
  {
    case StartupIsPointedAtPole:
      {
        //              0123456789012345
        String choices(" Yes  No  Cancl ");
        if (isAtPole == 1) {
          choices.setCharAt(0, '>');
          choices.setCharAt(4, '<');
        }
        if (isAtPole == 0) {
          choices.setCharAt(5, '>');
          choices.setCharAt(8, '<');
        }
        if (isAtPole == 2) {
          choices.setCharAt(9, '>');
          choices.setCharAt(15, '<');
        }
        lcdMenu.setCursor(0, 0);
        lcdMenu.printMenu("Pointed at pole?");
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
