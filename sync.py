#!/usr/bin/python2.7

import serial
from datetime import datetime, date
import time
import dateutil.parser



ser = serial.Serial('/dev/ttyUSB0', 9600, timeout=0.5)

timestamp = int(time.mktime(datetime.now().timetuple()))
ser.write("T{}\r\n".format(timestamp))

print "Setting time to {}".format(datetime.fromtimestamp(timestamp))

a = raw_input("channel A time: ")
if a != "":
	a_time = dateutil.parser.parse(a)
	print "Setting channel A to {}".format(a_time)
	timestamp = int(time.mktime(a_time.timetuple())) % 86400
	ser.write("A{}\r\n".format(timestamp))
else:
	ser.write("X\r\n")

b = raw_input("channel B time: ")
if b != "":
	b_time = dateutil.parser.parse(b)
	print "Setting channel B to {}".format(b_time)
	timestamp = int(time.mktime(b_time.timetuple())) % 86400
	ser.write("B{}\r\n".format(timestamp))
else:
	ser.write("Y\r\n")

c = raw_input("reset time: ")
if c != "":
	c_time = dateutil.parser.parse(c)
	print "Setting channel C to {}".format(c_time)
	timestamp = int(time.mktime(c_time.timetuple())) % 86400
	ser.write("C{}\r\n".format(timestamp))
else:
	ser.write("Z\r\n")