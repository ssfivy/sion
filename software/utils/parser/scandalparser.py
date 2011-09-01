#!/usr/bin/env python

# Parser for scandal can log

#Load necessary modules
#import sys
import fileinput
import matplotlib
#Necessary files
#log = sys.argv[2] #the log file to open
#scandal_devices = '../carsoft/scanalysis/scandal_devices.cfg' #Scanalysis node database
output_file = 'filtered.log' #output file name

#initialise CAN packet filter

#Message priority
priority = 0x0 #telem_low
#message type
mtype=0x00 #channel message
#node address
address = 0x0d #negsum
#channel number
channel = 0x001 #current 1
#Construct filter
pfilter = (priority<<26) | (mtype<<18) | (address<<10) | (channel<<0)

#initialize mask (for ignoring parameters such as priority)

#Message priority
priority = 0x0 #set to low
#message type
mtype=0xFF 
#node address
address = 0xFF 
#channel number
channel = 0x3FF
#Construct filter
mask = (priority<<26) | (mtype<<18) | (address<<10) | (channel<<0)

print pfilter
#print bin(mask)

raw_input("Press ENTER...")

#open file
logs = fileinput.input()
temp = open(output_file,'w')

#process data
number = 0
for line in logs:
	column = line.split()
	#filter the data we want
	if ((int(column[0],16) & mask) == pfilter ):
		print column[0]+' '+column[1]+' '+column[2]
		print int(column[0],16)
		print pfilter
		#append to temp file (on disk)
		raw_input("Press ENTER...")
		temp.writelines([column[0], ' ', column[1], ' ', column[2]])

temp.flush()
temp.close()

#plot file
#temp = fileinput.input(output_file)



