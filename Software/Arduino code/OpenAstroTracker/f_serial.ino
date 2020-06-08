#ifdef SUPPORT_SERIAL_CONTROL
#include "MeadeCommandProcessor.h"

////////////////////////////////////////////////
// The main loop when under serial control
void serialLoop()
{
    mount.loop();
    mount.displayStepperPositionThrottled();

#ifdef ESPBOARD
    processSerialData();
#endif

#ifdef WIFI_ENABLED
    wifiControl.loop();
#endif
}

//////////////////////////////////////////////////
// Event that is triggered when the serial port receives data.
#ifndef ESPBOARD
void serialEvent() {
    processSerialData();
}
#endif

// ESP8266 needs to call this in a loop :_(
void processSerialData() {
    while (Serial.available() > 0) {

        String inCmd = Serial.readStringUntil('#');

        auto retVal = MeadeCommandProcessor::instance()->processCommand(inCmd);
        if (retVal != "") {
            Serial.print(retVal);
        }
        mount.loop();
    }
}

#endif
