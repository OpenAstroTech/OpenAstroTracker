#!/usr/bin/env python3
import serial, io

def sendCommand(command, serialport):
    ser = serial.Serial(serialport, 57600, timeout = 1)
    sio = io.TextIOWrapper(io.BufferedRWPair(ser, ser))
    ser.write(str(command).encode())
    sio.flush()
    result = sio.readline()
    return result