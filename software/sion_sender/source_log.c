/*
source_log.c

dummy source for SION
opens a previous scandal logfile
use with source.h

*/

#include <stdio.h>
#include <stdint.h>
#include "scandal.h"

FILE *sd;

int init_source(char* sourcepath) {
	/* open log file */
	sd = fopen(sourcepath, "r");
	if (sd == NULL){
		printf("blargh blargh file read fails\n");
		return -1;
	}
	return 0;
}

FILE *get_fd(void) {
	return sd;
}

int get_can_line(char *line, can_pkt *pkt, long double *ciel_timestamp) {
	sscanf(line,"%x %Lx %Lf", &pkt->id, &pkt->payload, ciel_timestamp );
	return 0;
	
}