#!/usr/bin/env python3
import curses
import time
import sys
import serial
import re

def clearline(screen, line):
    screen.move(line, 0)
    screen.clrtoeol();

def keypress(screen, altaz, value):
    if value > 0:
        line = f"{altaz} raising by {value}"
    elif value < 0:
        line = f"{altaz} lowering by {value}"
    elif value == 0:
        line = ""
    clearline(screen, 3)
    screen.addstr(3, 0, line)
    return True

def altaz(axis, errorvalue, serialport):
	if not re.match(r"^[-+]?([0-9]*\.[0-9]+|[0-9]+)$", str(errorvalue)):
		print("Error value not valid")
		return
		
	errorvalue = float(errorvalue)
	if axis.lower() == "alt":
		commandToSend = f":MAL{errorvalue}#"
	elif axis.lower() == "az":
		commandToSend = f":MAZ{errorvalue}#"
	else:
		print("Axis input not correct")
		return

	#print(command) #For debugging

	ser = serial.Serial(serialport, 57600, timeout = 1)
	#commandToSend = ':Sr16:00:00#:MS#'
	ser.write(str(commandToSend).encode())
	return

#if sys.argv[1] == "help" or sys.argv[1] == "--help" or sys.argv[1] == "-h" or sys.argv[1] == "?":
#    print("This program will allow joystick-like control of the Alt/Az axis of the OpenAstroTracker using keyboard arrow keys.")
#    print("Usage: sudo joystick.py <serialport of OAT>")

if len(sys.argv) >= 2:
    serialport = sys.argv[1]
else:
    serialport = "/dev/ttyACM0"

#screen = curses.filter()
# get the curses screen window
screen = curses.initscr()

# turn off input echoing
curses.noecho()
 
# respond to keys immediately (don't wait for enter)
curses.cbreak()

# map arrow keys to special values
screen.keypad(True)
screen.nodelay(1)
screen.addstr(0, 0, 'Up/Down arrow keys to raise/lower altitude.')
screen.addstr(1, 0, 'Right/Left arrow keys to increase/decrease azimuth.')
screen.addstr(2, 0, 'Press \'q\' to exit.')

keypress_timeout = time.time()
status_timeout = time.time()
keypress_shown = False
status_shown = False

azimuth = 0
altitude = 0
try:
    while True:
        char = screen.getch()
        if char == ord('q'):
            break
        elif char == curses.KEY_RIGHT:
            keypress_timeout = time.time()
            altitude = 0
            azimuth += 1
            keypress_shown = keypress(screen, "Azimuth", azimuth)
        elif char == curses.KEY_LEFT:
            keypress_timeout = time.time()
            altitude = 0
            azimuth -= 1
            keypress_shown = keypress(screen, "Azimuth", azimuth)        
        elif char == curses.KEY_UP:
            keypress_timeout = time.time()
            azimuth = 0
            altitude += 1
            keypress_shown = keypress(screen, "Altitude", altitude) 
        elif char == curses.KEY_DOWN:
            keypress_timeout = time.time()
            azimuth = 0
            altitude -= 1
            keypress_shown = keypress(screen, "Altitude", altitude)
        if time.time() - keypress_timeout > 1 and keypress_shown:
            keypress_shown = False
            if altitude != 0:
                statusline = f"Adjusting altitude by {altitude} arcminutes"
                screen.addstr(4, 0, statusline)
                status_shown = True
                altaz("alt", altitude, serialport)
            if azimuth != 0:
                statusline = f"Adjusting azimuth by {azimuth} arcminutes"
                screen.addstr(4, 0, statusline)
                status_shown = True
                altaz("az", azimuth, serialport)
            azimuth = 0
            altitude = 0
            status_timeout = time.time()
            clearline(screen, 3)
        if time.time() - status_timeout > 3 and status_shown:
            status_shown = False
            clearline(screen, 4)
finally:
    # shut down cleanly
    curses.nocbreak(); screen.keypad(0); curses.echo()
    curses.endwin()
