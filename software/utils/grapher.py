#!/usr/bin/env python

#grapher.py
#graphs can channel against time from a sqlite database
#./grapher.py <databasename> <packet_type> <can_address> <can_specifics (channels etc)> <start_seq> <end_seq>
import sqlite3
import sys
import matplotlib
import matplotlib.pyplot as plt
import numpy 
import datetime

#set up filter
start_seq = sys.argv[5] # 1600000
end_seq = sys.argv[6] # 1775000
msgtype = sys.argv[2] # 0 for Channel message, 2 for heartbeat
addr = sys.argv[3] #address of the sending node
specifics = sys.argv[4] #for channel message, this is the channel number. For heartbeats, this is node type.
pkt = (start_seq, end_seq, msgtype, addr, specifics)

#setup database
dbname = sys.argv[1]
conn = sqlite3.connect(dbname)
c = conn.cursor()

#create empty numpy array to hold data. Memory intensive, I know :(
c.execute('SELECT COUNT(packet_number) FROM canlog WHERE packet_number > ? AND packet_number < ? AND message_type = ? AND source_address = ? AND specifics = ?', pkt)
graph = numpy.empty(shape=(c.fetchone()[0], 2))

#fetch data from database
line = 0
c.execute('SELECT value, ciel_timestamp FROM canlog WHERE packet_number > ? AND packet_number < ? AND message_type = ? AND source_address = ? AND specifics = ? ORDER BY packet_number', pkt)
for row in c:
	graph[line] = row
	line +=1


#create empty numpy array to hold data. Memory intensive, I know :(
c.execute('SELECT COUNT(packet_number) FROM canlog WHERE message_type = 0 AND source_address = 12 AND specifics = 2')
graph2 = numpy.empty(shape=(c.fetchone()[0], 2))

#fetch data from database
line = 0
c.execute('SELECT value, ciel_timestamp FROM canlog WHERE message_type = 0 AND source_address = 12 AND specifics = 2 ORDER BY packet_number')
for row in c:
	graph2[line] = row
	line +=1


#close database, we're done with it
c.close()

#debug info, can be omitted
#print graph[:,7]
print graph.shape
print graph.size
#print graph.dtype.name
#print graph.itemsize

#plot stuff

plt.plot(graph[:,1], graph[:,0], 'r.')
plt.plot(graph[:,1], graph[:,0] )
plt.plot(graph2[:,1], graph2[:,0]*10000, 'g.')
plt.plot(graph2[:,1], graph2[:,0]*10000 )
#dt = datetime.datetime.fromtimestamp(graph[:,8])
#dates = matplotlib.dates.date2num(dt)
#plt.plot_dates(dates, graph[:,5], 'r.')

#axis, show, etc
plt.ylabel('current')
plt.xlabel('time')
plt.grid(True)
plt.show()

