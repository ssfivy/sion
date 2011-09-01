/*
Scandal CAN log parser
parses CAN log ....and stores them in SQLite database (next version)

run this and you will get a familiar appearance.

depends on libsqlite3-dev

depends on modules in ../common

compile with gcc -Wall -Wextra -lsqlite3 -o parser parser.c

*/
#include <stdio.h>
#include <string.h>
#include <stdint.h> //for exact definitions of variable lengths
#include <stdlib.h> //exit()
#include <sqlite3.h> //need libsqlite3-dev
#include <unistd.h> //usleep

#include "source.h"
#include "scandal.h"
#include "sqlitedriver.h"

#define MAX_LINE_LENGTH 50
#define BLOCKLEN 40
#define STREAM 1
#define BULK 0
#define STREAM_MODE BULK /* set to stream if you want slow log reading.*/



int main (int argc, char *argv[]) {
	if (argc != 3) {
		printf("Usage: parser <logfile> <sqlitefile>\n");
		exit(1);
	}
	char line[MAX_LINE_LENGTH];

	/* can packet parameters */
	can_pkt pkt;
	sion_entry entry;
	
	sqlite3 *db;
	
	init_source(argv[1]);
	init_sqlite3(&db, argv[2], NEWDB);

	while ( fgets(line, MAX_LINE_LENGTH-1, get_fd()) != NULL ) {
		
		if (STREAM_MODE)
			usleep(5000);
		
		get_can_line(line, &pkt, &entry.ciel_timestamp);
		cantoentry(&pkt, &entry);
		//insert_can_packet(db, &entry);
		queue_can_packet(db, &entry, BLOCKLEN);
		
		/* ahh, CANUSB appearance. Soothing.*/
		printf("%u\t%u\t%u\t%u\t%d\t\t%u\t\t%Lf\n",  entry.priority, entry.message_type, entry.source_address, entry.specifics, entry.value, entry.scandal_timestamp, entry.ciel_timestamp);
	}
	
	shutdown_sqlite3(db);
	return 0;
}

