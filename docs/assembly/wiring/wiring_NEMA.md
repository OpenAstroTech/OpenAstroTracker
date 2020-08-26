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
# Wiring:

<img src="../nema_wiring/2209_pins.jpg" width="100%">

Wire|Pin||Wire|Pin
-|-|-|-|-
RA Step|22||DEC Step|23
RA Direction|24||DEC Direction|25
RA Enable|26||DEC Enable|27
RA UART|TX3||DEC UART|TX3

------------------------

For UART, the DEC driver board needs to have MS1 enabled, do this by switching the '1' button to ON. All others, also on the RA driver have to be OFF

Be sure to have "To Arduino GND" actually connected properly to Arduino GND. It HAS to be Arduino GND, it can not be the same GND as from the stepper powersource!

## If using CAT6 adapters

For both drivers, you can get away with exactly 6 wires + 2 for the DIAG pins (used by autohoming). So you can fit everything into 1 Ethernet cable.

<img src="../nema_wiring/image1.png" width="75%">

To not get confused later, it helps a lot to colorcode you connections and write them down. The side of the CAT6 adapter will have some numbers on it. For example write down: Slot 1, Blue, RA STEP.
This will help a lot when you do the other side. 

The pins can be changed in the configuration_pins.hpp, but the dafault wiring is:
- 1x GND:          Arduino GND
- 2x RA/DEC STEP:  22/23
- 2x RA/DEC DIR:   24/25
- 1x UART:         Tx3
- 2x RA/DEC DIAG:  28/29

As you can see in the image, bend pin connectors help a lot. 

<img src="../nema_wiring/image2.png" width="75%">

On the drivers side, connect the correct slots to the driver pins. If you wrote down which slot has which signal this will be fairly straightforward.
There are some special cases to be noted:

- GND slot has to have two wires going out, connecting to both drivers. Do this by either forcing two wires into one slot of the CAT6 adapter, or solder/crimp yourself a Y connection.
- The same goes for UART. Two wires to both drivers from one Slot.
- To save wires, there is no EN signal transmitted from Arduino. The TMC2209 needs to have its EN pins driven with a logic LOW signal to work,
in order to do that wire the E pins of the drivers to a GND. Alternatively you can use the DIAG wires for that (if you dont use autohoming) and wire them to the Arduino pins 26/27

# How to test the UART connection:

After you have both steppers wired up, go into the code. Make sure that all necessary settings for TMC UART are set. Those are:

- RA/DEC_STEPPER_TYPE
- RA/DEC_DRIVER_TYPE
- SET_MICROSTEPPING to adjust RA slew stepping
- TRACKING_MICROSTEPPING 
- DEC_SLEW_MICROSTEPPING  
- DEC_GUIDE_MICROSTEPPING 

**!! Important !!**
If you change any microstepping and you have adjusted the RA or DEC steps at any point, you need to run the EEPROM_clear sketch in File > Examples > EEPROM!

These are the important settings, but there are more detailed settings in the TMC2209 UART section in Configuration_adv.

To verify that the UART connection can successfully set settings, go to the Configuration_adv tab and find

	#define RA_AUDIO_FEEDBACK  0
    
Change the 0 to 1. If UART is working as intended, your RA motor should make very strange sounding noises, but this is only the stealthchopper being off, don't worry. Change it back to 0 and do the same for the line below, for DEC:

	#define DEC_AUDIO_FEEDBACK 0
    
This time the DEC motor should make weird noises. If it does, change it back to 0 and youre good to go.

If it does not:

- double check your wiring
- check your wiring again
- Especially make sure that Arduino GND is properly connected to the driver IO GND
- You may connected to the wrong UART pin on the driver. Try the other one. There are two, and some manufacturers have only one of them enabled:


### Bigtreetech / MKS UART pin:



<img src="../nema_wiring/BTT_UARTpin.png" width="50%">


### Watterot UART pin:



<img src="../nema_wiring/Watterot_pins.png" width="50%">

You will have to solder a bridge thats right beneath the two UART pins. It is 3 pads that all need a connection with each other. Then, solder a wire or pin on the top of the board to one of the UART pins



<br/><br/>
# Troubleshooting
{: .d-inline-block }
- Axis only turning in one direction:

In Configuration_adv.hpp, find INVERT_RA/DEC_DIR and change it to either 1 or 0 for the axis in question. If the rotation is now still in the same direction than before, doubecheck you DIR wire

- Stepper just vibrating but not moving:

Check your STEP wire. Also test your UART connection as described [above](#how-to-test-the-uart-connection).

- Stepper getting **really** warm

It is normal and even desired that the steppers get warm during operation. If they however get very hot very fast, youre giving them too much current. 
Reduce the value for RA/DEC_RMSCURRENT in Configuration_adv and to be save also reduce the max current over the drivers potentiometer. NOTE that TMC2209 INCREASE their current if you turn
the potentiometer counterclockwise! So you have to turn the pot clockwise to decrease the current, which will feel wrong but isnt.