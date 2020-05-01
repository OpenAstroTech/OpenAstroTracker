#include "WifiControl.hpp"
#ifdef WIFI_ENABLED
#include "ESP8266Wifi.h"


WifiControl::WifiControl(Mount* mount, LcdMenu* lcdMenu) :
    _cmdProcessor(mount, lcdMenu)
{

    _mount = mount;
    _lcdMenu = lcdMenu;

    switch (WIFI_MODE) {
    case 0: // startup Infrastructure Mode
        break;
    case 1: // startup AP mode
        break;
    case 2: // Attempt Infra, fail over to AP
        break;
    }
}


void WifiControl::startInfrastructureMode()
{
    WiFi.begin(INFRA_SSID, INFRA_WPAKEY);
}

void WifiControl::loop()
{
    if (_status != WiFi.status()) {
        _status = WiFi.status();
#ifdef DEBUG_MODE
        Serial.println("Connected status changed to " + _status);
#endif
        if (_status == WL_CONNECTED) {
            _tcpServer = new WiFiServer(4030);
            _tcpServer->begin();
            _tcpServer->setNoDelay(true);
#ifdef DEBUG_MODE            
            Serial.printf("Server status is %d\n", _tcpServer->status());
#endif
            _udp = new WiFiUDP();
            _udp->begin(4031);

#ifdef DEBUG_MODE
            Serial.print("Connecting to SSID ");
            Serial.print(INFRA_SSID);
            Serial.print("  IP: ");
            Serial.println(WiFi.localIP());
#endif
        }
    }
    _mount->loop();

    if (_status != WL_CONNECTED) return;

    tcpLoop();
    udpLoop();
}

void WifiControl::tcpLoop() {
    if (client && client.connected()) {
        while (client.available()) {
            String cmd = client.readStringUntil('#');
#ifdef DEBUG_MODE
            Serial.printf("<--  %s#\n", cmd.c_str());
#endif
            auto retVal = _cmdProcessor.processCommand(cmd);

            if (retVal != "") {
                client.write(retVal.c_str());
#ifdef DEBUG_MODE
                Serial.printf("-->  %s\n", retVal.c_str());
#endif
            }

            _mount->loop();
        }
    }
    else {
        client = _tcpServer->available();
    }

}

void WifiControl::udpLoop()
{
    int packetSize = _udp->parsePacket();
    if (packetSize)
    {
        String lookingFor = "skyfi:";
        lookingFor += HOSTNAME;
        lookingFor += "?";

        String reply = "skyfi:";
        reply += HOSTNAME;
        reply += "@";
        reply += WiFi.localIP().toString();
#ifdef DEBUG_MODE
        Serial.printf("Received %d bytes from %s, port %d\n", packetSize, _udp->remoteIP().toString().c_str(), _udp->remotePort());
#endif
        char incomingPacket[255];
        int len = _udp->read(incomingPacket, 255);
        incomingPacket[lookingFor.length()] = NULL;
#ifdef DEBUG_MODE
        Serial.printf("Received: %s\n", incomingPacket);
#endif
        
        
        if (lookingFor.equalsIgnoreCase(incomingPacket)) {
            _udp->beginPacket(_udp->remoteIP(), 4031);
            /*unsigned char bytes[255];
            reply.getBytes(bytes, 255);
            _udp->write(bytes, reply.length());*/

            _udp->write(reply.c_str());
            _udp->endPacket();
#ifdef DEBUG_MODE
            Serial.printf("Replied: %s\n", reply.c_str());
#endif
        }
    }
}
#endif