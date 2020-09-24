---
layout: default
title: Wifi for ESP32
grand_parent: Assembly
parent: Software
nav_order: 1
---

Last updated: 9/20/2020

The following is only relevant if you're using a WiFi-enabled microcontroller like the ESP32

<br><br>
## Setup:

To connect OAT Control to ESP32, OAT Control and ESP32 must neighbors of the same sub wifi network. Assuming OATControl runs on a laptop, as of 9/20/2020

- laptop and ESP connected to a mobile hotspot => works
- laptop connected to ESP's WiFI => doesn't work
- laptop hosting a WiFi network to which ESP is connected => doesn't work


To setup your ESP32 for WiFi, open the Arduino code and find Configuration_adv.hpp. Then, locate the lines:

	// WIFI SETTINGS
  
See these lines: 

	#define INFRA_SSID "ssid"
	#define INFRA_WPAKEY "password"
	#define OAT_WPAKEY "password"
	#define HOSTNAME "ssid"

	#define WIFI_MODE 0
	// 0 - Infrastructure Only - Connecting to a Router
	// 1 - AP Mode Only        - Acting as a Router
	// 2 - Attempt Infrastructure, Fail over to AP Mode.


WIFI_MODE offers to setup your ESP32 either as a WiFi defice who connects to an external network (Infrastructure - mode 0), or to act as a WiFi hotspot. 

At the time of this writing, you must set WIFI_MODE to 0, and enter the credentials to connect to the external WiFi for INFRA_SSID and INFRA_WPAKEY. Your laptop will need to connect to the same network. 



Once that's done, saved, and uploaded, the ESP will try to connect to the provided network (make sure it is up!). Now you can open OATControl, and hit connect. 
Under the connection section where you usually see 

	COM_
You should see a WIFI connection: select it and connect. 
Now OATControl should be sending commands to the OAT via the ESP32! 
