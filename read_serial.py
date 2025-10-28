#!/usr/bin/env python3
import serial
import sys
import time

try:
    ser = serial.Serial('/dev/ttyUSB0', 115200, timeout=1)
    print("Connected to /dev/ttyUSB0")
    
    # Read for 10 seconds
    start_time = time.time()
    while time.time() - start_time < 10:
        try:
            line = ser.readline()
            if line:
                print(line.decode('utf-8', errors='ignore'), end='')
        except Exception as e:
            pass
    
    ser.close()
except Exception as e:
    print(f"Error: {e}")
    sys.exit(1)

