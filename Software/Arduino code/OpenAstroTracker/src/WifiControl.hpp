#pragma once
#include "../Configuration_adv.hpp"

#ifdef WIFI_ENABLED
#include "Mount.hpp"
#include "LcdMenu.hpp"
#include "MeadeCommandProcessor.hpp"
#include "WiFiServer.h"
#include "WiFiUdp.h"
#include "WiFiClient.h"

#ifdef ESP8266
#include "ESP8266Wifi.h"
#endif

#ifdef ESP32
#include <WiFi.h>
#include <WiFiSTA.h>
#endif

class WifiControl {
public: 
    WifiControl(Mount* mount, LcdMenu* lcdMenu);
    void setup();
    void loop();
    String getStatus();
private: 
    void startInfrastructureMode();
    void startAccessPointMode();
    void infraToAPFailover();
    void tcpLoop();
    void udpLoop();
    wl_status_t _status;
    Mount* _mount;
    LcdMenu* _lcdMenu;
    MeadeCommandProcessor* _cmdProcessor;

    WiFiServer* _tcpServer;
    WiFiUDP* _udp;
    WiFiClient client;

    unsigned long _infraStart = 0;
    unsigned long _infraWait = 30000; // 30 second timeout for 
};

extern WifiControl wifiControl;

#endif // WIFI_ENABLED
