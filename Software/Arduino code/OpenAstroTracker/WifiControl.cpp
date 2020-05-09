#include "WifiControl.hpp"
#ifdef WIFI_ENABLED
#include "ESP8266Wifi.h"


WifiControl::WifiControl(Mount* mount, LcdMenu* lcdMenu) 
{
    _mount = mount;
    _lcdMenu = lcdMenu;
}

void WifiControl::setup() {

#ifdef DEBUG_MODE
    Serial.printf("Starting up Wifi As Mode %d\n", WIFI_MODE);
#endif

  _cmdProcessor = MeadeCommandProcessor::instance();

  switch (WIFI_MODE) {
  case 0: // startup Infrastructure Mode
      startInfrastructureMode();
      break;
  case 1: // startup AP mode
      startAccessPointMode();
      break;
  case 2: // Attempt Infra, fail over to AP
      startInfrastructureMode();
      _infraStart = millis();
      break;
  }
}


void WifiControl::startInfrastructureMode()
{
#ifdef DEBUG_MODE
    Serial.println("Starting Infrastructure Mode Wifi");
#endif
    WiFi.hostname(HOSTNAME);
    WiFi.begin(INFRA_SSID, INFRA_WPAKEY);
}

void WifiControl::startAccessPointMode()
{
#ifdef DEBUG_MODE
    Serial.println("Starting AP Mode Wifi");
#endif
    IPAddress local_ip(192, 168, 1, 1);
    IPAddress gateway(192, 168, 1, 1);
    IPAddress subnet(255, 255, 255, 0);
    
    WiFi.hostname(HOSTNAME);
    WiFi.softAP(HOSTNAME, OAT_WPAKEY);
    WiFi.softAPConfig(local_ip, gateway, subnet);
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

    if (_status != WL_CONNECTED) {
        infraToAPFailover();
        return;
    }

    tcpLoop();
    udpLoop();
}

void WifiControl::infraToAPFailover() {
    if (_infraStart != 0 && 
        !WiFi.isConnected() && 
        _infraStart + _infraWait < millis()) {

        WiFi.disconnect();
        startAccessPointMode();
        _infraStart = 0;

#ifdef DEBUG_MODE
        Serial.println("Could not connect to Infra, Starting AP.");
#endif
    }
}

void WifiControl::tcpLoop() {
    if (client && client.connected()) {
        while (client.available()) {
            String cmd = client.readStringUntil('#');
#ifdef DEBUG_MODE
            Serial.printf("<--  %s#\n", cmd.c_str());
#endif
            auto retVal = _cmdProcessor->processCommand(cmd);

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
        String lookingFor = "skyfi:";;

        String reply = "skyfi:";
        reply += HOSTNAME;
        reply += "@";
        reply += WiFi.localIP().toString();
#ifdef DEBUG_MODE
        Serial.printf("Received %d bytes from %s, port %d\n", packetSize, _udp->remoteIP().toString().c_str(), _udp->remotePort());
#endif
        char incomingPacket[255];
        int len = _udp->read(incomingPacket, 255);
        incomingPacket[len] = 0;
#ifdef DEBUG_MODE
        Serial.printf("Received: %s\n", incomingPacket);
#endif

        incomingPacket[lookingFor.length()] = 0;
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