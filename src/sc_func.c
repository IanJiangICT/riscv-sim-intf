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
#include "sc_func.h"

#define SOCK_BUF_SIZE 4096

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

static int sim_start(char *elf, unsigned int port)
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

	ret = sim_start(elf, port);
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
	uint16_t *arg_size = (uint16_t *)(tx_buf + 1);

	if (npc == NULL) return -1;
	if (pc == NULL) return -1;
	if (insn == NULL) return -1;

	fd = sock_fd;

	tx_buf[0] = 'R';
	tx_size = 1;
	tx_size += sizeof(*arg_size);
	*arg_size = 0;
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
	uint16_t *arg_size = (uint16_t *)(tx_buf + 1);
	unsigned long long actual_pc;

	fd = sock_fd;

	tx_buf[0] = 'F';
	tx_size = 1;
	tx_size += sizeof(*arg_size);
	memcpy(tx_buf + tx_size, &pc, sizeof(uint64_t));
	tx_size += sizeof(uint64_t);
	*arg_size = tx_size - (1 + sizeof(*arg_size));
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
	uint16_t *arg_size = (uint16_t *)(tx_buf + 1);
	uint16_t insn_cnt;
	int cnt_size = sizeof(insn_cnt);
	int list_size;
	uint64_t val_u64;
	int i;

	if (code_len < 0) return -1;
	if (code_data == NULL) return -1;
	if (insn_max <= 0) return -1;
	if (insn_list == NULL) return -1;

	fd = sock_fd;

	tx_buf[0] = 'D';
	tx_size = 1;
	tx_size += sizeof(*arg_size);

	if (code_len == 0) { // Request with zero length of code implies a new session
		*arg_size = 0;
	} else {
		memcpy(tx_buf + tx_size, code_data, code_len);
		tx_size += code_len;
		*arg_size = tx_size - (1 + sizeof(*arg_size));
	}

	ret = send(fd, tx_buf, tx_size, 0);
	if (ret != tx_size) {
		printf("SC Error: Failed to send\n");
		return -1;
	}
#ifdef SC_DEBUG
	printf("<tx %4d:", tx_size);
	for (i = 0; i < tx_size; i++) {
		printf(" %02x", (unsigned char)tx_buf[i]);
	}
	printf(">\n");
#endif

	if (code_len == 0) { // No response to receive
		return 0;
	}

	/* Receive instruction count */
	rx_offset = 0;
	while (1) {
		rx_size = recv(fd, rx_buf + rx_offset, cnt_size - rx_offset, 0);
#ifdef SC_DEBUG
		printf("<rx %4d at %4d:", rx_size, rx_offset);
		for (i = 0; i < cnt_size; i++) {
			printf(" %02x", (unsigned char)rx_buf[i]);
		}
		printf(">\n");
#endif
		if (rx_size < 0) {
			printf("SC Error: Failed to receive\n");
			return -1;
		} else if (rx_size == 0) {
			printf("SC Server disconnected\n");
			return -1;
		} else if ((rx_size + rx_offset) < cnt_size) {
			rx_offset += rx_size;
			continue;
		} else {
			break;
		}
	}
	memcpy(&insn_cnt, rx_buf, sizeof(insn_cnt));

	if (insn_cnt <= 0) {
		return 0;
	}

	/* Receive instruciton list */
	list_size = (sizeof(uint64_t) + sizeof(uint64_t) + INSN_DISASM_MAX_LEN) * insn_cnt;
	if (cnt_size + list_size > SOCK_BUF_SIZE) {
		printf("SC Error: Recv buffer %d not large enough for %d instructions\n",
			   SOCK_BUF_SIZE, insn_cnt);
	}
	rx_offset = cnt_size;
	while (1) {
		rx_size = recv(fd, rx_buf + rx_offset, cnt_size + list_size - rx_offset, 0);
#ifdef SC_DEBUG
		printf("<rx %4d at %4d:", rx_size, rx_offset);
		for (i = 0; i < cnt_size+rx_offset+rx_size; i++) {
			printf(" %02x", (unsigned char)rx_buf[i]);
		}
		printf(">\n");
#endif
		if (rx_size < 0) {
			printf("SC Error: Failed to receive\n");
			return -1;
		} else if (rx_size == 0) {
			printf("SC Server disconnected\n");
			return -1;
		} else if ((rx_size + rx_offset) < (cnt_size + list_size)) {
			rx_offset += rx_size;
			continue;
		} else {
			break;
		}
	}

	rx_size = cnt_size + list_size;
	rx_offset = cnt_size;
	for (i = 0; i < insn_cnt; i++) {
		memcpy(&val_u64, rx_buf + rx_offset, sizeof(uint64_t));
		insn_list[i].len = val_u64;
		rx_offset += sizeof(uint64_t);
		memcpy(&val_u64, rx_buf + rx_offset, sizeof(uint64_t));
		insn_list[i].insn= val_u64;
		rx_offset += sizeof(uint64_t);
		memcpy(insn_list[i].disasm, rx_buf + rx_offset, INSN_DISASM_MAX_LEN);
		rx_offset += INSN_DISASM_MAX_LEN;
		insn_list[i].disasm[INSN_DISASM_MAX_LEN-1] = '\0';
	}

	/* Detect instruction features */
	for (i = 0; i < insn_cnt; i++) {
		insn_list[i].ext = insn_match_extension((char *)insn_list[i].disasm);
		insn_list[i].type = insn_match_type((char *)insn_list[i].disasm);
	}

#ifdef SC_DEBUG
	printf("D %d instructions OK\n", i);
#endif
	return i;
}

struct imt_ext {
	unsigned char ext;
	int prefix_len;
	char *prefix_str;
	int feature_len;
	char *feature_str;
};

static const struct imt_ext lut_ext[] = {
	{ .ext = RVC, .prefix_len = 2, .prefix_str = "c.",  .feature_len = 0, .feature_str = ""},
	{ .ext = RVD, .prefix_len = 1, .prefix_str = "f",   .feature_len = 2, .feature_str = ".d"},
	{ .ext = RVQ, .prefix_len = 1, .prefix_str = "f",   .feature_len = 2, .feature_str = ".q"},
	{ .ext = RVF, .prefix_len = 1, .prefix_str = "f",   .feature_len = 0, .feature_str = ""},
	{ .ext = RVA, .prefix_len = 3, .prefix_str = "amo", .feature_len = 0, .feature_str = ""},
	{ .ext = RVA, .prefix_len = 2, .prefix_str = "lr",  .feature_len = 0, .feature_str = ""},
	{ .ext = RVA, .prefix_len = 2, .prefix_str = "sc",  .feature_len = 0, .feature_str = ""},
	{ .ext = RVM, .prefix_len = 3, .prefix_str = "mul", .feature_len = 0, .feature_str = ""},
	{ .ext = RVM, .prefix_len = 3, .prefix_str = "div", .feature_len = 0, .feature_str = ""},
	{ .ext = RVM, .prefix_len = 3, .prefix_str = "rem", .feature_len = 0, .feature_str = ""},
	{ .ext = RVV, .prefix_len = 1, .prefix_str = "v",   .feature_len = 0, .feature_str = ""},
	{ .ext = RVP, .prefix_len = 3, .prefix_str = "csr", .feature_len = 0, .feature_str = ""},
	{ .ext = RVP, .prefix_len = 3, .prefix_str = "wfi", .feature_len = 0, .feature_str = ""},
	{ .ext = RVP, .prefix_len = 3, .prefix_str = "mre", .feature_len = 0, .feature_str = ""},   // MRET
	{ .ext = RVS, .prefix_len = 3, .prefix_str = "sre", .feature_len = 0, .feature_str = ""},   // SRET
	{ .ext = RVS, .prefix_len = 3, .prefix_str = "sfe", .feature_len = 0, .feature_str = ""},   // SFENCE
	{ .ext = RVB, .prefix_len = 0, .prefix_str = "",    .feature_len = 0, .feature_str = ""},
	{ .ext = RV0, .prefix_len = 0, .prefix_str = "",    .feature_len = 0, .feature_str = ""},
};

unsigned char insn_match_extension(const char *disasm)
{
	int i = 0;
	unsigned char ext = RV0;
	if (disasm == NULL) return ext;

	do {
		if (lut_ext[i].prefix_len == 0) {
			break;
		}
		if (strncmp(disasm, lut_ext[i].prefix_str, lut_ext[i].prefix_len) != 0) {
			i++;
			continue;
		}
		if (lut_ext[i].feature_len == 0) {
			break;
		}
		if (strstr(disasm, lut_ext[i].feature_str) == NULL) {
			i++;
			continue;
		}
	} while (lut_ext[i].ext != RV0);

	ext = lut_ext[i].ext;
	return ext;
}

struct imt_type {
	unsigned char itype;
	int prefix_cnt;
	const char **prefix_list;
};

/* Lists of prefix strings of each type, excpet ITC (compute type).
   Note: All others are sorted to type ITC */
const char *imt_pl_m[] = {"l", "s", "fl", "fs", "c.l", "c.s", "lr.", "sc.", "vl", "vs"};
const char *imt_pl_b[] = {"b", "j", "c.b", "c.j"};
const char *imt_pl_t[] = {"wfi", "mret", "sret", "c.eb"};
const char *imt_pl_s[] = {"csr"};
const char *imt_pl_f[] = {"fen", "sfe"};

const static struct imt_type lut_type[] = {
	{ .itype = ITM, .prefix_cnt = sizeof(imt_pl_m)/sizeof(char *), .prefix_list = imt_pl_m},
	{ .itype = ITB, .prefix_cnt = sizeof(imt_pl_b)/sizeof(char *), .prefix_list = imt_pl_b},
	{ .itype = ITT, .prefix_cnt = sizeof(imt_pl_t)/sizeof(char *), .prefix_list = imt_pl_t},
	{ .itype = ITS, .prefix_cnt = sizeof(imt_pl_s)/sizeof(char *), .prefix_list = imt_pl_s},
	{ .itype = ITF, .prefix_cnt = sizeof(imt_pl_f)/sizeof(char *), .prefix_list = imt_pl_f},
};

unsigned char insn_match_type(const char *disasm)
{
	int i = 0;
	int j = 0;
	char *p = NULL;

	if (disasm == NULL) return ITO;
	if (strlen(disasm) <= 1) return ITO;

	for (i = 0; i < sizeof(lut_type)/sizeof(struct imt_type); i++) {
		for (j = 0; j < lut_type[i].prefix_cnt; j++) {
			p = strstr(disasm, lut_type[i].prefix_list[j]);
			if (p == disasm) {
				return lut_type[i].itype;
			}
		}
	}
	return ITC;
}
