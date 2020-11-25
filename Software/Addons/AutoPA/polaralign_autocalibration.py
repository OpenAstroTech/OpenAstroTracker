#!/usr/bin/env python3
from astropy.coordinates import SkyCoord, EarthLocation, AltAz, Angle
from astropy import units as u
from astropy.time import Time
from astropy.utils import iers
import time
from autopa_modules import indi
from autopa_modules import platesolve
from autopa_modules import LX200
from datetime import datetime
import re, os
import argparse
import statistics

def polarcalibrate(axis, indiclient, iteration, observing_location, telescope, ccd, serialport, radius, exposure, calibration_distance, RA, DEC, backlashCorrection, pixel_resolution):
    #iers.conf.auto_download = False
    #iers.conf.auto_max_age = None
    
    if (iteration % 2) == 0:
        backlashCorrection[0] = backlashCorrection[0] * -1
        backlashCorrection[1] = backlashCorrection[1] * -1
        calibration_distance = calibration_distance * -1
    
    #Initialize steppers to a known backlash position
    initSteppers(axis, indiclient, telescope, serialport, backlashCorrection)
    
    p1RA, p1DEC, p1Time = captureSolve(indiclient, RA, DEC, radius, exposure, telescope, ccd, blobEvent, pixel_resolution, "altazCalibration1.fits")
    p1Time = datetime.utcfromtimestamp(p1Time)
    
    #Disconnect telescope mount from INDI to free up serial port for Alt/Az adjustments
    print (f"Disconnecting {telescope} from INDI server")
    indi.disconnectScope(indiclient, telescope)
    print ("Disconnected.")

    #Adjust alt/az axis
    if axis.lower() == "az":
        print (f"Adjusting azimuth by {calibration_distance} arcminutes")
        LX200.sendCommand(f":MAZ{calibration_distance}#", serialport)
    elif axis.lower() == "alt":
        print (f"Adjusting altitude by {calibration_distance} arcminutes")
        LX200.sendCommand(f":MAL{calibration_distance}#", serialport)
    
    waitForMotors(serialport) #FUTURE - poll the OAT for motor status and automatically resume calibration once they have stopped moving
    #input("When Alt/Az has stopped moving, press Enter to continue.")
    #time.sleep(40)
    
    #Re-connect telescope mount to INDI before disconnecting from the INDI server
    print (f"Reconnecting {telescope} to INDI server")
    indi.connectScope(indiclient, telescope)
    
    p2RA, p2DEC, p2Time = captureSolve(indiclient, RA, DEC, radius, exposure, telescope, ccd, blobEvent, pixel_resolution, "altazCalibration2.fits")
    p2Time = datetime.utcfromtimestamp(p2Time)
    
    p1 = SkyCoord(p1RA, p1DEC, frame='itrs', unit='deg', representation_type='spherical', obstime=Time(p1Time))
    p2 = SkyCoord(p2RA, p2DEC, frame='itrs', unit='deg', representation_type='spherical', obstime=Time(p2Time))
    
    p1AzAlt = p1.transform_to(AltAz(obstime=Time(p1Time),location=observing_location))
    p2AzAlt = p2.transform_to(AltAz(obstime=Time(p1Time),location=observing_location))
    print(f"First capture Az./Alt.: {p1AzAlt.az.to_string(u.hour, precision=2)}/{p1AzAlt.alt.to_string(u.deg, precision=2)}.")
    print(f"Second capture Az./Alt.: {p2AzAlt.az.to_string(u.hour, precision=2)}/{p2AzAlt.alt.to_string(u.deg, precision=2)}.")

    #Calculate delta between solves
    #Normalize the azimuth values to between -180 and 180 degrees prior to determining offset.
    deltaAz = (((p2AzAlt.az.deg + 180) % 360 - 180)-((p1AzAlt.az.deg + 180) % 360 - 180))*60
    deltaAlt = (p2AzAlt.alt.deg-p1AzAlt.alt.deg)*60
    
    return (deltaAz, deltaAlt)

def captureSolve(indiclient, RA, DEC, radius, exposure, telescope, ccd, blobEvent, pixel_resolution, filename="platesolve.fits"):
    original_exposure = exposure
    solved = False
    while not solved:
        print (f"Capturing {exposure} second exposure on \"{ccd}\"")
        captureTime = indi.capture(indiclient, ccd, exposure, filename, blobEvent)
        
        print (f"Attempting plate solving in the region of: RA {Angle(RA*u.deg).to_string(unit=u.hour)}, DEC {Angle(DEC*u.deg).to_string(unit=u.degree)}.")
        solveTimeStart = time.time()
        resultRA, resultDEC = platesolve.solve_ASTAP(filename, RA, DEC, radius, pixel_resolution)
        if resultRA != "Failed" and resultDEC != "Failed":
            solved = True
        else:
            print("Image could not be solved. Attempting another capture with increased exposure time (Max 5 attempts)")
            exposure += 1
            if exposure - original_exposure >= 5:
                print ("Image could not be solved after 5 attempts. Exiting program.")
                exit()
        solveTimeFinish = time.time()
    print (f"Image solved in {(solveTimeFinish - solveTimeStart):.2f} seconds. Coordinates are RA {Angle(resultRA, unit=u.deg).to_string(unit=u.hour)}, DEC {Angle(resultDEC, unit=u.deg).to_string(unit=u.degree)}")

    return (resultRA, resultDEC, captureTime)
    
def initSteppers(axis, indiclient, telescope, serialport, backlashCorrection):

    print ("Initializing steppers")
    #Disconnect telescope mount from INDI to free up serial port for Alt/Az adjustments
    indi.disconnectScope(indiclient, telescope)
    
    #Increase both Az & Alt by approximately 16-steps
    if axis == "az":
        LX200.sendCommand(f":MAZ{backlashCorrection[0]}#", serialport)
    elif axis == "alt":
        LX200.sendCommand(f":MAL{backlashCorrection[1]}#", serialport)
    time.sleep(3)
    
    #Re-connect telescope mount to INDI before disconnecting from the INDI server
    indi.connectScope(indiclient, telescope)
    return

def waitForMotors(serialport):
    statusAz = "z"
    statusAlt = "a"
    startTime = time.time()
    while statusAz != "-" or statusAlt != "-":
        print(f"Polling stepper status from mount. Alt/Az steppers moving for...{round((time.time() - startTime))} seconds.\r", end="")
        status = LX200.sendCommand(":GX#\n", serialport)
        search = re.match(r"^[^,]*,([^,]*),", str(status))
        statusAz = list(search.group(1))[3]
        statusAlt = list(search.group(1))[4]
        #print (f"Azimuth motor status: {statusAz}")
        #print (f"Altitude motor status: {statusAlt}")
    print(f"Polling stepper status from mount. Alt/Az steppers moving for...{round((time.time() - startTime))} seconds.")
    print(f"Finished moving.")
    time.sleep(1) #Wait for steppers momentum to stop moving
    return

parser = argparse.ArgumentParser(usage='%(prog)s [axis] [mylat] [mylong] [myelev] [options]', description='OpenAstroTracker AutoPA Calibration: \
        This tool is used to automatically take two exposures a specified Alt/Az distance apart in arcminutes,\
        plate solve, then calculate the actual distance moved. This distance relative to the specified travel distance will give a correction ratio for the Alt/Az')
parser.add_argument("axis", help="Which axis to calibrate (\"alt\" or \"az\")", type=str)
parser.add_argument("mylat", help="Your latitude in degrees", type=float)
parser.add_argument("mylong", help="Your longitude in degrees", type=float)
parser.add_argument("myelev", help="Your elevation in metres", type=float)
parser.add_argument("--serialport", help="Serial port address for the OAT (default is /dev/ttyACM0)", type=str)
parser.add_argument("--existRA", help="Initial starting RA in degrees (default is 0)", type=float)
parser.add_argument("--existDEC", help="Initial starting DEC in degrees (default is 85)", type=float)
parser.add_argument("--exposure", help="Exposure time in seconds (default is 8 seconds)", type=float)
parser.add_argument("--telescope", help="Name of INDI telescope to control movement (default is Ekos \"Telescope Simulator\")", type=str)
parser.add_argument("--ccd", help="Name of INDI CCD for capturing exposures (default is Ekos \"CCD Simulator\")", type=str)
parser.add_argument("--radius", help="Field radius of plate solving in degrees (default is 30)", type=float)
parser.add_argument("--pixelSize", help="CCD pixel size in micrometres. Used to decrease plate solving time.", type=float)
parser.add_argument("--focalLength", help="Lens focal length in millimetres. Used to decrease plate solving time.", type=float)
parser.add_argument("--calibration_distance", help="Distance the axis moves for calibration", type=float)
args = parser.parse_args()
axis = args.axis
mylat = args.mylat
mylong = args.mylong
myelev = args.myelev
if args.serialport:
    serialport = args.serialport
else:
	serialport = "/dev/ttyACM0"
if args.existRA:
    existRA = args.existRA
else:
	existRA = "-"
if args.existDEC:
    existDEC = args.existDEC
else:
	existDEC = "-"
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
if args.calibration_distance:
    calibration_distance = args.calibration_distance
else:
	calibration_distance = 120
if args.pixelSize and args.focalLength:
    pixel_resolution = (args.pixelSize/args.focalLength)*206.265
else:
	pixel_resolution = 0

#Backlash correction in arcminutes (approximately 16 full steps of each axis)
if calibration_distance > 0:
    backlashCorrection = [4.2665,0.1647]
else:
    backlashCorrection = [-4.2665,-0.1647]

iteration = input("How many calibration iterations to perform? (Default is 4) ")
try:
    iteration = int(iteration)
except:
    iteration = 4

#Create location object based on lat/long/elev
observing_location = EarthLocation(lat=mylat*u.deg, lon=mylong*u.deg, height=myelev*u.m)

#Connect to INDI server
indiclient, blobEvent = indi.indiserverConnect()

#Get existing telescope direction from INDI
if (existRA == "-" and existDEC == "-"):
    prop = indi.getProp(indiclient, telescope, "Number", "EQUATORIAL_EOD_COORD")
    existRA = prop[0].value*15
    existDEC = prop[1].value
print (f"Telescope \"{telescope}\" currently at RA {Angle(existRA*u.deg).to_string(unit=u.hour)}, DEC {Angle(existDEC*u.deg).to_string(unit=u.degree)}")

#Perform calibration iteration(s)
deltaAz=[]
deltaAlt=[]
for i in range(1, iteration+1):
    if iteration > 1:
        print (f"Starting calibration iteration {i}...")
    tempAz, tempAlt = polarcalibrate(axis, indiclient, i, observing_location, telescope, ccd, serialport, radius, exposure, calibration_distance, existRA, existDEC, backlashCorrection, pixel_resolution)
    deltaAz.append(abs(tempAz))
    deltaAlt.append(abs(tempAlt))

#DIsconnect from INDI server        
indi.indiserverDisconnect(indiclient)

print(f"List of azimuth deltas: {deltaAz}")
print(f"List of altitude deltas: {deltaAlt}")

#Calculate averages of each change in movement
avgDeltaAz = sum(deltaAz) / len(deltaAz)
avgDeltaAlt = sum(deltaAlt) / len(deltaAlt)

#Adjust alt/az axis
if axis.lower() == "az":
    print(f"Average azimuth delta is: {avgDeltaAz:.4f} arcminutes. Expected value: {calibration_distance} arcminutes.")
    print(f"Azimuth arcseconds per step is off by a factor of: {avgDeltaAz/calibration_distance} arcminutes.")
    print(f"Average altitude delta is: {avgDeltaAlt:.4f} arcminutes. If this number is not near zero, it means the azimuth plane is not level and the above factor is likely not accurate.")
    if iteration > 1:
        print(f"Standard deviation of measured azimuth deltas is {statistics.stdev(deltaAz)} arcminutes.")
elif axis.lower() == "alt":
    print(f"Average altitude delta is: {avgDeltaAlt:.4f} arcminutes. Expected value: {calibration_distance} arcminutes.")
    print(f"Altitude arcseconds per step is off by a factor of: {avgDeltaAlt/calibration_distance}.")
    print(f"Average azimuth delta is: {avgDeltaAz:.4f} arcminutes. If this number is not near zero, it means the azimuth plane is not level and the above factor is likely is not accurate.")
    if iteration > 1:
        print(f"Standard deviation of measured altitude deltas is {round(statistics.stdev(deltaAlt), 4)} arcminutes.")