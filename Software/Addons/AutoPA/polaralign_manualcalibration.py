#!/usr/bin/env python3
from astropy.coordinates import SkyCoord, EarthLocation, AltAz
from astropy import units as u
from astropy.time import Time
from astropy.utils import iers
import argparse

def polarcalibrate(mylat, mylong, myelev):
    #iers.conf.auto_download = False
    #iers.conf.auto_max_age = None

    #Create location object based on lat/long/elev
    observing_location = EarthLocation(lat=mylat*u.deg, lon=mylong*u.deg, height=myelev*u.m)

    p1RA = input("Enter first RA value in 00h00m00.0s format: ")
    p1DEC = input("Enter first DEC value in 00d00m00.0s format: ")
    p1Time = input("Enter first Time value in YYYY-MM-DD HH:MM:SS format: ")
    p2RA = input("Enter second RA value in 00h00m00.0s format: ")
    p2DEC = input("Enter second DEC value in 00d00m00.0s format: ")
    p2Time = input("Enter second Time value in YYYY-MM-DD HH:MM:SS format: ")

    p1 = SkyCoord(p1RA, p1DEC, frame='itrs', unit='deg', representation_type='spherical', obstime=Time(p1Time))
    p2 = SkyCoord(p2RA, p2DEC, frame='itrs', unit='deg', representation_type='spherical', obstime=Time(p2Time))
    
    p1AzAlt = p1.transform_to(AltAz(obstime=Time(p1Time),location=observing_location))
    p2AzAlt = p2.transform_to(AltAz(obstime=Time(p2Time),location=observing_location))
    print(f"First capture Az./Alt.: {p1AzAlt.az.to_string(u.hour, precision=2)}/{p1AzAlt.alt.to_string(u.deg, precision=2)}.")
    print(f"Second capture Az./Alt.: {p2AzAlt.az.to_string(u.hour, precision=2)}/{p2AzAlt.alt.to_string(u.deg, precision=2)}.")

    #Calculate delta between solves
    #Normalize the azimuth values to between -180 and 180 degrees prior to determining offset.
    deltaAz = (((p2AzAlt.az.deg + 180) % 360 - 180)-((p1AzAlt.az.deg + 180) % 360 - 180))*60
    deltaAlt = (p2AzAlt.alt.deg-p1AzAlt.alt.deg)*60
    
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

polarcalibrate(mylat, mylong, myelev)