OpenAstroTracker ASCOM driver 0.2.0.0b : 2020-04-23

This driver is still in a great deal of flux.  Capacitor levels of flux.  Use at your own risk!

* History
	* 0.1.3.0	2020-04-15		:		Initial release
	* 0.1.3.1	2020-04-16		:		BUGFIX : Allow , as decimal separator where Windows regional settings use it.
	* 0.1.4b	2020-04-18		:		CHANGE : Implement pulse guiding.
										BUGFIX : Uninstaller was not correctly removing previous driver DLL
	* 0.1.4.1b	2020-04-18		:		BUGFIX : Driver not correctly handling return value from Halt
	* 0.1.4.2b	2020-04-20		:		CHANGE : Driver uses :CM LX200 Protocol command to sync
	* 0.2.0.0b	2020-04-20		:		CHANGE : Local Server version of driver, first release
										
* Arduino information
	* Tested on Arduino Uno.  No other variants of Arduino have been tested.
	* Currently built for Version V1.6.32 and above of the Arduino Code (As of 2020-04-23)
	* Uncomment #define SUPPORT_SERIAL_CONTROL in Globals.h
	* Leave #define DEBUG_MODE in Globals.h commented.
	
* Supported ASCOM Properties and Methods
	* Documented in included spreadsheet
	* Note : Implementation of a property does not mean it "does stuff", merely that it will return a value instead of an exception.  The value may never change.

* Last Conformance Test - 2020-04-13

* Cautions and warnings
	* CAUTION : Neither the mount nor the driver currently support setting any slew limits.  Thus it is quite possible the mount may be told to slew to a position that causes a crash
				with some gadget you've printed and added, or results in an untennable position for your camera, or any number of other "Bad Things (tm)".  So...keep track of your own towel.

* Known Issues
	* All known current issues are probably documented at https://github.com/ClutchplateDude/OpenAstroTracker/issues 
	
* Work in Progress / Coming soon
	* A simple PC Control application is being developed.
	* Implementation of AltAz methods and properties
	* Ability to set home position of mount via ASCOM

	
