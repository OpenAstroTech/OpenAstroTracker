---
layout: default
title: Power wiring
grand_parent: Assembly
parent: Wiring
nav_order: 1
---

## 28BY and ULN2003 power wiring

Get a old USB cable and cut the end off. Inside will be 4 wires. Green, white, red and black. You can ignore green and white, those are the data lines.
Strip the red and black wire and insert them into the + and - ports of the buck converter. (Red = +; black = --)

<img  src="../power_wiring/image3.png" width="75%">

Cut off 4 dupont wires with female connectors, strip the other ends and insert them into the output of the buck converter.

<img  src="../power_wiring/image1.jpg" width="75%">

Screw both ULN2003 drivers on top. Connect the + port to the + pins and the -- port to the -- pins.

<img  src="../power_wiring/image2.jpg" width="75%">


<br/><br/>

## NEMA and bipolar drivers power wiring

Id you are using the driver holder boards, simply cut up a USB cable as above, split it into two wires (by soldering or crimping) and connect each to the corresponding + and -- terminals of the boards.

<img  src="../power_wiring/nema_power.png" width="75%">