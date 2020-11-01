#!/usr/bin/env python3

import subprocess
import re, math, os, time
from pathlib import Path
from astropy.time import Time
from astropy.coordinates import SkyCoord, EarthLocation, AltAz, Angle
from astropy import units as u
from astropy.wcs import WCS
from autopa_modules import LX200
import contextlib

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

def solve_ASTAP (filename, RA, DEC, radius, pixel_resolution):
    cmd = f"/opt/astap/astap -r 30 -speed auto -o /tmp/solution -f {filename}"    
    try:
        output = str(subprocess.check_output(cmd, shell=True))
        output = WCS("/tmp/solution.wcs")
        object = SkyCoord(output.wcs.crval[0], output.wcs.crval[1], frame='fk5', unit='deg')
    except:
        return ("Failed")
    return (object)

def solve_ASTROMETRY (filename, RA, DEC, radius, pixel_resolution):
    config_file="/home/astroberry/.local/share/kstars/astrometry/astrometry.cfg"
    disable_output_files=f"-B none -W none -S none -N none -M none -R none --temp-axy -U /tmp/{Path(filename).with_suffix('')}.xyls"
    cmd = f"/usr/bin/solve-field --config {config_file} {disable_output_files} -O --no-plots --no-verify --resort --downsample 2 -3 {RA} -4 {DEC} -5 {radius} {filename}"
    if pixel_resolution != 0:
        cmd = f"{cmd} {filename} --scale-low={pixel_resolution*0.75} --scale-high={pixel_resolution*1.25} --scale-units=\"arcsecperpix\""
    cmd = f"{cmd} {filename}"
    output = str(subprocess.check_output(cmd, shell=True))
    time.sleep(3)
    result = re.search(r"Field center: \(RA H:M:S, Dec D:M:S\) = \((?P<RA_H>\d+):(?P<RA_M>\d+):(?P<RA_S>\d+\.\d+), (?P<DEC_D>[+-]?\d+):(?P<DEC_M>\d+):(?P<DEC_S>\d+\.\d+)\)", output)
    if result is None:
        return ("Failed", "Failed")
    else:
        RA_H = result.group("RA_H")
        RA_M = result.group("RA_M")
        RA_S = result.group("RA_S")
        DEC_D = result.group("DEC_D")
        DEC_M = result.group("DEC_M")
        DEC_S = result.group("DEC_S")
        return (f"{RA_H}h{RA_M}m{RA_S}s", f"{DEC_D}d{DEC_M}m{DEC_S}s")
        
def polarCalc(mylat, mylong, myelev, observing_time, p1, p2, p3):
    #iers.conf.auto_download = False
    #iers.conf.auto_max_age = None
    
    #Create time object based on given time
    observing_time = Time(observing_time)

    #Create location object based on lat/long/elev
    observing_location = EarthLocation(lat=mylat*u.deg, lon=mylong*u.deg, height=myelev*u.m)

    #Create coordinate objects for each point
    #p1 = SkyCoord(p1RA, p1DEC, unit='deg')
    #p2 = SkyCoord(p2RA, p2DEC, unit='deg')
    #p3 = SkyCoord(p3RA, p3DEC, unit='deg')
    p1X = (90 - p1.dec.degree) * math.cos(p1.ra.radian)
    p1Y = (90 - p1.dec.degree) * math.sin(p1.ra.radian)
    p2X = (90 - p2.dec.degree) * math.cos(p2.ra.radian)
    p2Y = (90 - p2.dec.degree) * math.sin(p2.ra.radian)
    p3X = (90 - p3.dec.degree) * math.cos(p3.ra.radian)
    p3Y = (90 - p3.dec.degree) * math.sin(p3.ra.radian)

    #Calculate center of circle using three points in the complex plane. RA/DEC are treated as unitless for the purposes of the calculation.
    x, y, z = complex(p1X,p1Y), complex(p2X,p2Y), complex(p3X,p3Y)
    w = z-x
    w /= y-x
    c = (x-y)*(w-abs(w)**2)/2j/w.imag-x
    resultX = -c.real
    resultY = -c.imag

    #Convert X/Y values of circle into RA/DEC
    resultDEC = (90 - math.sqrt(resultX**2 + resultY**2))
    resultRA = math.atan2(resultY, resultX)*360 / (2*math.pi)
    if resultRA < 0:
        resultRA = (180-abs(resultRA))+180

    print(f"Current alignment in RA/DEC: {Angle(resultRA*u.deg).to_string(u.hour, precision=2)}/{Angle(resultDEC*u.deg).to_string(u.degree, precision=2)}.")
    
    #Create alt/az coordinate object for pole
    poleAzAlt = RADECtoAltAz(observing_location, observing_time, 0, 90)
    print(f"True polar alignment in Az./Alt.: 00h00m00s/{poleAzAlt.alt.to_string(u.degree, precision=2)}.")
    
    #Create alt/az coordinate object for current alignment
    offsetAzAlt = RADECtoAltAz(observing_location, observing_time, resultRA, resultDEC)
    print(f"Current alignment in Az./Alt.: {offsetAzAlt.az.to_string(u.hour, precision=2)}/{offsetAzAlt.alt.to_string(u.degree, precision=2)}.")

    #Calculate offset deltas from pole
    #Normalize the azimuth values to between -180 and 180 degrees prior to determining offset.
    errorAz = (((poleAzAlt.az.deg + 180) % 360 - 180)-((offsetAzAlt.az.deg + 180) % 360 - 180))*60
    errorAlt = (poleAzAlt.alt.deg-offsetAzAlt.alt.deg)*60
    
    return errorAz, errorAlt
    
def RADECtoAltAz(observing_location, observing_time, RA, DEC):
    #Create coordinate object for current alignment offset
    offset = SkyCoord(RA, DEC, frame='itrs', unit='deg', representation_type='spherical', obstime=observing_time)
    
    #Transform current alignment to Alt/Az coordinate system
    AzAlt = offset.transform_to(AltAz(obstime=observing_time,location=observing_location))

    return AzAlt