#!/usr/bin/env python3
import math
from astropy.coordinates import SkyCoord, EarthLocation, AltAz, Angle
from astropy import units as u
from astropy.time import Time
from astropy.utils import iers
import sys, subprocess
import argparse

def polarcalibrate(mylat, mylong, myelev):
	#iers.conf.auto_download = False
	#iers.conf.auto_max_age = None

	#Create location object based on lat/long/elev
	observing_location = EarthLocation(lat=mylat*u.deg, lon=mylong*u.deg, height=myelev*u.m)

	p1RA = input("Enter first RA value in 00h00m00.0s format: ")
	p1DEC = input("Enter first DEC value in 00d00m00.0s format: ")
	p1Time = Time(input("Enter first Time value in YYYY-MM-DD HH:MM:SS format: "))
	p2RA = input("Enter second RA value in 00h00m00.0s format: ")
	p2DEC = input("Enter second DEC value in 00d00m00.0s format: ")
	p2Time = Time(input("Enter second Time value in YYYY-MM-DD HH:MM:SS format: "))

	p1 = SkyCoord(p1RA, p1DEC, frame='itrs', unit='deg', representation_type='spherical', obstime=Time(p1Time))
	p2 = SkyCoord(p2RA, p2DEC, frame='itrs', unit='deg', representation_type='spherical', obstime=Time(p2Time))
	
	p1AzAlt = p1.transform_to(AltAz(obstime=p1Time,location=observing_location))
	p2AzAlt = p2.transform_to(AltAz(obstime=p2Time,location=observing_location))
	print(f"First capture Az./Alt.: {p1AzAlt.az.to_string(u.hour, precision=2)}/{p1AzAlt.alt.to_string(u.deg, precision=2)}.")
	print(f"Second capture Az./Alt.: {p2AzAlt.az.to_string(u.hour, precision=2)}/{p2AzAlt.alt.to_string(u.deg, precision=2)}.")

	if p1AzAlt.az.deg > 180:
		p1Az = (360-p1AzAlt.az.deg)*60
	else:
		p1Az = p1AzAlt.az.deg*60
	if p2AzAlt.az.deg > 180:
		p2Az = (360-p2AzAlt.az.deg)*60
	else:
		p2Az = p2AzAlt.az.deg*60
		
	deltaAz = abs(p2Az - p1Az)
	deltaAlt = abs(p2AzAlt.alt.arcmin - p1AzAlt.alt.arcmin)
	
	print(f"Azimuth delta is: {deltaAz:.4f} arcminutes.")
	print(f"Altitude delta is: {deltaAlt:.4f} arcminutes.")
	
	return

parser = argparse.ArgumentParser(usage='%(prog)s [mylat] [mylong] [myelev]', description='OpenAstroTracker AutoPA Manual Calibration: \
        This tool is used to measure the alt/az distance between RA/DEC values (Entered manually. \
        Values can be obtained via uploading two images to https://nova.astrometry.net/upload).\
        It can be used to calibrate the arcminutes/step values by comparing the actual measured movement compared to any movement made using the LCD controls.')
parser.add_argument("mylat", help="Your latitude in degrees", type=float)
parser.add_argument("mylong", help="Your longitude in degrees", type=float)
parser.add_argument("myelev", help="Your elevation in metres", type=float)
args = parser.parse_args()
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

polarcalibrate(mylat, mylong, myelev)