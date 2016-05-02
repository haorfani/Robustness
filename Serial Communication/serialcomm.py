#!/usr/bin/python
import serial
import time 

ser = serial.Serial(
   port='/dev/ttyUSB0',\
   baudrate=115200,\
   parity=serial.PARITY_NONE,\
   stopbits=serial.STOPBITS_ONE,\
   bytesize=serial.EIGHTBITS,\
   timeout=0)
print("connected to: " + ser.portstr)
#ser.write("help\n");
#ser.write("Egw sas gamaw olous!\n");


while True:
	ser.write(raw_input(">> ") + "\n")
	out = ''

	time.sleep(1)
	while ser.inWaiting() > 0:
		out += ser.read(40)
		if out != '':
			print(">>" + out)
	

ser.close()