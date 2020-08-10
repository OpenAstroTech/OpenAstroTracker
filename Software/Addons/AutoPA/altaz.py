#!/usr/bin/env python3
import sys
import serial
import re

def altaz(axis, errorvalue, serialport):
	if not re.match(r"^[-+]?([0-9]*\.[0-9]+|[0-9]+)$", errorvalue):
		print("Error value not valid")
		return
		
	errorvalue = float(errorvalue)
	if axis.lower() == "alt":
		print(f"Adjusting Altitude by {errorvalue:.4f} arcminutes.")
		commandToSend = f":MAL{errorvalue}#"
	elif axis.lower() == "az":
		print(f"Adjusting Azimuth by {errorvalue:.4f} arcminutes.")
		commandToSend = f":MAZ{errorvalue}#"
	else:
		print("Axis input not correct")
		return

	#print(command) #For debugging

	ser = serial.Serial(serialport, 57600, timeout = 1)
	#commandToSend = ':Sr16:00:00#:MS#'
	ser.write(str(commandToSend).encode())
	return
	
if len(sys.argv) != 4:
	print("Incorrect number of parameters")
	sys.exit()
		
axis = sys.argv[1]
errorvalue = sys.argv[2]
serialport = sys.argv[3]

altaz(axis, errorvalue, serialport)

