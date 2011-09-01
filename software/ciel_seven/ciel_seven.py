#!/usr/bin/env python

# ciel_seven.py
#control car telemetry interface.
#receives data from sensors, treats them as telemetry from CAN nodes
#and sends them to ciel_receiver for logging using sockets (as IPC method...).
#Sockets = can put this script on separate computer if needed, also probably easiest IPC method for future newbies.

import sys
import signal
import datetime

#socket configs
SEVEN_IP = '127.0.0.1'
SEVEN_PORT = '1701' #sending port # NCC-1701 / USS Enterprise
CIEL_IP = '127.0.0.1'
CIEL_PORT = '3435' #receiving port # Munchausen number

print "SEVEN: Starting up.... "

#signal handling

def signal_handler(signal,frame):
	print 'Receiving signal, shutting down....'
	sys.exit(0)

#connect to ciel
#uses UDP, there should be zero loss as it does not leave the OS.


#load drivers.
#All drivers are treated as character devices. You need to overhaul things if you want block devices.
#Drivers should fail to load when they fail, but should not stop program execution.
print "SEVEN: Loading drivers.... "
from drivers import windsonic

wind = windsonic.windsonic()

#enter loop, send stuff

#HACK

createtime = datetime.datetime.utcnow()
endname = createtime.strftime("%Y-%m-%d-windlog.log")
filename = '../canlog/' + endname
print filename
f = open(filename, 'a')

while 1:
	wind.poll()
	string=""
#TOTAL HACK - I dont have time to debug my scandal packet creation code, so I'll justr dump a text file instead!
	if wind.datavalid == 1:
		for i in wind.packet:
			string += str(i)
			string += ' '
		string += '\n'
		f.write(string)
		print string

#	wind.speed.string()
#	print wind.speed.longstring
