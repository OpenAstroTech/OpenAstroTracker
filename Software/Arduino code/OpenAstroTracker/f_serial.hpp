#pragma once

#if SUPPORT_SERIAL_CONTROL == 1
#include "MeadeCommandProcessor.hpp"

void processSerialData();

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
        LOGV2(DEBUG_SERIAL, "Serial: Received: %s", inCmd.c_str());

        String retVal = MeadeCommandProcessor::instance()->processCommand(inCmd);
        if (retVal != "") {
            LOGV2(DEBUG_SERIAL,"Serial: Replied:  %s", inCmd.c_str());
            Serial.print(retVal);
        }
        
        mount.loop();
    }
}

#endif
