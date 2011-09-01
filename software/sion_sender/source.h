/*
source.h
header file for source block of SION
specifies standard interface for the source driver
*/
#ifndef __SOURCE_DEFINED__
#define __SOURCE_DEFINED__

#include <stdint.h>
#include "scandal.h"

/*
 initialises the source.
 sourcepath specifies which device the data is coming from.
 in most cases this will be a device descriptor, but
 in case of logfile dummysender this will be a path to file.
 
 Returns 0 for success, -1 for fail.
*/
int init_source(char* sourcepath);

/* reads a single CAN packet from the source.
 */
int get_can_pkt(can_pkt *pkt);
int send_can_pkt(can_pkt *pkt);






/* gets file descriptor to the logfile for a dummy sender.
 */
FILE *get_fd(void);

/* reads a single can packet from dummysender
 */
int get_can_line(char *line, can_pkt *pkt, long double *ciel_timestamp);


#endif
