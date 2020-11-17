#!/usr/bin/env python3

from autopa_modules import platesolve
from autopa_modules import indi
from autopa_modules import LX200
import argparse
import subprocess

def adjustAltAz(result, serialport, backlashCorrection):
    #Verify error correction values can be handled by AutoPA hardware (assuming it is in home/centered position)
    moveAz = "N"
    if abs(result[0]) > 120:
        moveAz = input("Azimuth error may be out of bounds of hardware capabilities if not in home position. Continue? (Y/N): ") 
    else:
        moveAz = "Y"
    if moveAz.upper() == "Y":
        if result[0] < 0:
            correction = result[0] - backlashCorrection[0]
        reply = LX200.sendCommand(f":MAL{correction}#", serialport)

    moveAlt = "N"
    if result[1] > 168:
        moveAz = input("Altitude error may be out of bounds of hardware capabilities if not in home position. Continue? (Y/N): ")
    elif result[1] > 432:
        moveAz = input("Altitude error may be out of bounds of hardware capabilities if not in home position. Continue? (Y/N): ")
    else:
        moveAlt = "Y"
    if moveAlt.upper() == "Y":
        if result[1] < 0:
            correction = result[1] - backlashCorrection[1]
        reply = LX200.sendCommand(f":MAZ{correction}#", serialport)
    return

parser = argparse.ArgumentParser(usage='%(prog)s [mylat] [mylong] [myelev] [time]', description='OpenAstroTracker AutoPA Manual Calculation: \
        This tool will output the altitude/azimuth difference between the current alignment and polar alignment.\
        The input RA/DEC values are determined by manually capturing three photos 30 degrees apart on the RA axis and plate solving them.')
parser.add_argument("mylat", help="Your latitude in degrees", type=float)
parser.add_argument("mylong", help="Your longitude in degrees", type=float)
parser.add_argument("myelev", help="Your elevation in metres", type=float)
parser.add_argument("time", help="The time the images were captured (In YYYY-MM-DD HH:MM:SS UTC format.", type=str)
parser.add_argument("p1RA", help="First point RA (in degrees)", type=float)
parser.add_argument("p1DEC", help="First point DEC (in degrees)", type=float)
parser.add_argument("p2RA", help="Second point RA (in degrees)", type=float)
parser.add_argument("p2DEC", help="Second point DEC (in degrees)", type=float)
parser.add_argument("p3RA", help="Third point RA (in degrees)", type=float)
parser.add_argument("p3DEC", help="Third point DEC (in degrees)", type=float)
parser.add_argument("--serialport", help="Serial port address for the OAT (default is /dev/ttyACM0)", type=str)
parser.add_argument("--telescope", help="Name of INDI telescope (default assumes INDI is not running)", type=str)
args = parser.parse_args()
mylat = args.mylat
mylong = args.mylong
myelev = args.myelev
time = args.time
p1RA = args.p1RA
p1DEC = args.p1DEC
p2RA = args.p2RA
p2DEC = args.p2DEC
p3RA = args.p3RA
p3DEC = args.p3DEC
if args.serialport:
    serialport = args.serialport
else:
	serialport = "/dev/ttyACM0"
if args.telescope:
    telescope = args.telescope
else:
	telescope = ""

#Calculate polar alignment error
result = platesolve.polarCalc(mylat, mylong, myelev, time, p1RA, p1DEC, p2RA, p2DEC, p3RA, p3DEC)
print(f"Azimuth error correction is: {result[0]:.4f} arcminutes.")
print(f"Altitude error correction is: {result[1]:.4f} arcminutes.")

print("Note: Stepper backlash has not been accounted for and may cause up to 4 arcminutes and 0.2 arcminutes error in azimuth and altitude respectively.")
if input("Adjust the OAT altitude/azimuth motors? (Y/N): ").upper() != "Y":
    exit()

#Backlash correction in arcminutes (approximately 16 full steps of each axis)
backlashCorrection = (0,0)

if telescope != "":
    #Connect to indi server
    indiclient, blobEvent = indi.indiserverConnect()

    #Disconnect telescope mount from INDI to free up serial port for Alt/Az adjustments
    print (f"Disconnecting {telescope} from INDI server")
    indi.disconnectScope(indiclient, telescope)
    print ("Disconnected.")

try:
    #Adjust alt/az axis
    print ("Adjusting altitude/azimuth axes.")
    platesolve.adjustAltAz(result, serialport, backlashCorrection)
except Exception as err:
    print (err)
    print ("Error accessing serial port. If INDI is running, please specify the telescope name to disconnect INDI from the device.")

if telescope != "":
    #Re-connect telescope mount to INDI before disconnecting from the INDI server
    indi.connectScope(indiclient, telescope)

    #Disconnect from indi server
    indi.indiserverDisconnect(indiclient)