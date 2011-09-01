#!/usr/bin/env python

#windplotter.py
#plots wind speed against time
#./grapher.py <databasename>
import sqlite3
import sys
import matplotlib
import matplotlib.pyplot as plt
import numpy 

#setup database
dbname = sys.argv[1]
conn = sqlite3.connect(dbname)
c = conn.cursor()

#create empty numpy array to hold data. Memory intensive, I know :(
c.execute('SELECT COUNT(*) FROM windlog')
graph = numpy.empty(shape=(c.fetchone()[0], 3))


#fetch data from database
line = 0
c.execute('SELECT wind_direction, wind_speed, kongming_timestamp FROM windlog ORDER BY packet_number')
for row in c:
	print row
	graph[line] = row
	print graph[line]
	line +=1

#close database, we're done with it
c.close()

#debug info, can be omitted
print graph[:,1]
print graph.shape
print graph.size
print graph.dtype.name
print graph.itemsize

#plot stuff

plt.plot(graph[:,2], graph[:,1], 'r.')
plt.plot(graph[:,2], graph[:,1])

#dt = datetime.datetime.fromtimestamp(graph[:,8])
#dates = matplotlib.dates.date2num(dt)
#plt.plot_dates(dates, graph[:,5], 'r.')

#axis, show, etc
plt.ylabel('wind speed')
plt.xlabel('time')
plt.show()

