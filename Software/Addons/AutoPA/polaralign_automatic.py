#!/usr/bin/env python3
from astropy import units as u
from astropy.coordinates import SkyCoord, EarthLocation, AltAz, Angle
from astropy.time import Time
from astropy.utils import iers
import sys, subprocess, time, math
from autopa_modules import indi
from autopa_modules import platesolve
from autopa_modules import LX200
from datetime import datetime
from statistics import mean
import argparse

def slewCaptureSolve(indiclient, RA, DEC, radius, exposure, telescope, ccd, blobEvent, pixel_resolution, filename="platesolve.fits"):
    print (f"Slewing \"{telescope}\" to {Angle(RA*u.deg).to_string(unit=u.hour)}, {Angle(DEC*u.deg).to_string(unit=u.degree)}")
    indi.slewSync(indiclient, telescope, RA, DEC)
    #Wait one second to let the telescope stop moving and let any motor vibrations reduce
    time.sleep(1)
    
    original_exposure = exposure
    solved = False
    while not solved:
        print (f"Capturing {exposure} second exposure on \"{ccd}\"")
        captureTime = indi.capture(indiclient, ccd, exposure, filename, blobEvent)
        
        print (f"Attempting plate solving in the region of: RA {Angle(RA*u.deg).to_string(unit=u.hour)}, DEC {Angle(DEC*u.deg).to_string(unit=u.degree)}.")
        solveTimeStart = time.time()
        #resultRA, resultDEC = platesolve.solve_ASTROMETRY(filename, RA, DEC, radius, pixel_resolution)
        result = platesolve.solve_ASTAP(filename, RA, DEC, radius, pixel_resolution)
        if not isinstance(result, str):
            solved = True
        else:
            print("Image could not be solved. Attempting another capture with increased exposure time (Max is 10 second exposure)")
            if exposure < 10:
                exposure += 1
            elif exposure - original_exposure >= 5:
                print ("Image could not be solved. Exiting.")
        solveTimeFinish = time.time()
    print (f"Image solved in {(solveTimeFinish - solveTimeStart):.2f} seconds.")
    #print (f"Image solved. Coordinates are RA {Angle(resultRA, unit=u.deg).to_string(unit=u.hour)}, DEC {Angle(resultDEC, unit=u.deg).to_string(unit=u.degree)}")

    return (result, captureTime)
    
def initSteppers(indiclient, telescope, serialport, backlashCorrection):

    print ("Initializing steppers")
    #Disconnect telescope mount from INDI to free up serial port for Alt/Az adjustments
    indi.disconnectScope(indiclient, telescope)
    
    #Increase both Az & Alt by approximately 16-steps
    LX200.sendCommand(f":MAZ{backlashCorrection[0]}#", serialport)
    LX200.sendCommand(f":MAL{backlashCorrection[1]}#", serialport)

    time.sleep(3)
    
    #Re-connect telescope mount to INDI before disconnecting from the INDI server
    indi.connectScope(indiclient, telescope)
    return

parser = argparse.ArgumentParser(usage='%(prog)s [mylat] [mylong] [myelev] [options]', description='OpenAstroTracker AutoPA: This tool is used to automatically rotate the mount, capture images,\
        plate solve, and calculate the polar alignment error of the OAT. Serial commands are automatically issued to the OAT to adjust the motorized altitude/azimuth axis to correct this error.')
parser.add_argument("mylat", help="your latitude in degrees", type=float)
parser.add_argument("mylong", help="your longitude in degrees", type=float)
parser.add_argument("myelev", help="your elevation in metres", type=float)
parser.add_argument("--serialport", help="serial port address for the OAT (default is /dev/ttyACM0)", type=str)
parser.add_argument("--targetRA", help="initial starting RA in degrees (default is 0)", type=float)
parser.add_argument("--targetDEC", help="initial starting DEC in degrees (default is 85)", type=float)
parser.add_argument("--exposure", help="exposure time in seconds (default is 8 seconds)", type=float)
parser.add_argument("--telescope", help="name of INDI telescope to control movement (default is Ekos \"Telescope Simulator\")", type=str)
parser.add_argument("--ccd", help="name of INDI CCD for capturing exposures (default is Ekos \"CCD Simulator\")", type=str)
parser.add_argument("--radius", help="field radius of plate solving", type=float)
parser.add_argument("--pixelSize", help="CCD pixel size in micrometres. Used to decrease plate solving time.", type=float)
parser.add_argument("--pixelX", help="Quantity of pixels in the X direction.", type=float)
parser.add_argument("--pixelY", help="Quantity of pixels in the Y direction.", type=float)
parser.add_argument("--focalLength", help="Lens focal length in millimetres. Used to decrease plate solving time.", type=float)
parser.add_argument("--nomove", help="Run AutoPA sequence but do not move the steppers.", action="store_true")
args = parser.parse_args()
mylat = args.mylat
mylong = args.mylong
myelev = args.myelev
if args.serialport:
    serialport = args.serialport
else:
	serialport = "/dev/ttyACM0"
if args.targetRA:
    targetRA = args.targetRA
else:
	targetRA = 0
if args.targetDEC:
    targetDEC = args.targetDEC
else:
	targetDEC = 85
if args.exposure:
    exposure = args.exposure
else:
	exposure = 8.0
if args.telescope:
    telescope = args.telescope
else:
	telescope = "Telescope Simulator"
if args.ccd:
    ccd = args.ccd
else:
	ccd = "CCD Simulator"
if args.radius:
    radius = args.radius
else:
	radius = 30
if args.pixelSize and args.focalLength:
    pixel_resolution = (args.pixelSize/args.focalLength)*206.265
else:
	pixel_resolution = 0
if args.nomove:
    nomove = True
else:
	nomove = False
if args.pixelX and args.pixelY:
    pixelX = args.pixelX
    pixelY = args.pixelY
    if pixel_resolution != 0:
        radius = (max(pixelX, pixelY) * pixel_resolution) / 3600 * 1.1 #Calculate FOV and double for range of plate solving
else:
    pixelX = 0
    pixelY = 0

startTime = time.time()
solveTimeFinish = time.time()

#Backlash correction in arcminutes (approximately 16 full steps of each axis)
backlashCorrection = [4.2665,0.1647]

#Connect to indi server
indiclient, blobEvent = indi.indiserverConnect()

if not nomove:
    #Initialize steppers to a known backlash position
    initSteppers(indiclient, telescope, serialport, backlashCorrection)

#Execute polar alignment routine to capture and solve three times 30 degrees apart
p1, p1Time = slewCaptureSolve(indiclient, (targetRA % 360), targetDEC, radius, exposure, telescope, ccd, blobEvent, pixel_resolution, "capture1.fits")
p2, p2Time = slewCaptureSolve(indiclient, ((targetRA+30) % 360), targetDEC, radius, exposure, telescope, ccd, blobEvent, pixel_resolution, "capture2.fits")
p3, p3Time = slewCaptureSolve(indiclient, ((targetRA+60) % 360), targetDEC, radius, exposure, telescope, ccd, blobEvent, pixel_resolution, "capture3.fits")

#Calculate capture time based on the average timestamps of each image
observing_time = datetime.utcfromtimestamp(mean((p1Time, p2Time, p3Time)))
print (f"Time of captures is {observing_time} (UTC).")

#Calculate polar alignment error
result = platesolve.polarCalc(mylat, mylong, myelev, observing_time, p1, p2, p3)
print(f"Azimuth error correction is: {result[0]:.4f} arcminutes.")
print(f"Altitude error correction is: {result[1]:.4f} arcminutes.")

if not nomove:
    #Disconnect telescope mount from INDI to free up serial port for Alt/Az adjustments
    print (f"Disconnecting {telescope} from INDI server")
    indi.disconnectScope(indiclient, telescope)
    print ("Disconnected.")

    #Adjust alt/az axis
    print ("Adjusting altitude/azimuth axes.")
    platesolve.adjustAltAz(result, serialport, backlashCorrection)

    #Re-connect telescope mount to INDI before disconnecting from the INDI server
    indi.connectScope(indiclient, telescope)

#Disconnect from indi server
indi.indiserverDisconnect(indiclient)

print (f"Polar alignment took {time.strftime('%Mm%Ss', time.gmtime(time.time() - startTime))}.")