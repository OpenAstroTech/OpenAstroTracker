---
layout: default
title: Troubleshooting / common issues
nav_order: 5

---

# Troubleshooting Guide
{: .no_toc }

### Table of contents
{: .no_toc .text-delta }
1. TOC
{:toc}


## LCD screen doesnt show anything
If the upload completed and the RA drivers LEDs blink, the code is working. You will have to adjust the screen contrast via the potentiometer on the LCD. Turn the tiny screw on the small blue box until letters are visible on the LCD. This might take a lot of turns.


## LCD Screen shows something but not all
This can happen when the Arduino runs out of memory. If you are using an UNO, you cannot enable all the features as well as Serial support, for example. Check the output of Arduino Studio when uploading. If the program usage goes above 80%-85% or the data above 75%, you might be experiencing this.


## Motors not moving
If the motors don't move, or just vibrate but do not move, double check your wiring. If you are certain your wiring is correct, try switching the position of the wires 2 and 3. If that doesn't work, try different wires, they are sometimes broken.
If you are using a Mega, make sure you wired the steppers as indicated in the Mega wiring diagram and not like the Uno one. If you soldered the stepper wires onto the LCD Shield, the DEC pins will need to be increased by 40 (so 15 becomes 55, etc).

If you're using the Mega with the standard pinout according to the wiring guide, go to the a_inits.ino in the code and change the numbers for the RA and DECmotorpins as following: 22, 24, 26, 28 for RA, 30, 34, 32, 36 for DEC (yes, the order is correct). This is going to be automated in the next update. 

## Motors STILL not moving / only vibrating
It has been reported that certain kinds of ULN2003 versions dont work as intended. Take a very close look to the small black piece on the drivers and the number written on it. 

<img src="../image1.jpg" width="75%">

If it says "ULN2003APG" you may have one of those strange behaving drivers. Buy new ones from a different source. Tip: you can [buy the IC only](https://www2.mouser.com/ProductDetail/Texas-Instruments/ULN2003AN?qs=FOlmdCx%252BAA1wYQ1G8c8hpQ==) instead of the whole driver board, but you'll have to switch it manually which can be a bit hard. 

## Is it moving correctly?
When you use either the LCD or the PC OATControl app, using the arrows to move the mount from startup should cause this behavior (in the northern hemisphere):
Pressing Left should move the RA ring logo to the left (CW) and decrease the RA hour time.
Pressing Right should move the RA ring logo to the right (CCW) and increase the RA hour time.
Pressing Up (from home position) will move the camera lens upwards and DEC will decrease and RA will flip by 12 hrs
Pressing Down (from home position) will move the camera lens downwards and DEC will decrease

## Mount isn’t tracking
If the mount moves when you set coordinates but doesn’t if it’s tracking, check the LEDs on the RA driver. They should be “marching” in a straight pattern. If they do, the mount is tracking, but keep in mind that the movement is extremely slow, too slow to actually see any movement. To test the tracking, leave the mount on for 10-20 minutes and see if it has moved a little.

## I have Star trails on my exposures
First check that you have correctly set RAPulleyTeeth in OpenAstroTracker.hpp. If you printed the RA pulley, you will need to change it to 20. If you purchased gears, count how many teeth they have, they usually come in 20 and 16 tooth variants.
If you have set this correctly and are still getting star trails, look at the direction in which they occur. RA Tracking speed errors will cause horizontal streaks. If this is the case, you should set the Speed Calibration setting to either speed up (make it larger than 1.0) or slow down (make it less than 1.0) tracking.

If you streaks are not continuous, you may have bumped the mount or other vibrations may have caused it. Telescopes are very sensitive to vibrations.

If the streaks are not horizontal you may not be correctly Polar aligned. If you have aligned by Polar Alignment only, you may need to also do a Drift Alignment to get an exact polar alignment.


## Star trails after a slew (GoTo)
This is a common behavior. The gears in the motor have become misaligned and first have to overcome the backlash. Wait one or two minutes and things should be back to normal. Current firmware does account for backlash when using GoTo to slew to various locations, but not after a manual slew.


## ASCOM / PC not connecting, connection timing out
Did you enable “SUPPORT_SERIAL_CONTROL” in Globals.h?


## I can’t connect via Bluetooth
Yes, the current code base does not support Bluetooth, yet. It is on our plan of features, so it may come at some point in the future. The other wireless option is to use the ESP32 board, which support WiFi connectivity.


## How can I find and send logfiles?
For ASCOM Logs: In the ASCOM Chooser Dialog, check the option to enable Trace Logging. Then execute the action that causes problems and shutdown the app.
Navigate to your \Documents\ASCOM\Logs YYYY-MM-DD\ folder, you’ll find log files in here.
For OATControl logs: Navigate to the following folder in your AppData folder: C:\Users\<you>\AppData\Roaming\OpenAstroTracker\. The logfiles of you last 6 sessions will be saved there.

## Is there a support avenue?
Well, this is an Open Source project, so technically there is no official support. However, we will all try to help you as best we can, either on Reddit in [/r/OpenAstroTech](https://www.reddit.com/r/OpenAstroTech/) or on our Slack server where we hang out and discuss the tracker (amongst other things, like 3D printing, weather, AP equipment, pandemics, etc.) The server can be reached [>here<](https://join.slack.com/t/spaceonly/shared_invite/zt-5qejkkme-mj5gLaWuwhCJ_fNRIUY7Ow) Join the #oat channel.
