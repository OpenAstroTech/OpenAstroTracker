#pragma once

#ifdef BLUETOOTH_ENABLED
#if SUPPORT_SERIAL_CONTROL == 1
#include "MeadeCommandProcessor.hpp"
#include "BluetoothSerial.h"
BluetoothSerial SerialBT;
#define BLUETOOTH_SERIAL SerialBT

bool bt_connected = false;
void processSerialBTData();
void bt_callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param);
bool setupCallback = false;


void bluetoothLoop() 
{
    if (!setupCallback) {
        BLUETOOTH_SERIAL.register_callback(bt_callback);
        setupCallback = true;
    }
    if (bt_connected) {
        processSerialBTData();
    }
}

void processSerialBTData() {
    char buffer[2];
    while (BLUETOOTH_SERIAL.available() > 0) {
        if (BLUETOOTH_SERIAL.readBytes(buffer, 1) == 1) {
            if (buffer[0] == '#') { 
                // Empty command sent?
            } else if (buffer[0] == 0x06) {
                // ACK from client, requesting Alignment Query, 
                // allows Stellaris (And others?) to tell if this 
                // is a LX200/Meade comaptible mount
                LOGV1(DEBUG_SERIAL, F("SerialBT: Received: ACK request, replying"));
                // Assuming Polar alignment mounting mode
                BLUETOOTH_SERIAL.print('P'); 
            } else {
                String inCmd = String(buffer[0]) + BLUETOOTH_SERIAL.readStringUntil('#');
                LOGV2(DEBUG_SERIAL, F("SerialBT: Received: %s"), inCmd.c_str());
                // Handle NexStar attempt to prevent communication issue
                if (inCmd == "Ka" || inCmd == "K*") {
                    continue;
                }
            
                String retVal = MeadeCommandProcessor::instance()->processCommand(inCmd);
                if (retVal != "") {
                    LOGV2(DEBUG_SERIAL, F("SerialBT: Replied:  %s"), retVal);
                    SerialBT.print(retVal);
                }
            }
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
            ;
        default:
            break;
    }
}

#endif
#endif



void BTin() {
    #ifdef BLUETOOTH_ENABLED
    bluetoothLoop();
    #endif
  /*
    while (BT.available()) {


      inBT = BT.readStringUntil('#');
      //String part = BT.readStringUntil('#');
      //if (inBT == '1') BT.println("test");

      Serial.println(inBT);


      if (inBT == "Ka") BT.write("1"); Serial.write(1); inBT = "";
      if (inBT == "K*") BT.write(1); Serial.write(1); inBT = "";
      if (inBT.length() == 0) BT.println("1"); Serial.print(1); inBT = "";

      if (inBT.indexOf("6") > 0)  BT.println("P"); Serial.print(1);

      if (inBT == ":GR") {
        sprintf(scratchBuffer, "%02d:%02d:%02d", int(hourRAprint), int(minRAprint), int(secRAprint));
        BT.print(scratchBuffer);
        BT.print("#");
        Serial.print(scratchBuffer);
        Serial.print("#");
        //inCmd = "";
      }

      if (inBT == ":GD") {
        sprintf(scratchBuffer, "%02d:%02d:%02d", int(hourRAprint), int(minRAprint), int(secRAprint));
        BT.print(scratchBuffer);
        BT.print("#");
        //inCmd = "";
      }




    }
  */
}

