#!/usr/bin/env python3

import PyIndi
import time
import threading
import io
from astropy.coordinates import Angle
from astropy import units as u

class IndiClient(PyIndi.BaseClient):
    def __init__(self):
        super(IndiClient, self).__init__()
    def newDevice(self, d):
        pass
    def newProperty(self, p):
        pass
    def removeProperty(self, p):
        pass
    def newBLOB(self, bp):
        global blobEvent
        blobEvent.set()
        pass
    def newSwitch(self, svp):
        pass
    def newNumber(self, nvp):
        pass
    def newText(self, tvp):
        pass
    def newLight(self, lvp):
        pass
    def newMessage(self, d, m):
        pass
    def serverConnected(self):
        pass
    def serverDisconnected(self, code):
        pass

def capture(indiclient, ccd, exposure, outputfile, blobEvent):
    # Let's take some pictures
    device_ccd=indiclient.getDevice(ccd)
    while not(device_ccd):
        time.sleep(0.5)
        device_ccd=indiclient.getDevice(ccd)   
     
    ccd_connect=device_ccd.getSwitch("CONNECTION")
    while not(ccd_connect):
        time.sleep(0.5)
        ccd_connect=device_ccd.getSwitch("CONNECTION")
    if not(device_ccd.isConnected()):
        ccd_connect[0].s=PyIndi.ISS_ON  # the "CONNECT" switch
        ccd_connect[1].s=PyIndi.ISS_OFF # the "DISCONNECT" switch
        indiclient.sendNewSwitch(ccd_connect)
     
    ccd_exposure=device_ccd.getNumber("CCD_EXPOSURE")
    while not(ccd_exposure):
        time.sleep(0.5)
        ccd_exposure=device_ccd.getNumber("CCD_EXPOSURE")

    # Ensure the CCD simulator snoops the telescope simulator
    # otherwise you may not have a picture of vega
    ccd_active_devices=device_ccd.getText("ACTIVE_DEVICES")
    while not(ccd_active_devices):
        time.sleep(0.5)
        ccd_active_devices=device_ccd.getText("ACTIVE_DEVICES")
    ccd_active_devices[0].text="Telescope Simulator"
    indiclient.sendNewText(ccd_active_devices)
     
    # we should inform the indi server that we want to receive the
    # "CCD1" blob from this device
    indiclient.setBLOBMode(PyIndi.B_ALSO, ccd, "CCD1")
     
    ccd_ccd1=device_ccd.getBLOB("CCD1")
    while not(ccd_ccd1):
        time.sleep(0.5)
        ccd_ccd1=device_ccd.getBLOB("CCD1")
     
    # we use here the threading.Event facility of Python
    # we define an event for newBlob event
    blobEvent.clear()
    ccd_exposure[0].value=exposure
    indiclient.sendNewNumber(ccd_exposure)
    blobEvent.wait()
    captureTime = time.time()
    
    for blob in ccd_ccd1:
        print(f"Received image data from {ccd}. {round((float(blob.size) / 1048576), 2)} MB of data received in {blob.format} format.")
        # pyindi-client adds a getblobdata() method to IBLOB item
        # for accessing the contents of the blob, which is a bytearray in Python
        fits=blob.getblobdata()
        blobfile = io.BytesIO(fits)
        # open a file and save buffer to disk
        with open(outputfile, "wb") as f:
            f.write(blobfile.getvalue())
    return (captureTime)

def getProp(indiclient, device, propType, propName):
    indidevice=indiclient.getDevice(device)
    while not(indidevice):
        time.sleep(0.5)
        indidevice=indiclient.getDevice(device)
    if propType.lower() == "switch":
        result = indidevice.getSwitch(propName)
        while not(result):
            time.sleep(0.5)
            result = indidevice.getSwitch(propName)
    elif propType.lower() == "number":
        result = indidevice.getNumber(propName)
        while not(result):
            time.sleep(0.5)
            result = indidevice.getNumber(propName)
    elif propType.lower() == "text":
        result = indidevice.getText(propName)
        while not(result):
            time.sleep(0.5)
            result = indidevice.getText(propName)
    return result

def disconnectScope(indiclient, telescope):
    device_telescope=indiclient.getDevice(telescope)
    while not(device_telescope):
        time.sleep(0.5)
        device_telescope=indiclient.getDevice(telescope)
    
    # wait CONNECTION property be defined for telescope
    telescope_connect=device_telescope.getSwitch("CONNECTION")
    while not(telescope_connect):
        time.sleep(0.5)
        telescope_connect=device_telescope.getSwitch("CONNECTION")
     
    # if the telescope device is not connected, we do connect it
    while device_telescope.isConnected():
        # Property vectors are mapped to iterable Python objects
        # Hence we can access each element of the vector using Python indexing
        # each element of the "CONNECTION" vector is a ISwitch
        telescope_connect[1].s=PyIndi.ISS_ON  # the "CONNECT" switch
        telescope_connect[0].s=PyIndi.ISS_OFF # the "DISCONNECT" switch
        indiclient.sendNewSwitch(telescope_connect) # send this new value to the device
    time.sleep(1)

def connectScope(indiclient, telescope):
    device_telescope=indiclient.getDevice(telescope)
    while not(device_telescope):
        time.sleep(0.5)
        device_telescope=indiclient.getDevice(telescope)
    
    # wait CONNECTION property be defined for telescope
    telescope_connect=device_telescope.getSwitch("CONNECTION")
    while not(telescope_connect):
        time.sleep(0.5)
        telescope_connect=device_telescope.getSwitch("CONNECTION")
     
    # if the telescope device is not connected, we do connect it
    if not(device_telescope.isConnected()):
        # Property vectors are mapped to iterable Python objects
        # Hence we can access each element of the vector using Python indexing
        # each element of the "CONNECTION" vector is a ISwitch
        telescope_connect[0].s=PyIndi.ISS_ON  # the "CONNECT" switch
        telescope_connect[1].s=PyIndi.ISS_OFF # the "DISCONNECT" switch
        indiclient.sendNewSwitch(telescope_connect) # send this new value to the device
        
def slewSync(indiclient, telescope, RA, DEC, sync=False):
    # get the telescope device
    device_telescope=indiclient.getDevice(telescope)
    while not(device_telescope):
        time.sleep(0.5)
        device_telescope=indiclient.getDevice(telescope)
         
    # wait CONNECTION property be defined for telescope
    telescope_connect=device_telescope.getSwitch("CONNECTION")
    while not(telescope_connect):
        time.sleep(0.5)
        telescope_connect=device_telescope.getSwitch("CONNECTION")
     
    # if the telescope device is not connected, we do connect it
    if not(device_telescope.isConnected()):
        # Property vectors are mapped to iterable Python objects
        # Hence we can access each element of the vector using Python indexing
        # each element of the "CONNECTION" vector is a ISwitch
        telescope_connect[0].s=PyIndi.ISS_ON  # the "CONNECT" switch
        telescope_connect[1].s=PyIndi.ISS_OFF # the "DISCONNECT" switch
        indiclient.sendNewSwitch(telescope_connect) # send this new value to the device
          
    # Now let's make a goto to the target
    # Beware that ra/dec are in decimal hours/degrees
    target={'ra': (float(RA) * 24.0)/360.0, 'dec': float(DEC) }
     
    # We want to set the ON_COORD_SET switch to engage tracking after goto
    # device.getSwitch is a helper to retrieve a property vector
    telescope_on_coord_set=device_telescope.getSwitch("ON_COORD_SET")
    while not(telescope_on_coord_set):
        time.sleep(0.5)
        telescope_on_coord_set=device_telescope.getSwitch("ON_COORD_SET")
    # the order below is defined in the property vector, look at the standard Properties page
    # or enumerate them in the Python shell when you're developing your program
    if not sync:
        telescope_on_coord_set[0].s=PyIndi.ISS_ON  # TRACK
        telescope_on_coord_set[1].s=PyIndi.ISS_OFF # SLEW
        telescope_on_coord_set[2].s=PyIndi.ISS_OFF # SYNC
    else:
        telescope_on_coord_set[0].s=PyIndi.ISS_OFF  # TRACK
        telescope_on_coord_set[1].s=PyIndi.ISS_OFF # SLEW
        telescope_on_coord_set[2].s=PyIndi.ISS_ON # SYNC
    indiclient.sendNewSwitch(telescope_on_coord_set)
    # We set the desired coordinates
    telescope_radec=device_telescope.getNumber("EQUATORIAL_EOD_COORD")
    while not(telescope_radec):
        time.sleep(0.5)
        telescope_radec=device_telescope.getNumber("EQUATORIAL_EOD_COORD")
    telescope_radec[0].value=target['ra']
    telescope_radec[1].value=target['dec']
    indiclient.sendNewNumber(telescope_radec)
    # and wait for the scope has finished moving (if not syncing)
    if not sync:
        while (telescope_radec.s==PyIndi.IPS_BUSY):
            print(f"Scope Moving: RA {Angle(telescope_radec[0].value*u.hour).to_string(unit=u.hour)}, DEC {Angle(telescope_radec[1].value*u.deg).to_string(unit=u.degree)}\r", end="")
            time.sleep(0.25)
        print(f"Scope Moving: RA {Angle(telescope_radec[0].value*u.hour).to_string(unit=u.hour)}, DEC {Angle(telescope_radec[1].value*u.deg).to_string(unit=u.degree)}")
    else:
        print(f"Scope Synced: RA {Angle(telescope_radec[0].value*u.hour).to_string(unit=u.hour)}, DEC {Angle(telescope_radec[1].value*u.deg).to_string(unit=u.degree)}")
    return

def indiserverConnect(hostname="localhost", port="7624"):
    # connect the server
    indiclient=IndiClient()
    indiclient.setServer(hostname,int(port))
    global blobEvent
    blobEvent=threading.Event()

    if (not(indiclient.connectServer())):
         print("No indiserver running on "+indiclient.getHost()+":"+str(indiclient.getPort())+" - Run server in Ekos first.")
         return None
    return (indiclient, blobEvent)

def indiserverDisconnect(indiclient):
    indiclient.disconnectServer()