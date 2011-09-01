#!/usr/bin/env python
# Fluke Hydra data logger interface software
# for logging x number of channels straight from command line to a sqlite database
# Irving Tjiptowarsono
# original code by Sam May
#

# libs
import argparse
import datetime
import time

# internal scanalysis libs
import Fluke 

#parse stuff
parser = argparse.ArgumentParser(description='Logs information from Fluke hydra datalogger')
parser.add_argument('tty', default='/dev/ttyUSB0', help='TTY port where the hydra is connected. Default is /dev/ttyUSB0', )
args = parser.parse_args()

createtime = datetime.datetime.utcnow()
dbname = createtime.strftime("hydralog-%Y-%m-%d-%H-%M-%S.sq3")
conn = sqlite3.connect(dbname)
c = conn.cursor()
c.execute('''CREATE TABLE hydralog(packet_number INTEGER PRIMARY KEY AUTOINCREMENT, channel_number INTEGER, channel_value REAL, channel_unit REAL, speed_unit TEXT, status INTEGER, kongming_timestamp REAL);''')
#c.execute('''PRAGMA synchronous=OFF''') #disables atomic commit for better performance
conn.commit()


## CONFIGURATION ##
interval = 1 # in secs
config["rate"] = Fluke.SLOW_RATE

# channels. each channel configuration is a list of arguments for the
# Fluke.set_function() function. So, for channel 2 here, set_function()
# will be called like set_function(2,"Temperature","J Thermocouple")
#
# the order is: channel,function,range,scaling values,terminals)
#	config["channels"] = {
#            2:{
#                "function":"Temperature",
#                "range":"J Thermocouple",
#            }, 
#            3:{
#                "function":"DC Voltage",
#                "range":"300mV",
#                "scaling":(0.0123,0,0),
#            },
#            4:{
#                "function":"DC Voltage",
#                "range":"300mV",
#                "scaling":(0.012285,0,0),
#            },
#        }

channels = {
1:{
    "function":"DC Voltage",
    "range":"300V",
    "name":"Output Voltage"
    }, 
2:{
    "function":"DC Voltage",
    "range":"300mV",
    "scaling":(39.4186975521086,0.00121266137497,0),
    "name":"Input Current"
    }, 
3:{
    "function":"DC Voltage",
    "range":"300mV",
    "scaling":(40.0034500365415,0.00049343453048,0),
    "name":"Output Current"
    },
11:{
    "function":"DC Voltage",
    "range":"300V",
    "name":"Input Voltage"
    },
}



print "Starting fluke on port " + tty
self.fluke = Fluke(tty)

# make sure we are in idle mode
self.fluke.stop_logging()

# set the time
self.fluke.set_datetime(datetime.datetime.now())

# set the interval
self.fluke.set_interval(interval)

# configure channel functions
functions = channels
self.channels = functions.keys()
self.channels.sort() # SIDE EFFECTS, sort() is IN PLACE!
for channel in self.channels:
    if "scaling" in functions[channel]:
	scaling = functions[channel]["scaling"]
    else:
	scaling = None
    self.fluke.set_function(channel,functions[channel]["function"],
			    scaling=scaling)

if config.has_key("monitor"):
    self.fluke.start_monitoring(config["monitor"])

self.fluke.start_logging()

while self.polling_event.isSet():
    data,timestamp = self.fluke.read_log()
    # we want seconds since the epoch
    t = time.mktime(timestamp.timetuple())
    # data is in order of channel
    packets = [(i,Driver.Deliverable(data[i],t)) for i in range(len(data))]

    # THREADING BIT
    self.log_queue_lock.acquire()
    # put on the array of deliverables
    self.log_queue.append(packets)
    self.log_queue_lock.release()
    #END THREADING BIT
self.fluke.stop_logging()
self.fluke.serial_link.close()


