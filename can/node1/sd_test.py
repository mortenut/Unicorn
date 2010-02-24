#!/usr/bin/env python

import serial
import time

ser = serial.Serial()
ser.baud = 38400
ser.port = '/dev/ttyUSB0'
ser.open()

ser.write('a')
ser.read(1)
t1 = time.time()
ser.read(1)
t2 = time.time()
print t2-t1
