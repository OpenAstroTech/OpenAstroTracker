---
layout: default
title: Arduino LCD
parent: How to use the OAT
nav_order: 1
permalink: /docs/using_oat/lcd
---

# OpenAstroTracker LCD Guide

### Table of contents
{: .no_toc .text-delta }
1. TOC
{:toc}

The tracker uses a LCD display that shows two rows of 16 characters to allow you to interact with it. It has 6 buttons on it.

<p align="center">
<img src="../lcd/image13.png" width="50%">
</p>

If you don’t see any text on the system, you may need to turn the Brightness screw until you do.

The **SELECT** button chooses an option, the **LEFT** button generally navigates in sub menu items, **UP** and **DOWN** are used to scroll through lists, **RIGHT** is used in top-level menu navigation. And the **RESET** button is generally not used, since it does just that. It immediately stops all operation and reboots the Arduino. Think of it as a Panic button (in case you don’t have your towel with you) to use when you see things are about to turn bad….

Generally the system displays a menu of options on the first line that you can select by pressing (and releasing) the **RIGHT** button. The **RIGHT** button simply moves you along from menu item to menu item (with one exception that we’ll come to later). 

The second row of characters usually displays information or allows you to do something pertinent to that menu.

The highlighted items are marked by being enclosed by **arrows** like this:


<p align="left">
<img src="../lcd/image5.png" width="10%">
</p>


Or by having a **single arrow** to the left of it like this:


<p align="left">
<img src="../lcd/image18.png" width="10%">
</p>


The selected menu item is always displayed in the middle of row 1 and is surrounded by arrows.


# Initial Setup on Powerup

When the tracker is powered up you will see a short splash screen that shows the current version number, similar to this:


<p align="center">
<img src="../lcd/image23.png" width="50%">
</p>


Once it has started up, the system will guide you through an initial setup step. In order for the tracker to work correctly, it needs to be started with the mount aligned with the celestial pole. So we want the RA ring centered and the lens (DEC wheel) pointing ‘up’ at the celestial pole (basically the DEC wheel should be oriented with its open end (where the belts go in) at the back of the RA ring). 



## 1. Aligning to Celestial Pole

 So the first thing you will see after the splash screen is the following screen:


<p align="left">
<img src="../lcd/image10.png" width="50%">
</p>


 It is asking you whether the mount is in its home position, which is RA ring centered and camera lens pointing out of the right at right angles. If it is, you can use the **LEFT **button to move the arrow cursor to the correct option. Note that the **LEFT **button actually moves the arrow cursor to the right (you’ll get used to that, promise).



 *   **YES** - If the mount is correctly oriented, select this option. You will skip to Step 4
 *   **NO** - If the mount is not correctly oriented and you want to move it there select this option. You will continue at Step 2
 *   **CANCL** - If you don’t want to finish the initial setup, select this. Note that the controller assumes that the tracker is correctly oriented when it does its operations, so be careful not to command it to do things that will make the RA or DEC rings go too far since physical damage is likely to result (use **RESET **to avoid this when in trouble). \
 This exits to the main menu





## 2. Moving the mount

If you selected **NO**, you will land in a mode that allows you to move the two rings to their correct positions. The menu displayed looks like this:

<p align="left">
<img src="../lcd/image6.png" width="50%">
</p>


You can now use the **LEFT** and **RIGHT** buttons to move the RA ring to the left (CW) and right (CCW). You can use the **UP** and **DOWN** buttons to move the DEC ring such that the lens moves up or down. 

Pressing and holding one of the buttons slews the mount in the corresponding direction. Releasing it stops the motion. Only one axis can be operated at a time. Note that both axes have acceleration and deceleration so they will go a little further than when you released the button as it decelerates. The DEC wheel has a longer acceleration and deceleration phase than the RA wheel.



## 3. Confirm the Home position

Once the mount is aligned correctly, press the **SELECT** button. This brings up the following screen:

<p align="left">
<img src="../lcd/image10.png" width="50%">
</p>

If you’re satisfied with the position of the mount, select **Yes** (use **LEFT** to move the selection) and hit **SELECT**. The mount now knows where the home point is. You are returned to Step 1, the screen asking you whether the mount is pointed at the pole. 

If you select **No**, you are also returned to Step 1, but the mount still assumes the home point is how the mount was positioned at startup.


## 4. Enter the HA

In this step you need to enter the current HA. You will see this screen:

<p align="left">
<img src="../lcd/image25.png" width="50%">
</p>

Use the **UP** and **DOWN** keys to set the hour of the HA. 

Use the **LEFT** key to move between hours and minutes.

Use the **SELECT** key to confirm the entry. 

This completes initial setup.

Note that at this point you need to follow the instructions in the [Polar Alignment Guide](https://drive.google.com/file/d/1WRThR709xvfjo2IPQmQuXUPK6YBmoFnY/view) or use the Polar Alignment function in the CAL menu.


# Control through USB

When the mount is being controlled via an attached computer and being sent commands via the serial port, you will see this display:

<p align="left">
<img src="../lcd/image24.png" width="50%">
</p>

As long as the program is connected and controlling the mount, you’ll see this screen, showing the current RA and DEC of the mount. If for some reason you want to take control via LCD Shield you can press **SELECT**.

Currently a subset of the **Meade** protocol is implemented and there is an **ASCOM** driver available. A PC control application is also under development.


# Main Menu

The main menu consists of the following menu items:



1. **RA** - for Right Ascension coordinates
2. **DEC** - for Declination coordinates
3. **GO** - for place to make the mount go to
4. **HA** - To set the Hour Angle time
5. **CTRL** - To manually move the mount
6. **CAL** - to do polar alignment, drift calibration and fine tune tracking speed 
7. **INFO** - to display information about the mount

## 1. RA

<img align="right" src="../lcd/image3.png" width="35%">

This menu allows you to set the target Right Ascension (JNow) that you want to slew to. 

You can use the **LEFT** button to jump from hours to minutes to seconds and back. The target RA is immediately updated in memory, but the slew only starts when you press **SELECT** in either this menu or the DEC menu.

So if you are changing both RA and DEC, you can enter both and then press **SELECT** in either menu to start slewing.


## 2. DEC

<img align="right" src="../lcd/image14.png" width="35%">

This menu allows you to set the target Declination(JNow) that you want to slew to. 

You can use the **LEFT** button to jump from degrees to minutes to seconds and back. The target DEC is immediately updated in memory, but the slew only starts when you press **SELECT** in either this menu or the RA menu.

So if you are changing both RA and DEC, you can enter both and then press **SELECT** in either menu to start slewing.



## 3. GO

<img align="right" src="../lcd/image17.png" width="35%">

This menu allows you to slew the mount to some galaxies and stars as well as the home position. Specifically at this time it supports:



*   Polaris
*   Megrez (center star of Ursa Major, aka the big dipper)
*   M31 Andromeda Galaxy
*   M42 Orion Nebula
*   M51 Whirlpool Galaxy
*   M63 Sunflower Galaxy
*   M81 Bodes Galaxy
*   M101 Pinwheel Galaxy

Use the **UP** and **DOWN** keys to move between these entries and press **SELECT** to slew the mount to that selection. Use **RIGHT** to move to the next Main Menu item.

The second last item is **HOME**, which will return the mount to its home position. Thsi should be done before powering down the mount.

The last item in the menu is **PARK**, which does the same as **HOME**, but additionally turns off tracking. If you leave the mount in **PARK** for a while you will have to update the **HA** when you start using it again.



## 4. HA

<img align="right" src="../lcd/image4.png" width="35%">

This menu allows you to set the Hour Angle for calculating the correct locations of the stars.

Use the **UP** and **DOWN** arrow to change the selected component (hour or minute) and use **LEFT** to switch between them.

Confirm the change with **SELECT** or **RIGHT**.



## 5. CTRL

<img align="right" src="../lcd/image9.png" width="35%">

This menu allows you to manually control and slew the mount without updating the RA or DEC values.

When this menu is displayed, press **SELECT** to enter manual control mode.

The display now shows you the **RA** and **DEC** stepper motor positions. You can now use the following buttons:

**LEFT** button to rotate the RA ring clockwise (looking at the mount head on).

**RIGHT** button to rotate the RA ring counter-clockwise (looking at the mount head on).

**UP** button to rotate the DEC holder such that the lens would point further up.

**DOWN** button to rotate the DEC holder such that the lens would point further down.

**SELECT** to leave manual control mode.


Notice that movement only occurs while the button is held down. When the button is released the movement stops. Also note that both steppers do not stop instantly, they both have acceleration and deceleration curves.

<img align="right" src="../lcd/image10.png" width="35%">
\
When you leave manual control mode via **SELECT**, you will see the screen shown on the right.

If the mount is at the home position you should select **YES**. In this case the mount is reset to the home position. You will likely need to update **HA** (see Step 5) in this case as well. If the mount is not at the home position, select **NO**. Note that in that case the software assumes the mount is where it was when you entered CTRL mode.

.



## 6. CAL

<img align="right" src="../lcd/image12.png" width="35%">

This menu allows you to do a polar alignment and to adjust for slight deviations in hardware so that tracking is in sync with earth’s rotation. 

There are two submenu items that display in this menu:

<img align="right" src="../lcd/image15.png" width="25%">

*   **Polar alignment:** This will do a polar alignment by having you center the mount on Polaris.
*   **Speed Calibratn:** This allows you to adjust a factor that is applied to the calculated tracking speed of the hardware. The tracking speed is multiplied with this number, so higher than 1.0 will make it track faster, lower than 1.0 will make it track slower.
*   **Drift alignment:** This will allow you to check drift by slewing the mount east and west with pauses at each end. 

You can use the **DOWN** button to select between the mode and then **SELECT** to choose that mode.

\
**<span style="text-decoration:underline;">Polar Alignment</span>**

<img align="right" src="../lcd/image8.png" width="35%">

After you press the **SELECT** button the mount will move to a distance beyond Polaris, such that the distance the mount thinks it’s pointed is the same distance from where Polaris should be than POlaris is from the celestial pole.You should now move the whole mount (physically) in such a way that Polaris is in the middle of your camera's viewfinder. Use the screws if you need to incline it, but make sure it’s still level sideways. You will see the screen at right.

Once Polaris is centered, hit **SELECT**. Your mount is now aligned to the celestial pole. The mount will slew to its home position. You will be back at the main menu.

**<span style="text-decoration:underline;">Speed Calibration</span>**

<img align="right" src="../lcd/image19.png" width="35%">

After you press **SELECT**, the speed factor will be displayed for you to adjust.

**UP** increases the number. The rate of increase increases the longer you hold down the button.

**DOWN** decreases the number. The rate of decrease increases the longer you hold down the button.

**SELECT** stores the value in the Arduinos non-volatile memory, so it’ll be retained across power shutdowns.

**RIGHT** leaves the menu as usual.

**<span style="text-decoration:underline;">Drift Alignment</span>**

<img align="right" src="../lcd/image22.png" width="35%">

After you press **SELECT**, you will be asked for the duration of the drift alignment.

This should correspond to the exposure time of your camera. The mount will pause 1.5s, then slew east for (almost) half the selected time, pause 1.5s, then slew west the same time as east and pause a last time for 1.5s. This entire sequence will take 1.5s less than the selected exposure time. So you can open the shutter and then confirm the selected time. Then simply let the sequence run. You can then check the photo for drift (see Drift Alignment under Guides).


## 7. INFO

<img align="right" src="../lcd/image11.png" width="35%">

This menu allows you to examine a few internal values of the software.

The display of these items can be cycled through by using the **UP** and **DOWN** arrows. Some of the items have sub-items that can be cycled through with the **LEFT** button.

The first level items that can be cycled through are:



1. **RA**
2. **DEC**
3. **TRK**
4. **Uptime**
5. **Firmware**

<br/><br/>
<img align="right" src="../lcd/image16.png" width="30%">

On the **RA** item, you can cycle through the following three values with the **LEFT** button:

*   Stepper position in steps
*   Current RA position
*   Target RA position

<br/><br/>
<img align="right" src="../lcd/image20.png" width="30%">

On the **DEC** item, you can cycle through the following three values with the **LEFT** button:

*   Stepper position in steps
*   Current DEC position
*   Target DEC position

<br/><br/>
<img align="right" src="../lcd/image21.png" width="30%">

On the **TRK** menu, you you can cycle through the following two values with the **LEFT** button:

*   Tracking Stepper position in steps
*   Tracking speed (in steps per second)

<br/><br/>
<img align="right" src="../lcd/image1.png" width="25%">

On the **Uptime** menu it will show you how long the Arduino has been powered up.

<br/><br/>
<img align="right" src="../lcd/image7.png" width="25%">

On the **Firmware** menu it will show the current version of the OpenAstroTracker software that the Arduino is running.