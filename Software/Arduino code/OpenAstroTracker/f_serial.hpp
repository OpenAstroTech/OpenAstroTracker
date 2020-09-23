#pragma once

// #include "b_setup.hpp"
#include "Mount.hpp"

#if SUPPORT_SERIAL_CONTROL == 1
#include "MeadeCommandProcessor.hpp"

void processSerialData();

bool inSerialControl = false;

////////////////////////////////////////////////
// The main loop when under serial control
void serialLoop()
{
    Mount::instance()->loop();
    Mount::instance()->displayStepperPositionThrottled();

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
void serialEvent()
{
    processSerialData();
}
#endif

// ESP8266 needs to call this in a loop :_(
void processSerialData()
{
    char buffer[2];
    while (Serial.available() > 0)
    {
        if (Serial.readBytes(buffer, 1) == 1)
        {
            if (buffer[0] == 0x06)
            {
                //LOGV1(DEBUG_SERIAL, "Serial: Received: ACK request, replying");
                Serial.print('1');
            }
            else
            {
                String inCmd = String(buffer[0]) + Serial.readStringUntil('#');
                //LOGV2(DEBUG_SERIAL, "Serial: Received: %s", inCmd.c_str());

                String retVal = MeadeCommandProcessor::instance()->processCommand(inCmd);
                if (retVal != "")
                {
                    //LOGV2(DEBUG_SERIAL, "Serial: Replied:  %s", inCmd.c_str());
                    Serial.print(retVal);
                }
            }
        }

        Mount::instance()->loop();
    }
}

#endif
