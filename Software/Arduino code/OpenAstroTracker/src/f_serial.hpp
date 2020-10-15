#pragma once

#include "b_setup.hpp"

#if SUPPORT_SERIAL_CONTROL == 1
#include "MeadeCommandProcessor.hpp"

void processSerialData();
#ifdef HEADLESS_BLUETOOTH
bool bt_connected = false;
void processSerialBTData();
void bt_callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param);
bool setupCallback = false;
#endif

////////////////////////////////////////////////
// The main loop when under serial control
void serialLoop()
{
    mount.loop();
    mount.displayStepperPositionThrottled();

#ifdef ESPBOARD
    processSerialData();

#ifdef HEADLESS_BLUETOOTH
    if (!setupCallback) {
        SerialBT.register_callback(bt_callback);
        setupCallback = true;
    }
    if (bt_connected) {
        processSerialBTData();
    }
#endif
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
    char buffer[2];
    while (Serial.available() > 0) {
        if (Serial.readBytes(buffer, 1) == 1) {
            if (buffer[0] == 0x06) {
                LOGV1(DEBUG_SERIAL, F("Serial: Received: ACK request, replying"));
                Serial.print('1');
            } else {
                String inCmd = String(buffer[0]) + Serial.readStringUntil('#');
                LOGV2(DEBUG_SERIAL, F("Serial: Received: %s"), inCmd.c_str());

                String retVal = MeadeCommandProcessor::instance()->processCommand(inCmd);
                if (retVal != "") {
                    LOGV2(DEBUG_SERIAL, F("Serial: Replied:  %s"), inCmd.c_str());
                    Serial.print(retVal);
                }
            }
        }
           
        mount.loop();
    }
}

#ifdef HEADLESS_BLUETOOTH 
void processSerialBTData() {
    char buffer[2];
    while (SerialBT.available() > 0) {
        if (SerialBT.readBytes(buffer, 1) == 1) {
            if (buffer[0] == 0x06) {
                LOGV1(DEBUG_SERIAL, F("SerialBT: Received: ACK request, replying"));
                SerialBT.print('1');
            } else {
                String inCmd = String(buffer[0]) + SerialBT.readStringUntil('#');
                LOGV2(DEBUG_SERIAL, F("SerialBT: Received: %s"), inCmd.c_str());
                if (inCmd == "Ka") {
                    SerialBT.print('1');
                    continue;
                }
                if (inCmd == "K*") {
                    SerialBT.print('1');
                    continue;
                }
                if (inCmd.charAt(1) == 0x06) {
                    SerialBT.print('P');
                    continue;
                }
                

                String retVal = MeadeCommandProcessor::instance()->processCommand(inCmd);
                if (retVal != "") {
                    LOGV2(DEBUG_SERIAL, F("SerialBT: Replied:  %s"), retVal);
                    SerialBT.print(retVal);
                } else {
                    LOGV2(DEBUG_SERIAL, F("Didnt reply %s"), retVal);
                }
            }
        } else {
            LOGV1(DEBUG_SERIAL, F("SerialBT: Read nothing"));
        }
           
        mount.loop();
    }
}
void bt_callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param) {
    switch (event) {
        case ESP_SPP_SRV_OPEN_EVT:
            bt_connected = true;
        break;
        case ESP_SPP_CLOSE_EVT:
            bt_connected = false;
            break;
    }
}
#endif

#endif
