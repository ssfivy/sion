#!/usr/bin/env python
import serial
import ctypes


#if there's a better way to convert unsigned input to signed output, I'm all ears.
def int32(x):
  if x>0xFFFFFFFF:
    raise OverflowError
  if x>0x7FFFFFFF:
    x=int(0x100000000-x)
    if x<2147483648:
      return -x
    else:
      return -2147483648
  return x



ser = serial.Serial('/dev/ttyUSB1', 115200)
ser.write("rrr")

while 1:
	while ser.read() != 'C':
		continue #loop until delimiter shows up

	buf = ser.read(14)
	data = 'C'+buf #add back the bloody delimiter

	header = ctypes.c_uint32( ((ord(data[1]) << 24) | (ord(data[2]) << 16) | (ord(data[3]) << 8) | (ord(data[4]) << 0) ) & 0xFFFFFFFF).value
	payload = ctypes.c_uint32( ((ord(data[5]) << 24) | (ord(data[6]) << 16) | (ord(data[7]) << 8) | (ord(data[8]) << 0) ) & 0xFFFFFFFF).value
	time = ctypes.c_uint32( ((ord(data[9]) << 24) | (ord(data[10]) << 16) | (ord(data[11]) << 8) | (ord(data[12]) << 0) ) & 0xFFFFFFFF).value

#	payload = int32(rawpayload)

	priority = (header >> 26) & ((1<<3)-1)
	messagetype = (header >> 18) & ((1<<8)-1)
	source_address = (header >> 10) & ((1<<8)-1)
	specifics = (header >> 0) & ((1<<10)-1)
	print str(header)
	print str(priority)+'\t'+str(messagetype)+'\t'+str(source_address)+'\t'+str(specifics)+'\t'+str(payload)+'\t'+str(time)




