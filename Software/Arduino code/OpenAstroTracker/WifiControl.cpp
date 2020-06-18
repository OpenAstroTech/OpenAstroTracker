#include "WifiControl.hpp"
#include "Utility.hpp"

#ifdef WIFI_ENABLED

#define PORT 4030

WifiControl::WifiControl(Mount* mount, LcdMenu* lcdMenu) 
{
    _mount = mount;
    _lcdMenu = lcdMenu;
}

void WifiControl::setup() {

#if DEBUG_LEVEL&DEBUG_WIFI
    logv("Wifi: Starting up Wifi As Mode %d\n", WIFI_MODE);
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
#if DEBUG_LEVEL&DEBUG_WIFI
    logv("Wifi: Starting Infrastructure Mode Wifi");
    logv("Wifi:    with host name: %s", String(HOSTNAME).c_str());
    logv("Wifi:          for SSID: %s", String(INFRA_SSID).c_str());
    logv("Wifi:       and WPA key: %s", String(INFRA_WPAKEY).c_str());
#endif

#if defined(ESP8266)
    WiFi.hostname(HOSTNAME);
#elif defined(ESP32)
    WiFi.setHostname(HOSTNAME);
#endif
    WiFi.begin(INFRA_SSID, INFRA_WPAKEY);
}

void WifiControl::startAccessPointMode()
{
#if DEBUG_LEVEL&DEBUG_WIFI
    logv("Wifi: Starting AP Mode Wifi");
#endif
    IPAddress local_ip(192, 168, 1, 1);
    IPAddress gateway(192, 168, 1, 1);
    IPAddress subnet(255, 255, 255, 0);
    
#if defined(ESP8266)
    WiFi.hostname(HOSTNAME);
#elif defined(ESP32)
    WiFi.setHostname(HOSTNAME);
#endif

    WiFi.softAP(HOSTNAME, OAT_WPAKEY);

    WiFi.softAPConfig(local_ip, gateway, subnet);
}

String wifiStatus(int status){
    if (status==WL_IDLE_STATUS) return "Idle.";
    if (status==WL_NO_SSID_AVAIL) return "No SSID available.";
    if (status==WL_SCAN_COMPLETED  ) return "Scan completed.";
    if (status==WL_CONNECTED       ) return "Connected!";
    if (status==WL_CONNECT_FAILED   ) return "Connect failed.";
    if (status==WL_CONNECTION_LOST  ) return "Connection Lost.";
    if (status==WL_DISCONNECTED     ) return "Disconnected.";
    return "#"+String(status);
}

String WifiControl::getStatus()
{
  String result = "1," + wifiStatus(WiFi.status()) + ",";
#ifdef ESP8266
  result += WiFi.hostname();
#elif defined(ESP32)
  result += WiFi.getHostname();
#endif

  result += "," + WiFi.localIP().toString() + ":" + PORT; 
  return result;
}

void WifiControl::loop()
{
    if (_status != WiFi.status()) {
        _status = WiFi.status();
#if DEBUG_LEVEL&DEBUG_WIFI
        logv("Wifi: Connected status changed to %s", wifiStatus(_status).c_str());
#endif
        if (_status == WL_CONNECTED) {
            _tcpServer = new WiFiServer(PORT);
            _tcpServer->begin();
            _tcpServer->setNoDelay(true);
#if (DEBUG_LEVEL&DEBUG_WIFI) && defined(ESP8266)
            logv("Wifi: Server status is %s", wifiStatus( _tcpServer->status()).c_str());
#endif
            _udp = new WiFiUDP();
            _udp->begin(4031);

#if DEBUG_LEVEL&DEBUG_WIFI
            logv("Wifi: Connecting to SSID %s at %s:%d", INFRA_SSID, WiFi.localIP().toString().c_str(), PORT);
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

#if DEBUG_LEVEL&DEBUG_WIFI
        logv("Wifi: Could not connect to Infra, Starting AP.");
#endif
    }
}

void WifiControl::tcpLoop() {
    if (client && client.connected()) {
        while (client.available()) {
            String cmd = client.readStringUntil('#');
#if DEBUG_LEVEL&DEBUG_WIFI
            logv("WifiTCP: Query <-- %s#", cmd.c_str());
#endif
            String retVal = _cmdProcessor->processCommand(cmd);

            if (retVal != "") {
                client.write(retVal.c_str());
#if DEBUG_LEVEL&DEBUG_WIFI
                logv("WifiTCP: Reply --> %s", retVal.c_str());
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
#if DEBUG_LEVEL&DEBUG_WIFI
        logv("WifiUDP: Received %d bytes from %s, port %d", packetSize, _udp->remoteIP().toString().c_str(), _udp->remotePort());
#endif
        char incomingPacket[255];
        int len = _udp->read(incomingPacket, 255);
        incomingPacket[len] = 0;
#if DEBUG_LEVEL&DEBUG_WIFI
        logv("WifiUDP: Received: %s", incomingPacket);
#endif

        incomingPacket[lookingFor.length()] = 0;
        if (lookingFor.equalsIgnoreCase(incomingPacket)) {
            _udp->beginPacket(_udp->remoteIP(), 4031);
            /*unsigned char bytes[255];
            reply.getBytes(bytes, 255);
            _udp->write(bytes, reply.length());*/

#if defined(ESP8266)
            _udp->write(reply.c_str());
#elif defined(ESP32)
            _udp->print(reply.c_str());
#endif
            
            _udp->endPacket();
#if DEBUG_LEVEL&DEBUG_WIFI
            logv("WifiUDP: Replied: %s", reply.c_str());
#endif
        }
    }
}
#endif
