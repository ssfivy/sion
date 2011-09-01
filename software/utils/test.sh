#!/bin/bash
LOGFILE=test.canlog
DBFILE=test.sqlite
./ciel_receiver $DBFILE &
ssh ertos@sion 'cd /home/ertos/sion/; ./sion_sender $LOGFILE'

