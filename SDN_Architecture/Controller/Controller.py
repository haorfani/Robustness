#!/usr/bin/python
import serial
import time, threading
import csv

ser = serial.Serial(
   port='/dev/ttyUSB0',\
   baudrate=115200,\
   parity=serial.PARITY_NONE,\
   stopbits=serial.STOPBITS_ONE,\
   bytesize=serial.EIGHTBITS,\
   timeout=0)
print("connected to: " + ser.portstr)

fileopen = open("flows.txt", "rw+") # name of the flowtable
flowtable = []


def flowtableUpdate(str):
	fileopen = open("flows.txt", "rw+")
	print "Opened file: ", fileopen.name
	
	for line in fileopen:
		flowtable.append(line)

	i = 0
	for line in flowtable:
		print flowtable[i]
		i = i+1
	return

def interferenceID():


#put the fingerprint to the learning algorithm and send the result(wifi,bluetooth,Microwave)
#if the the result error is over a thresshold try to create a new category of interference which will have the according mitigation strategy regarding the channel, time 
	
	print "Detected  WiFi intereference "
	return


# while True:
# 	ser.write(raw_input(">> ") + "\n")
# 	out = ''
# 	counter =1
# 	time.sleep(1)
	# while ser.inWaiting() > 0:
	# 	out = ser.read(ser.inWaiting())
	# 	if out == "received line: open\n":
	# 		fo = open("flows.txt", "rw+")
	# 		print "Opened file: ", fo.name
	# 		line = fo.readline()
	# 		print "Read Line: %s" % (line)
	# 		fo.close()
	# 	elif out == "Message received!\n":
	# 		print("message No"+counter)
	# 		counter += 1
	# 	else:
	# 		print(">>"+out)
line = []
flowtableUpdate(fileopen)

while True:
    for c in ser.read():
        line.append(c)
        if c == '\n':
            print("".join(line)) # print all serial output
            s1 = "".join(line)
            s2=s1.split(":")
            if s2[0]=="Message received":
            	src_address = s2[6].split(" ")
            	src_address = src_address[0]
            	i=0
            	for line in flowtable:
            		flow = flowtable[i].split(",")
            		i = i+1
            		if src_address == flow[0]:
            			src=flow[0]
            			dst=flow[1]
            			action=flow[2]
            			statistics=flow[3]
            			if action=="drop":
            				ser.write("Drop packet\n")
            			if action=="accept":
            				ser.write("Accept packet\n")
            			print ("Flow detected "+str(flow))
            line = []
            break

ser.close()
	
