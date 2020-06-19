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
#if DEBUG_LEVEL&DEBUG_SERIAL
        logv("Serial: Received: %s", inCmd.c_str());
#endif

        String retVal = MeadeCommandProcessor::instance()->processCommand(inCmd);
        if (retVal != "") {
#if DEBUG_LEVEL&DEBUG_SERIAL
            logv("Serial: Replied:  %s", inCmd.c_str());
#endif
            Serial.print(retVal);
        }
        
        mount.loop();
    }
}

#endif
