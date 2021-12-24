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

#include "sc_types.h"

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

#define CMD_STR_LEG_MAX 256
#define SIM_CMD_PREFIX  "spike --state-ctrl="
#define SIM_PORT_DEF    12300

static int start_sim(char *elf, unsigned int port)
{
	char cmd_str[CMD_STR_LEG_MAX] = {SIM_CMD_PREFIX};
	int ret;
	pid_t fork_pid;

	fork_pid = fork();
	if (fork_pid != 0) {
		return fork_pid;
	}

	sprintf(cmd_str, SIM_CMD_PREFIX"%u %s < /dev/zero > spike-run.log 2>&1", port, elf);
	printf("SC: Starting simulator in new process\n  %s\n", cmd_str);
	ret = system(cmd_str);
	if (ret != 0) {
		printf("SC Error: Failed to start Simulator process. ret = %d\n", ret);
		exit(ret);
	}
	exit(0);
}

int sc_init_sim(char *elf, unsigned int port)
{
	int ret;

	if (elf == NULL) return -1;
	if (port <= 0) port = SIM_PORT_DEF;

	ret = start_sim(elf, port);
	if (ret < 0) {
		return ret;
	}

	printf("SC: Wait for simualtor starting\n");
	sleep(2);

	return sc_init("127.0.0.1", port);
}

int sc_run_next(unsigned long long *npc, unsigned long long *pc, unsigned long long *insn)
{
	int fd;
	int tx_size;
	int rx_size;
	int rx_offset;
	char tx_buf[SOCK_BUF_SIZE];
	char rx_buf[SOCK_BUF_SIZE];
	int ret;

	if (npc == NULL) return -1;
	if (pc == NULL) return -1;
	if (insn == NULL) return -1;

	fd = sock_fd;

	tx_buf[0] = 'R';
	tx_size = 1;
	rx_size = sizeof(uint64_t); // Next PC
	rx_size += sizeof(uint64_t); // Instruction
	rx_size += sizeof(uint64_t); // PC

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
	if (ret != rx_size || (rx_buf[0] & 0x07) == 0x05)
		goto resend;

	rx_offset = 0;
	memcpy(npc, rx_buf + rx_offset, sizeof(*npc));
	rx_offset += sizeof(uint64_t);
	memcpy(pc, rx_buf + rx_offset, sizeof(*pc));
	rx_offset += sizeof(uint64_t);
	memcpy(insn, rx_buf + rx_offset, sizeof(*insn));
	
	return 1;
}

int sc_force_pc(unsigned long long pc)
{
	int fd;
	int tx_size;
	int rx_size;
	int rx_offset;
	char tx_buf[SOCK_BUF_SIZE];
	char rx_buf[SOCK_BUF_SIZE];
	int ret;
	unsigned long long actual_pc;

	fd = sock_fd;

	tx_buf[0] = 'F';
	tx_size = 1;
	memcpy(tx_buf + tx_size, &pc, sizeof(uint64_t));
	tx_size += sizeof(uint64_t);
	rx_size = sizeof(uint64_t); // Next PC
	rx_size += sizeof(uint64_t); // PC
	rx_size += sizeof(uint64_t); // Instruction

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
	if (ret != rx_size) {
		printf("SC Error: Failed to receive ret = %d\n", ret);
		return -1;
	}

	rx_offset = 0;
	rx_offset += sizeof(uint64_t);
	memcpy(&actual_pc, rx_buf + rx_offset, sizeof(actual_pc));
	if (actual_pc != pc) {
		printf("SC Error: Failed to force pc. Req 0x%llx Got 0x%llx\n", pc, actual_pc);
		return -1;
	}
	return 1;
}

int sc_decode(int code_len, char *code_data, int insn_max, insn_info_t *insn_list)
{
	int fd;
	int tx_size;
	int rx_size;
	int rx_offset;
	char tx_buf[SOCK_BUF_SIZE];
	char rx_buf[SOCK_BUF_SIZE];
	int ret;
	uint64_t cnt;
	uint64_t val_u64;
	int i;

	if (code_len <= 0) return -1;
	if (code_data == NULL) return -1;
	if (insn_max <= 0) return -1;
	if (insn_list == NULL) return -1;

	fd = sock_fd;

	tx_buf[0] = 'D';
	tx_size = 1;
	memcpy(tx_buf + tx_size, code_data, code_len);
	tx_size += code_len;

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
	rx_size = ret;
	if (rx_size < sizeof(uint64_t)) {
		printf("SC Error: Received entry count\n");
		return -1;
	}
	rx_offset = 0;
	memcpy(&cnt, rx_buf + rx_offset, sizeof(uint64_t));
	rx_offset += sizeof(uint64_t);

	for (i = 0; i < cnt; i++) {
		memcpy(&val_u64, rx_buf + rx_offset, sizeof(uint64_t));
		insn_list[i].len = (unsigned int)val_u64;
		rx_offset += sizeof(uint64_t);
		memcpy(&val_u64, rx_buf + rx_offset, sizeof(uint64_t));
		insn_list[i].insn= (unsigned long)val_u64;
		rx_offset += sizeof(uint64_t);
		memcpy(insn_list[i].disasm, rx_buf + rx_offset, INSN_DISASM_MAX_LEN);
		rx_offset += INSN_DISASM_MAX_LEN;
		insn_list[i].disasm[INSN_DISASM_MAX_LEN-1] = '\0';
	}
	return i;
}
