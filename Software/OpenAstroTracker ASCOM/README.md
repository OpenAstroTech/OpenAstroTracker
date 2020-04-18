OpenAstroTracker ASCOM driver 0.1.4.1b : 2020-04-18

This driver is still in a great deal of flux.  Capacitor levels of flux.  Use at your own risk!

* History
	* 0.1.3.0	2020-04-15		:		Initial release
	* 0.1.3.1	2020-04-16		:		BUGFIX : Allow , as decimal separator where Windows regional settings use it.
	* 0.1.4b	2020-04-18		:		CHANGE : Implement pulse guiding.
										BUGFIX : Uninstaller was not correctly removing previous driver DLL
	* 0.1.4.1b	2020-04-18		:		BUGFIX : Driver not correctly handling return value from Halt
										
* Arduino information
	* Tested on Arduino Uno.  No other variants of Arduino have been tested.
	* Currently built for Version V1.6.27 of the Arduino Code (As of 2020-04-18)
	* Uncomment #define SUPPORT_SERIAL_CONTROL in Globals.h
	* Leave #define DEBUG_MODE in Globals.h commented.
	
* Supported ASCOM Properties and Methods
	* Documented in included spreadsheet
	* Note : Implementation of a property does not mean it "does stuff", merely that it will return a value instead of an exception.  The value may never change.

* Last Conformance Test - 2020-04-18

* Cautions and warnings
	* WARNING : DO NOT BUILD THIS DRIVER ON THE SAME MACHINE YOU INTEND TO USE IT ON.
		* There is a known issue with a "LoadFromContext Exception" with at least one client software.  This will PREVENT use of the driver in that software if you build on that machine.
		* The issue has been asked about here https://ascomtalk.groups.io/g/Developer/topic/loadfromcontext_exception/72971681?p=,,,20,0,0,0::recentpostdate%2Fsticky,,,20,2,0,72971681
		* issue #21 in ClutchplateDude/OpenAstroTracker on github
	* CAUTION : Neither the mount nor the driver currently support setting any slew limits.  Thus it is quite possible the mount may be told to slew to a position that causes a crash
				with some gadget you've printed and added, or results in an untennable position for your camera, or any number of other "Bad Things (tm)".  So...keep track of your own towel.

* Known Issues
	* All known current issues are probably documented at https://github.com/ClutchplateDude/OpenAstroTracker/issues 
	
* Work in Progress / Coming soon
	* A simple PC Control application has been built during testing.  A release version of this for basic mount control will follow soon.
		* You are welcome to play with the PC Control code included in "OAT PC Control" in this branch, but be aware it's not ready for prime time yet.
	* Implementation of AltAz methods and properties
	* Ability to set home position of mount via ASCOM
	* ASCOM local server wrapper (Need help!)
	