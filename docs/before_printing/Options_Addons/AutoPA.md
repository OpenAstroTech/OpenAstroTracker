---
layout: default
title: AutoPA
parent: Options and Addons
grand_parent: Before you print
nav_order: 1
---
# AutoPA (Auto Polar Alignment)

The AutoPA upgrade is a work-in-progress that allows motorized polar alignment by turning the Azimuth and Altitude using two additional stepper motors. 

The steppers turn the OAT with an **estimated** precision of 4 arcseconds/step for Azimuth, and 1 arcsecond/step for Altitude.

They can be manually controlled in arcminute integers via the calibration menu on the LCD, or via LX200 commands for finer precision.

# Parts List

| Item|Quantity|
| -|-
| 28BYJ-48 stepper|2|
|6801 bearing|4
|6001 bearing|1
|5 mm shaft, 57 mm long minimum|1
|16T pulley|3
|GT2 6 mm belt|Approx. 500 mm
|Superglue|
|GT2 belt tension spring|2
|Scotch (cellophane) tape|
|Various size M3 screws (mostly 12mm)|Approx. 20
|M3 nuts|6
|M4 12 mm screws|2
|M4 8 mm screws|4
|LM2596 buck converter (6V output)|1


# Printed Parts List

| Item|Quantity|
| -|-|
|13_rear_AltAz_mount.stl|1 - Replaces default rear mount
|Alt_gearbox.stl|1
|Alt_track.stl|1
|Alt_wormgear_cap.stl|1
|Alt_wormgear_largegear.stl|1
|Alt_wormgear_worm.stl|1
|Az_M14_belt_ring.stl|2
|Az_M14_dome_cap.stl|2
|Az_motor_mount.stl|1
|Az_pivot_frameA.stl|1
|Az_pivot_frameB.stl|1
|Az_slide_frame_left.stl|1
|Az_slide_frame_right.stl|1
|Az_slide_plate.stl|2


# Altitude Assembly Instructions

Assembly:
1. Superglue a strip of belt onto altitude track and ensure it's centered perfectly, otherwise there will be too much friction between the track and the rear mount.
	1. Insert altitude track into the rear mount and ensure it slides fairly easily. Trim the opening in rear mount with a utility knife if needed. 
	2. Slide the track back and forth repeatedly to wear it down a bit if needed.
2. Insert 1x 6801 bearing into the rear mount
3. Insert 3x 6801 bearings into the altitude gearbox
4. Slide the worm through gearbox so that the end with the openings faces the stepper
5. Assemble the drive shaft:
	1. Slide one 16T pulley onto 5mm shaft facing the rear mount and leave loose. 
	2. Temporarily insert the altitude track through bottom opening of the rear mount and align the 16T pulley with the center of the belt. 
	3. Remove the altitude track and pulley **(without allowing the pulley to shift)** then tighten the grubscrews.
	4. Remove flange from 16T pulley. Insert pulley into the wormgear large gear. The teeth of the pulley should be a relatively snug fit with no movement.
	5. Slide the wormgear large gear onto the end of the 5 mm shaft
	6. Install the shaft through rear mount and gearbox. Push the gearbox towards the rear mount until the large gear is touching the 6801 bearing inside the Gearbox.
	7. Remove gearbox and tighten the grubscrews on the 16T pulley attached to the large gear.
6. Install the gearbox to the rear mount with 4x M3 12mm screws.
7. Insert 2x M3 nuts into the worm.
8. Install the 28BYJ using M4 screws and tighten motor shaft in the worm using 2x M3 screws
9. Wire stepper according to **configure_pins.hpp** and run motor using the altitude control in the CAL menu to move Altitude Track into place.

# Azimuth Assembly Instructions

1. Assembly pivot frame using M3 10mm screws and M3 nuts
2. Insert 6001 bearing into pivot frame
3. Assemble slide frame and motor mount using M3 12mm screws.
4. Attach slide frame and pivot frames together using M3 screws (8, 10, or 12mm should work).
5. Insert the bottom of the altitude track in the 6001 bearing on the pivot frame.
5. Remove flange from 16T pulley. Install on 28BYJ stepper.
6. Install stepper on motor mount.
7. Wire stepper according to **configure_pins.hpp** and run motor using the azimuth control in the CAL menu to pull the belt through the motor mount.
8. Install the M14 belt ring parts on each front M14 leg with the belt tightening screw holes facing forward away from the OAT.
9. Install M3 nuts in each M14 belt ring to tighten the belt later.
10. Screw the M14 dome caps to each front leg.
	1) Optional: Sand down the dome caps prior to installation using ~200 grit sandpaper to have a completely smooth surface at the bottom where it touches the slide plate.
11. Feed the belt through each M14 belt ring with some slack on both ends.
12. Install a GT2 belt tension spring within the opening of each M14 belt ring.
13. Tighten the belt screw on one of the M14 belt rings. Pull the loose belt end while simultaneously pulling the motor block away from the tightened belt screw **(both tension springs should be stretched at this point)**
14. Tighten the remaining belt screw.
