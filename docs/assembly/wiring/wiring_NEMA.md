---
layout: default
title: NEMA wiring
grand_parent: Assembly
parent: Wiring
nav_order: 3
heading_anchors: true
---

## Table of contents
{: .no_toc .text-delta }
1. TOC
{:toc}


<br/><br/>
## Wiring:

<img src="../2209_pins.jpg" width="100%">

Wire|Pin||Wire|Pin
-|-|-|-|-
RA Step|22||DEC Step|23
RA Direction|24||DEC Direction|25
RA Enable|26||DEC Enable|27
RA UART|TX3||DEC UART|TX3
------------------------

For UART, the DEC driver board needs to have MS1 enabled, do this by switching the '1' button to ON. All others, also on the RA driver have to be OFF

Be sure to have "To Arduino GND" actually connected properly to Arduino GND. It HAS to be Arduino GND, it can not be the same GND as from the stepper powersource!


## How to test the UART connection:

After you have both steppers wired up, go into the code. Make sure that all necessary settings for TMC UART are set. Those are:

- RA/DEC_STEPPER_TYPE
- RA/DEC_DRIVER_TYPE
- SET_MICROSTEPPING to adjust RA slew stepping
- TRACKING_MICROSTEPPING 
- DEC_MICROSTEPPING

!! Important !!
If you change any microstepping and you have adjusted the RA or DEC steps at any point, you need to run the EEPROM_clear sketch in File > Examples > EEPROM!

These are the important settings, but there are more detailed settings in the TMC2209 UART section in Configuration_adv.

To verify that the UART connection can successfully set settings, go into the "mount.cpp" tab, and find the line:

	//_driverRA->en_spreadCycle(1);
    
It should be on line 360. Remove the two slashes // infront it and upload the code. If everything went right, your RA motor should make very strange sounding noises, but this is only the stealthchopper being off, don't worry. Add the two slashes again, and scroll a bit down. You will find a very similar line: 

	//_driverDEC->en_spreadCycle(1);
    
This time the DEC motor should make weird noises. If it does, add the slashes again, upload and youre good to go.

If it does not:

- double check your wiring
- Especially make sure that Arduino GND is properly connected to the driver IO GND
- You may connected to the wrong UART pin on the driver. Try the other one. There are two, and some manufacturers have only one of them enabled:


### Bigtreetech / MKS:



<img src="../BTT_UARTpin.png" width="50%">


### Watterot



<img src="../Watterot_pins.png" width="50%">

You will have to solder a bridge thats right beneath the two UART pins. I is 3 pads that all need a connection with each other. Then, solder a wire or pin on the top of the board to one of the UART pins



<br/><br/>
# Troubleshooting
{: .d-inline-block }
- Axis only turning in one direction:

In Configuration_adv.hpp, find INVERT_RA/DEC_DIR and change it to either 1 or 0 for the axis in question. If the rotation is now still in the same direction than before, doubecheck you DIR wire