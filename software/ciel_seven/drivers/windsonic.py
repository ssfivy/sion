import serial
import time
import can
class windsonic():
	def __init__(self):
		self.ser = serial.Serial("/dev/ttyS0", 9600, timeout=5)
		self.datavalid=0

	def poll(self):
		line = self.ser.readline()
		timestamp = int(round(time.time() * 1000))
		self.packet = line.split(',')
		#determine if packet is valid by checking checksum (also useful for weeding out the welcome message
		try:
			checksum = self.packet[5].strip('\x03\r\n')
			#todo later....
			#print self.packet[1]
			if not self.is_number(self.packet[1]):
				self.packet[1] = -1
			if not self.is_number(self.packet[2]):
				self.packet[2] = -1
			self.packet = (self.packet[0].strip('\x02'), self.packet[1], self.packet[2], self.packet[3], self.packet[4], timestamp)
			#print 'Received data: '+str(t)
			self.direction=can.sion_entry(7, 0, 220, 1, int(self.packet[1]), None, timestamp)
			self.speed=can.sion_entry(7, 0, 220, 0, int(float(self.packet[2]) * 1000), None, timestamp)
			self.datavalid=1
			#todo output the rest of the values
			
		except IndexError:
			print 'Unrecognized Data:'+line
			self.datavalid=0
			

	def is_number(self, s):
		try:
			float(s)
			return True
		except ValueError:
			return False
