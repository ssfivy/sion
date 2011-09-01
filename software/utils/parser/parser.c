/*
Scandal CAN log parser
parses CAN log ....and stores them in SQLite database (next version)

run this and you will get a familiar appearance.

depends on libsqlite3-dev

compile with gcc -Wall -Wextra -lsqlite3 -o parser parser.c

*/


#include <stdio.h>
#include <string.h>
#include <stdint.h> //for exact definitions of variable lengths
#include <stdlib.h> //exit()
#include <sqlite3.h> //need libsqlite3-dev

#define MAX_LINE_LENGTH 50
#define MAX_ARG_LENGTH 20
#define MAX_SQL_QUERY_LENGTH 300

#define PRI_OFFSET 26
#define PRI_BITS 3
#define TYPE_OFFSET 18
#define TYPE_BITS 8
#define ADDR_BIT_OFFSET 10
#define ADDR_BIT_MASK 8
#define SPECIFICS_BIT_OFFSET 0
#define SPECIFICS_BIT_MASK 10

int main (int argc, char *argv[]) {
	if (argc != 3) {
		printf("Usage: parser <logfile> <sqlitefile>\n");
		exit(1);
	}

	FILE *canlog;
	char line[MAX_LINE_LENGTH];
	long double control_timestamp;

	/* SQL stuff */
	sqlite3 *db;
	int dbconn;
	char query[MAX_SQL_QUERY_LENGTH];
	char * sqlerrmsg = 0;
	int rc;

	/* Scandal packet parameters */
	uint32_t header; //full 29 bit CAN header
	uint8_t priority, messagetype; //all messages
	uint8_t source_address;
	uint16_t specifics; //meaning fo this variable changes depending on message type. Channel number, node type, etc

	uint64_t payload;
	int32_t value; //32 bit value, signed
	uint32_t scandal_timestamp; //32 bit timestamp, unsigned

	/* open log file */
	canlog = fopen(argv[1], "r");
	if (canlog == NULL){
		printf("blargh blargh file read fails\n");
		exit(1);
	}

	/* open and setup database */ 
	dbconn = sqlite3_open(argv[2], &db);
	if( dbconn ){
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		exit(1);
	}

	memset(query, 0, sizeof(query));
	strcpy(query, "CREATE TABLE canlog(	packet_number INTEGER PRIMARY KEY AUTOINCREMENT, priority INTEGER, messagetype INTEGER, sourceaddress INTEGER, specifics INTEGER, value INTEGER, scandal_timestamp INTEGER, sion_timestamp INTEGER, control_timestamp REAL);");

	rc = sqlite3_exec(db, query, NULL, NULL, &sqlerrmsg);
	if( rc!=SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", sqlerrmsg);
		sqlite3_free(sqlerrmsg);
	}

	/* EXPERIMENTAL - turns atomic commit off!!! */
	memset(query, 0, sizeof(query));
	strcpy(query, "PRAGMA synchronous = OFF;");

	rc = sqlite3_exec(db, query, NULL, NULL, &sqlerrmsg);
	if( rc!=SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", sqlerrmsg);
		sqlite3_free(sqlerrmsg);
	}

	while ( fgets(line, MAX_LINE_LENGTH-1, canlog) ) {
		sscanf(line,"%x %Lx %Lf", &header, &payload, &control_timestamp ); 
		
		/* Let's Decode! */
		priority = (header >> PRI_OFFSET) & ((1<<PRI_BITS)-1); //Copied from Dave, he's trying to be clever here :<
		messagetype = (header >> TYPE_OFFSET) & ((1<<TYPE_BITS)-1);
		source_address = (header >> ADDR_BIT_OFFSET) & ((1<<ADDR_BIT_MASK)-1);
		specifics = (header >> SPECIFICS_BIT_OFFSET) & ((1<<SPECIFICS_BIT_MASK)-1);

		value = (int32_t)((payload >> 32) & 0x00000000FFFFFFFF);
		scandal_timestamp = (uint32_t)(payload & 0x00000000FFFFFFFF);

		/* ahh, CANUSB appearance. Soothing.*/
		//printf("--p-%Lx\n", payload);
		//printf("vt-%x-%x\n",value, scandal_timestamp);
		printf("%u\t%u\t%u\t%u\t%d\t\t%u\t\t%Lf\n", priority, messagetype, source_address, specifics, value, scandal_timestamp, control_timestamp);

		/* Insert row into database */
		memset(query, 0, sizeof(query));
		sprintf(query, "INSERT INTO canlog VALUES (NULL, %u, %u, %u, %u, %d, %u, 0, %Lf );", priority, messagetype, source_address, specifics, value, scandal_timestamp, control_timestamp);
		rc = sqlite3_exec(db, query, NULL, NULL, &sqlerrmsg);
		if( rc!=SQLITE_OK ){
			fprintf(stderr, "SQL error: %s\n", sqlerrmsg);
			sqlite3_free(sqlerrmsg);
		}

	}
	
	sqlite3_close(db);
	return 0;
}

