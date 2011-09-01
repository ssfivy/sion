#define UDP_MAX_PAYLOAD 1460

int socket_fd(void);
int socket_init(char *local_host, int local_port,
                char *remote_host, int remote_port);
int socket_send(void *data, size_t length);
int socket_recv(void *data, size_t length);
int socket_readable(void);
int socket_writeable(void);	
