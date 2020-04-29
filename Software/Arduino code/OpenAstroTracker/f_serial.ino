#ifdef SUPPORT_SERIAL_CONTROL
#include "MeadeCommandProcessor.h"

////////////////////////////////////////////////
// The main loop when under serial control
void serialLoop()
{
    mount.loop();
    mount.displayStepperPositionThrottled();
}

//////////////////////////////////////////////////
// Event that is triggered when the serial port receives data.
void serialEvent() {
    auto cp = MeadeCommandProcessor(&mount, &lcdMenu);
    while (Serial.available() > 0) {

        String inCmd = Serial.readStringUntil('#');

        auto retVal = cp.processCommand(inCmd);
        Serial.print(retVal);
        mount.loop();
    }
}

#endif
