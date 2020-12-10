#pragma once

#include "b_setup.hpp"

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
void serialEvent()
{
    processSerialData();
}
#endif

// ESP needs to call this in a loop :_(
void processSerialData()
{
    char buffer[2];
    while (Serial.available() > 0)
    {
        if (Serial.readBytes(buffer, 1) == 1)
        {
            if (buffer[0] == 0x06)
            {
                LOGV1(DEBUG_SERIAL, F("Serial: Received: ACK request, replying 1"));
                Serial.print('1');
            }
            else
            {
                String inCmd = String(buffer[0]) + Serial.readStringUntil('#');
                LOGV3(DEBUG_SERIAL, F("Serial: ReceivedCommand(%d): [%s]"), inCmd.length(), inCmd.c_str());

                String retVal = MeadeCommandProcessor::instance()->processCommand(inCmd);
                if (retVal != "")
                {
                    LOGV2(DEBUG_SERIAL, F("Serial: RepliedWith:  [%s]"), retVal.c_str());
                    Serial.print(retVal);
                }
                else
                {
                    LOGV1(DEBUG_SERIAL, F("Serial: NoReply"));
                }
            }
        }

        mount.loop();
    }
}

#endif
