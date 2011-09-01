#!/usr/bin/env python

import argparse
import serial
import datetime #for utc time
import time #for unix time
import sqlite3
import signal #ctrl-c cleanup
import sys

def signal_handler(signal, frame):
	print 'Comitting database and stuff...'
	conn.commit()
	c.close()
	ser.close()
	sys.exit(0)

def is_number(s):
    try:
        float(s)
        return True
    except ValueError:
        return False


#parse stuff
parser = argparse.ArgumentParser(description='Logs information from Windsonic sensor into an SQLite database, in scandal format.')
parser.add_argument('tty', help='TTY port where the sensor is connected.')
args = parser.parse_args()

#setup stuff
signal.signal(signal.SIGINT, signal_handler)
ser = serial.Serial(args.tty, 9600, timeout=5)
createtime = datetime.datetime.utcnow()
dbname = createtime.strftime("sunswift-%Y-%m-%d-%H-%M-%S.sq3")
conn = sqlite3.connect(dbname)
c = conn.cursor()
c.execute('''CREATE TABLE windlog(packet_number INTEGER PRIMARY KEY AUTOINCREMENT, windsonic_node TEXT, wind_direction INTEGER, wind_speed REAL, speed_unit TEXT, status INTEGER, kongming_timestamp REAL);''')
#c.execute('''PRAGMA synchronous=OFF''') #disables atomic commit for better performance
conn.commit()

while 1:
	#read packets
	line = ser.readline()
	timestamp = time.time()
	packet =  line.split(',')
	print packet

	#determine if packet is valid by checking checksum (also useful for weeding out the welcome message)
	try:
		checksum = packet[5].strip('\x03\r\n')
		#todo later....
		
		if not is_number(packet[1]):
			packet[1] = -1
		if not is_number(packet[2]):
			packet[2] = -1
		t = (packet[0].strip('\x02'), packet[1], packet[2], packet[3], packet[4], timestamp)
		c.execute("""INSERT INTO windlog VALUES(NULL, ?, ?, ?, ?, ?, ?)""", t)
		print 'Committing: '+str(t)
		conn.commit()
	except IndexError:
		print 'Unrecognized Data:'+line




