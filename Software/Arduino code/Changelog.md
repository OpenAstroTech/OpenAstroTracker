**V1.8.53 - Updates**
- Enforced DEC limits on slew movements. There is currently no indication that limiting has occurred.
- Added display of DEC limits to INFO screen.
- Changed some logging output.

**V1.8.52 - Updates**
- Moved LCD updates out of the mount loop.
- Fixed a bug that did not correctly handle displaying Roll and Pitch offset.
- Added ability to define DEC limits. Limits are not enforced yet, that code is still in development.
- Added new debug channel for digital level.

**V1.8.51 - Updates**
- Added EEPROM clear command to extended LX200 protocol.

**V1.8.50 - Updates**
- Fixed a bug related to setting EEPROM values.
- Moved some initialization code to a later stage during boot.

**V1.8.47 - Updates**
- Incorporated Roll levelling into guided startup (if Digital Level is present).

**V1.8.46 - Updates**
- Switched units for manual slew speed to degrees/sec.

**V1.8.45 - Updates**
- Fixed some ESP32 build errors
- Checked from some invalid ESP32 configurations
- Corrected some Meade command comments
- Sorted GO targets alphabetically (except Polaris)

**V1.8.44 - Updates**
- Fixed compiler define bug when Startup was disabled.

**V1.8.43 - Updates**
- Added ability to switch to the always-on coordinate display (top line RA, bottom line DEC) in CTRL menu. SELECT exits this mode.

**V1.8.42 - Updates**
- Fixed bug that turned of tracking when using NEMA17 with a non-UART driver.

**V1.8.41 - Updates**
- Removed UNO and ESP8266 support. Memory constraints are unsustainable.
- Removed Heating menu, since it hasn't been supported in months.
- Added user-submitted support for I2C LCD shield (thanks Christian Kardach)
- Added user-submitted support for Bluetooth on the ESP32 board (thanks mholeys)

**V1.8.40 - Updates**
- Fixed RA negative movement, which was not switching to Slew microstepping settings

**V1.8.39 - Updates**
- Fixed guiding to not jump back and forth on pulses.
- Fixed tracking during slew. We now turn off tracking for slew and compensate at the end of the slew.
- Fixed type for DEC motor speed.
- Added M33 and Pleiades to GO menu.
- Added lots of debug output around stepper motor control under new macro DEBUG_STEPPERS

**V1.8.38 - Updates**
- Fixed code to allow GPS use with Headless client.
- Tidied up some LCD strings

**V1.8.37 - Updates**
- Fixed bug that prevented the parking position from being read from persistent storage.
- Made sure Parking position was cleared  on EEPROM clear.

**V1.8.36 - Updates**
- Added GPS control to LX200 Meade protocol

**V1.8.35 - Updates**
- Added ability to define a parking position that is seperate from the Home position.

**V1.8.34 - Updates**
- Fixed a bunch of typos that massively screwed up DEC guiding

**V1.8.33 - Updates**
- Allowed axis inversion to be configured in local config
- Added Digital Level support to Meade serial protcol
- Fixed compilation problem with Digital Level

**V1.8.32 - Updates**
- Support for automatic local config selection based on selected board
- Included sample local config file.

**V1.8.31 - Updates**
- Now supports local configuration settings (thanks, AndreStefanov!)
- Major code re-structure, neccessary as the project grows.
- Fixed a bug that turned off Tracking when you tried to slew RA to the current location
- Moved some defines around
- Improved some comments
- Fixed a bug related to DEC guiding

**V1.8.27 - Updates**
- YABBFDTLOC (Yet Another Build Break FIx Due To Lack Of Coffee)
- Update Polaris to latest RA value.

**V1.8.26 - Updates**
- Build break fix for GPS-enabled configurations

**V1.8.25 - Updates**
- Added special char for Tracking display on LCD
- Renamed some conflicting defines (HALFSTEP et al to HALFSTEP_MODE)
- Moved all Debug strings to Flash memory
- Simplified EEPROM class (made it all static)
- Added prompt when pressing SELECT in either Roll or Pitch display in CAL menu, whether to save the reference value
- Fixed ESP32 compilation problem in freeMemory() implementation

**V1.8.24 - Updates**
- Allow EEPROM clearing via LCD buttons (press DOWN on boot)

**V1.8.23 - Updates**
- GPS location storage was broken. Fixed.

**V1.8.19 - Updates**
- Azimuth motor control during Polar Alignment was not implemented.
- After confirming Polar Alignment, the display shows slewing activity and no longer switches to the INFO menu.
- Pressing Right Button while slewing home after Polar Alignment now stops the slewing before changing menus.
- Removed some logging

**V1.8.16 - Updates**
- Throttled the INFO display update frequency (it was updating every cycle before).
- Added temperature display when using the Digital Level module
- Made the axis swap for the Digital Level on by default since most people will use the STL in the repo for the holder.

**V1.8.15 - Updates**
- Fixed linker error in ESP32 build.

**V1.8.14 - Updates**
- Turned the power to the Azimuth and Altitude motors on and off as needed.
- Enabled Altitude correction from LX200 commands
- Prevented tracking from starting after parking for NEMA motors.

**V1.8.13 - Updates**
- Made the LEFT button leave the Roll and Pitch menus without storing the offset (like SELECT does).

**V1.8.12 - Updates**
- Temporary fix for compile error from 1.8.11
- add different MS modes for DEC slewing/guiding
- add UART "noisefeedback" to config
- hardcoded guidemultipliers for 28BYJ-48 steppers
- stop tracking during a slew with NEMAs to prevent inaccurate slews

**V1.8.11 - Updates**
- Added support for electronic levelling via MPU-6050 module (CAL menu) (thanks to Marcel Isler for initial code)
- Disabled AzAlt stepper motors after leaving CAL menu
- Moved an overlapping EEPROM storage space (brightness, not used)

**V1.8.10 - Updates**
- Permanent display of tracking state in top-right corner of display.
- Shortened menu display
- Full GPS module (GT-U7) support for LST and location
- Separated HA menu into two files (one for GPS support, one for non-GPS)
- Added display of current location to INFO menu

**V1.8.06 - Updates**
- DEC axis was inadvertently inverted by default after merge. Fixed.
- Guide pulses were not 2x sidereal and 0 for default steppers. Fixed.

**V1.8.05 - Updates**
- Reverted Headless mode to off
- Increased screen updates back to 5Hz.

**V1.8.04 - Updates**
 - Merged the developautopa branch
 - Made AzAlt motors configurable.
 - Added extensions to the Meade protocol (:MAZn.n# and :MALn.n#) to move the aziumth and altitude motors.
 - Added Az and ALt motor status to :GX# command output
 - Added GPS and AzAlt features to hardware info query (:XGM#)
 - New CAL menu items to adjust Azimuth and Altitude in arcminutes. 
