#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#ifndef UCSIMM
#include <sys/poll.h>
#endif

#define MAX_PACKET_SIZE 1500

struct sockaddr_in local_address;
struct sockaddr_in remote_address;

int             sock = -1;

int
socket_fd(void){
	return sock;
}

int 
socket_init(char *local_host, int local_port,
            char *remote_host, int remote_port)
{
	int socket_flags;
	int buf_size = MAX_PACKET_SIZE;	

	/* resolve local address */
	memset(&local_address, 0, sizeof(struct sockaddr_in));
	local_address.sin_family = AF_INET;
	local_address.sin_addr.s_addr = inet_addr(local_host);
	local_address.sin_port = htons(local_port);

	/* resolve remote addrsss */
	memset(&remote_address, 0, sizeof(struct sockaddr_in));
	remote_address.sin_family = AF_INET;
	remote_address.sin_addr.s_addr = inet_addr(remote_host);
	remote_address.sin_port = htons(remote_port);

	/* open the socket */
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		fprintf(stderr, "error creating socket\n");
		goto err;
	}

	/* bind to a local address */
	if (bind(sock, (struct sockaddr *) & local_address,
		 sizeof(struct sockaddr_in))) {
		fprintf(stderr, "error binding socket\n");
		goto err;
	}

	/* minimise the buffer size */
	setsockopt(sock, SOL_SOCKET, SO_SNDBUF, &buf_size, sizeof(int));
	setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &buf_size, sizeof(int));

	/* make it non-blocking */
	socket_flags = fcntl(sock, F_GETFL) | O_NONBLOCK;
	fcntl(sock, F_SETFL, socket_flags);

	fprintf(stderr, "initialised client/server communications socket successfully\n");

	return 0;

err:
	fprintf(stderr, "unable to open client / server communications socket\n");
	fflush(stderr);

	sock = -1;

	return -1;
}

int 
socket_send(void *data, size_t length)
{
	if (sendto(sock, data, length, 0, (struct sockaddr *) &
		   remote_address, sizeof(struct sockaddr_in)) < 0){
		if(errno!=EAGAIN)
			perror("socket_send()");
		return -1;
	}

	return 0;
}

int 
socket_recv(void *data, size_t length)
{
	struct sockaddr_in address;
	socklen_t address_length = sizeof(struct sockaddr_in);

	if (recvfrom(sock, data, length, MSG_WAITALL,
		     (struct sockaddr*)&address, &address_length) < 0){
		if(errno!=EAGAIN)
			perror("socket_recv()");
		return -1;
	}

	/* should test to see if the packet was from the right address here */

	return 0;
}

int socket_readable(void){
#ifndef UCSIMM
	struct pollfd status;

	status.fd = sock;
	status.events = POLLIN;
	status.revents = 0;

	poll(&status, 1, 0);

	if(status.revents & POLLIN)
		return -1;	/* data available */

	return 0;	/* no data available */
#else
	return -1;
#endif
}

int socket_writeable(void){
#ifndef UCSIMM
  struct pollfd status;
  
  status.fd = sock;
  status.events = POLLOUT;
  status.revents = 0;
  
  poll(&status, 1, 0);
  
  if(status.revents & POLLOUT)
    return -1;	/* space available */
  
  return 0;	/* no space available */
#else
  return -1;
#endif
}
