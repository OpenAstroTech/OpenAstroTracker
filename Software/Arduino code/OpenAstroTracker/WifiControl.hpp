#pragma once
#include "Globals.h"

#ifdef WIFI_ENABLED
#include "Mount.hpp";
#include "LcdMenu.hpp";
#include "MeadeCommandProcessor.h"
#include "WiFiServer.h"
#include "WiFiUDP.h"
#include "WiFiClient.h"

class WifiControl {
public: 
    WifiControl(Mount* mount, LcdMenu* lcdMenu);
    void setup();
    void loop();

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

    long _infraStart = 0;
    int _infraWait = 30000; // 30 second timeout for 
};
#endif
