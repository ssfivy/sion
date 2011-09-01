/*
* Project: SION (Sunswift IV Observer Node)
* File name: socket.h
* Author: Irving Tjiptowarsono
* Description:
* Header file for socket comms
* 
* 
* 
* Copyright (C) 2011 NICTA (www.nicta.com.au)
* 
*/

#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <stdio.h>


int socket_init(int *sockfd, struct addrinfo **remoteinfo,
				char *local_host, char* local_port,
				char *remote_host, char* remote_port) ;

int socket_send(int *sockfd, void *data, int msglength, struct addrinfo *remoteinfo);
int socket_recv(int *sockfd, void *data, int maxlength);


/* oold definitions, kept for comparison so far.
int socket_init(char *local_host, int local_port,
                char *remote_host, int remote_port);
int socket_send(void *data, size_t length);
int socket_recv(void *data, size_t length);
int socket_readable(void);
int socket_writeable(void);	
*/
