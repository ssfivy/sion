/*
* Project: SION (Sunswift IV Observer Node)
* File name: socket.c
* Author: Irving Tjiptowarsono
* Description: 
* sockets interface based on tutorial on beej.us and old sunswift code
* Uses getaddrinfo() instead of manually loading structs
* ipv4 only for now.
* 
* 
* Copyright (C) 2011 NICTA (www.nicta.com.au)
* 
*/

#include "socket.h"

int socket_init(int *sockfd, struct addrinfo **remoteinfo,
				char *local_host, char* local_port,
				char *remote_host, char* remote_port) {
	
	struct addrinfo hints, *localinfo, *p;
	int rv;
	
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
	hints.ai_socktype = SOCK_DGRAM;
	//hints.ai_flags = AI_PASSIVE; // use my IP - but on which interface?
	
	if ((rv = getaddrinfo(remote_host, remote_port, &hints, remoteinfo)) != 0) {
        fprintf(stderr, "getaddrinfo/remote: %s\n", gai_strerror(rv));
        return 1;
    }
    
	if ((rv = getaddrinfo(local_host, local_port, &hints, &localinfo)) != 0) {
        fprintf(stderr, "getaddrinfo/local: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = localinfo; p != NULL; p = p->ai_next) {
        if ((*sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("listener: socket");
            continue;
        }

        if (bind(*sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(*sockfd);
            perror("listener: bind");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "listener: failed to bind socket\n");
        return 2;
    }

    freeaddrinfo(localinfo);
	
	return 0;
}

int socket_send(int *sockfd, void *data, int msglength, struct addrinfo *remoteinfo) {
	int numbytes;
	
	numbytes = sendto(*sockfd, data, msglength, 0,
					remoteinfo->ai_addr, 
					remoteinfo->ai_addrlen);
	
	if (numbytes < 0){
		if(errno!=EAGAIN)
			perror("socket_send()");
		return numbytes;
	}

	return numbytes;
}

int socket_recv(int *sockfd, void *data, int maxlength) {
	int numbytes;
	struct sockaddr_storage address;
	socklen_t address_length = sizeof(struct sockaddr_storage);
	
	numbytes = recvfrom(*sockfd, data, maxlength, MSG_WAITALL,
					(struct sockaddr*)&address, &address_length);
	
	if (numbytes < 0){
//		if(errno!=EAGAIN)
//			perror("socket_recv()");
		return numbytes;
	}

	/* should test to see if the packet was from the right address here */

	return numbytes;
}
