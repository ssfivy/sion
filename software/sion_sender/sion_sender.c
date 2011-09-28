/*
sion_sender.c
Main executable for SION
Takes CAN data from a source, logs them in sqlite3 and sends them through socket.
Syncs sqlite3 on shutdown.
Goes into the solar car, requires crosscompilation.

Irving Tjiptowarsono 2011

*/
#include <stdio.h>
#include <string.h>
#include <stdint.h> //for exact definitions of variable lengths
#include <stdlib.h> //exit(), system()
#include <unistd.h> //usleep
#include <signal.h> //signal
#include <pthread.h> //multithreading
#include <errno.h>

#include "sionconfig.h"
#include "sqlite3.h"
#include "source.h"
#include "scandal.h"
#include "sqlitedriver.h"
#include "socket.h"

#define MAX_LINE_LENGTH 50

/*
Very hacky way of handling signal.
Please don't kill me.

Also, sqlite connection handles aren't supposed to be moved between threads;
though I haven't seen any obvious problems here.

Probably also because there's only one transfer, then connection is closed.
*/
sqlite3 *db, *dbsync;

/* Exits due to signal */
void exit_cleanly(int signum) {
	printf("SION: Receiving signal %d, exiting...\n", signum);
	shutdown_sqlite3(db);
	sqlite3_close(dbsync);
	exit(1);
}

/* Shutdown code - identical to signal handler except it also halts the system */
/* Do not use if supercap backup time < time needed to halt*/
void shutdown_cleanly(void) {
	printf("SION: Power loss! Shutting down system!\n");
	shutdown_sqlite3(db);
	sqlite3_close(dbsync);

	//halt the system! 
	system("halt &"); 
	exit(1);
}


int sockfd_sender, sockfd_sync;
struct addrinfo *remoteinfo, *syncinfo;

volatile int can_interface_ready = -1;

void * sync_thread (void) {
	printf("SION: Sync thread started...\n");

	sion_entry rentry;
	control_packet ctrl_pkt;
	uint8_t cstring[CONTROLSTRINGSIZE];
	uint8_t rstring[SCANDALLONGSTRINGSIZE];
	
	while(1) {
		//FIXME: Need select due to nonblocking I/O
		socket_recv(&sockfd_sync, cstring, CONTROLSTRINGSIZE);
		stringtocontrol(cstring, &ctrl_pkt);
		if (ctrl_pkt.type == CAN_PACKET_RETRANSMISSION_REQUEST) {
			fetch_sion_entry(dbsync, &rentry, ctrl_pkt.value);
				//TODO: Handle failed fetching - if packet nonexistent send dummy
			entrytolongstring(&rentry, rstring);
			socket_send(&sockfd_sync, rstring, SCANDALLONGSTRINGSIZE, syncinfo);
		}
	}
	pthread_exit(NULL);
}

/* Comms from control car to solar car */
void * output_thread (void) {
	printf("SION: Output thread started...\n");

	can_pkt opkt;
	sion_entry oentry;
	uint8_t ostring[SCANDALSTRINGSIZE];

	printf("SION: Output thread: Waiting until CAN interface is ready...\n");
	while (can_interface_ready != 0){
		usleep(20000);
	}

	printf("SION: Done initialising output thread, entering main loop....\n");
	while (1) {
		if (socket_readable(&sockfd_sender) == 0){ //if there's data in the socket...
			socket_recv(&sockfd_sender, ostring, SCANDALSTRINGSIZE);
			stringtoentry(ostring, &oentry);
			//printf_sion_entry(&oentry);
			entrytocan(&oentry, &opkt);
			send_can_pkt(&opkt);
		}
	}
	pthread_exit(NULL);
}


/* Main function */
int main (int argc, char *argv[]) {
	can_interface_ready = -1;

	/* checking for arguments */
	if (argc != 2) {
		printf("Usage: sion_sender <source> \n");
		printf("Ex: sion_sender /dev/ttyO0 \n");
		exit(1);
	}
	
	/* Signal handling */
	printf("SION: Setting up signals...\n");
	signal(SIGINT, exit_cleanly); //exit on CTRL-C
	signal(SIGTERM, exit_cleanly); //exit on kill / killall

	/* Open / set up sqlite db */
	char dbfilename[MAX_LINE_LENGTH];
	makedbfile(dbfilename, ACCURACY_DAY);
	if (checkdbfile(dbfilename)){ //if database exists, do not re-create tables
		printf("SION: opening previous database: %s\n", dbfilename);
		init_sqlite3(&db, dbfilename, OLDDB); //for writing
		init_sqlite3(&dbsync, dbfilename, OLDDB);//for sync thread (read-only)
	}
	else { //make new db and fill it with tables.
		printf("SION: creating new database: %s\n", dbfilename);
		init_sqlite3(&db, dbfilename, NEWDB); //for writing
		init_sqlite3(&dbsync, dbfilename, OLDDB);//for sync thread (read only)
	}
	

	/* Networking stuff*/
	printf("SION: Setting up networks...\n");
	int numbytes;
	char sendqueue[MAX_SOCKET_BLOCK_LENGTH];
	socket_init(&sockfd_sender, &remoteinfo,
		SIONOUTHOST, SIONOUTPORT, CIELIN_SION_HOST, CIELIN_SION_PORT) ;
	socket_init(&sockfd_sync, &syncinfo,
		SIONIN_SYNC_HOST, SIONIN_SYNC_PORT, CIELOUT_SYNC_HOST, CIELOUT_SYNC_PORT) ;

	/* threading */
	printf("SION: Setting up threads...\n");
	pthread_t receiver_thread, retx_thread;
	int rc;
	//FIXME: DISABLED due to noblocking socket
	rc = pthread_create(&receiver_thread, NULL, output_thread, NULL);
	if (rc) {
		printf("ERROR; return code from pthread_create() is %d\n", rc);
		exit(-1);
	}
	/*
 	rc = pthread_create(&retx_thread, NULL, sync_thread, NULL);
	if (rc) {
		printf("ERROR; return code from pthread_create() is %d\n", rc);
		exit(-1);
	}
	*/

	/* can stuff */
	can_pkt pkt;
	sion_entry entry;
	uint8_t string[SCANDALLONGSTRINGSIZE];
	uint32_t pkt_id_new;
	pkt_id_new = get_largest_pkt_id(db);

	/*init this last, since it triggers the lpc1768 to start sending data*/
	can_interface_ready = init_source(argv[1]); 

	/* main sender loop */
	printf("SION: Done initialisation, entering main loop....\n");
	while (1) {
		
		if (get_can_pkt(&pkt) == -20 ) {
			/*if true, that means we received shutdown packet. Sync and exit. */
			shutdown_cleanly();
		}

		/* Convert to proper format and add packet ID */
		cantoentry(&pkt, &entry);
		pkt_id_new++;
		entry.pkt_id = pkt_id_new;
		entrytolongstring(&entry, string);

		/* Queue data to wifi */
		numbytes = queue_socket_send(&sockfd_sender, string, SCANDALLONGSTRINGSIZE, remoteinfo,
			 sendqueue, SOCKET_BLOCK_LENGTH);

		/* Queue data to SQLite */
		queue_can_packet(db, &entry, SQLITE_BLOCKLEN);
		
		/* debug printout on terminal. disable by default, since it can pollute login tty*/
		#define SION_DEBUG
		#ifdef SION_DEBUG
		//this will cause lag, do not use unless under low bandwith.
		//printf_sion_entry(&entry); 
		printf("%llu\t%u\n\r", entry.scandal_timestamp, entry.pkt_id);
		#endif
	}
	
	sqlite3_close(db);
	pthread_exit(NULL);
	return 0;
}

