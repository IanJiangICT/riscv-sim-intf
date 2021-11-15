#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>

#define SOCK_BUF_SIZE 1024

static int sock_fd;

int sc_init(char *host, unsigned int port)
{
	int fd;
	int ret;
	struct sockaddr_in addr;
	struct hostent *server;

	if (host == NULL) return -1;
	if (port <= 0) return -1;

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0) {
		printf("SC Error: Failed to create socket\n");
		return -1;
	}
	
	server = gethostbyname(host);
	if (server == NULL) {
		printf("SC Error: Failed to get server host %s\n", host);
		return -1;
	}
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	memcpy(&addr.sin_addr.s_addr, server->h_addr, server->h_length); 
	addr.sin_port = htons(port);

	ret = connect(fd, (struct sockaddr *)&addr, sizeof(addr));
	if (ret < 0) {
		printf("SC Error: Failed to connect\n");
		return -1;
	}

	printf("SC: Connected to %s %d\n", host, port);
	sock_fd = fd;
	return fd;
}

int sc_run(unsigned long long *npc)
{
	int fd;
	int tx_size;
	char tx_buf[SOCK_BUF_SIZE];
	char rx_buf[SOCK_BUF_SIZE];
	int ret;

	fd = sock_fd;

	tx_buf[0] = 'R';
	tx_size = 1;

resend:
	ret = send(fd, tx_buf, tx_size, 0);
	if (ret != tx_size) {
		printf("SC Error: Failed to send\n");
		return -1;
	}

	ret = recv(fd, rx_buf, sizeof(rx_buf), 0);
	if (ret < 0) {
		printf("SC Error: Failed to receive\n");
		return -1;
	}
	if (ret != sizeof(uint64_t) || rx_buf[0] & 0x3)
		goto resend;

	memcpy(npc, rx_buf, sizeof(*npc));
	
	return 1;
}
