#! /usr/bin/env python

import canbridge
import time

i=0
oldtime = time.time()
while True:
        canbridge.outputSend()
        canbridge.inputRecv()
        pkt = canbridge.inputDequeue()
        if pkt is not None:
            i=i+1
            (id, data, len) = pkt
            pprio = (id >> 26) & ((1<<3)-1)
            ptype = (id >> 18) & ((1<<8)-1)
            if ptype is 0: # Channel message for moment
                paddr = (id >> 10) & ((1<<8)-1)
                pchan = (id >> 0) & ((1<<10)-1)
                                
                pvalue = (((data >> 24) & 0xFF) << 0 ) | \
                        (((data >> 16) & 0xFF) << 8) | \
                        (((data >> 8) & 0xFF) << 16) | \
                        (((data >> 0) & 0xFF) << 24)
                ptime =  (((data >> 56) & 0xFF) << 0 ) | \
                        (((data >> 48) & 0xFF) << 8) | \
                        (((data >> 40) & 0xFF) << 16) | \
                        (((data >> 32) & 0xFF) << 24)
                
                if paddr is 5 and pchan is 2:
                        print "%u\t%u\t%u\t%u" % (paddr, pchan, pvalue, ptime)
                        
            continue
        time.sleep(0.01)
        
        if time.time() > oldtime + 1.0:
            rate = i / (time.time() - oldtime)
            i = 0
            oldtime = time.time()
            print "Rate: %f" % rate