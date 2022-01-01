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

static struct sim_context sim = {0};

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
	sim.sock_fd = fd;
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

/*
 * Return: count of bytes totally received
 * Resulting rx_buf:
 *   2_byte_of_cnt + cnt_bytes_of_data
 */
static int sim_recv(char *rx_buf)
{
	int fd;
	int rx_size;
	int rx_offset;
	const int cnt_size = sizeof(uint16_t);
	int data_size;

	fd = sim.sock_fd;
	rx_offset = 0;
	while (1) {
		rx_size = recv(fd, rx_buf + rx_offset, cnt_size - rx_offset, 0);
#if 0 //def SC_DEBUG
		printf("<rx %4d at %4d:", rx_size, rx_offset);
		for (int i = 0; i < cnt_size; i++) {
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
	data_size = (int)(*((uint16_t *)rx_buf));
	if (data_size <= 0) {
		return cnt_size;
	} else if (cnt_size + data_size > SOCK_BUF_SIZE) {
		printf("SC Error: Recv buffer %d not large enough for %d data\n",
			   SOCK_BUF_SIZE, data_size);
		return -2;
	}

	rx_offset = cnt_size;
	while (1) {
		rx_size = recv(fd, rx_buf + rx_offset, cnt_size + data_size - rx_offset, 0);
#if 0 //def SC_DEBUG
		printf("<rx %4d at %4d:", rx_size, rx_offset);
		for (int i = 0; i < cnt_size+rx_offset+rx_size; i++) {
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
		} else if ((rx_size + rx_offset) < (cnt_size + data_size)) {
			rx_offset += rx_size;
			continue;
		} else {
			break;
		}
	}

	rx_size = cnt_size + data_size;
	return rx_size;
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
	int size_size = sizeof(uint16_t);

	if (npc == NULL) return -1;
	if (pc == NULL) return -1;
	if (insn == NULL) return -1;

	fd = sim.sock_fd;

	tx_buf[0] = 'R';
	tx_size = 1;
	tx_size += sizeof(*arg_size);
	*arg_size = 0;
	rx_size = size_size;
	rx_size += sizeof(uint64_t); // Next PC
	rx_size += sizeof(uint64_t); // Instruction
	rx_size += sizeof(uint64_t); // PC

resend:
	ret = send(fd, tx_buf, tx_size, 0);
	if (ret != tx_size) {
		printf("SC Error: Failed to send\n");
		return -1;
	}

	rx_size = sim_recv(rx_buf);
	if (rx_size < size_size) {
		return -1;
	}

	if ((rx_buf[size_size] & 0x07) == 0x05)
		goto resend;

	rx_offset = size_size;
	memcpy(npc, rx_buf + rx_offset, sizeof(*npc));
	rx_offset += sizeof(uint64_t);
	memcpy(pc, rx_buf + rx_offset, sizeof(*pc));
	rx_offset += sizeof(uint64_t);
	memcpy(insn, rx_buf + rx_offset, sizeof(*insn));
	sim.pc = *pc;
	sim.npc = *npc;

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
	int size_size = sizeof(uint16_t);
	unsigned long long actual_pc;

	fd = sim.sock_fd;

	tx_buf[0] = 'F';
	tx_size = 1;
	tx_size += sizeof(*arg_size);
	memcpy(tx_buf + tx_size, &pc, sizeof(uint64_t));
	tx_size += sizeof(uint64_t);
	*arg_size = tx_size - (1 + sizeof(*arg_size));

	ret = send(fd, tx_buf, tx_size, 0);
	if (ret != tx_size) {
		printf("SC Error: Failed to send\n");
		return -1;
	}

	rx_size = sim_recv(rx_buf);
	if (rx_size < size_size) {
		return -1;
	}

	// Response data: Next PC +  PC + Instruction
	if ((rx_size - size_size) != sizeof(uint64_t) * 3) {
		printf("SC Error: Not enough data in response\n");
		return -1;
	}
	rx_offset = size_size + sizeof(uint64_t);
	memcpy(&actual_pc, rx_buf + rx_offset, sizeof(actual_pc));
	if (actual_pc != pc) {
		printf("SC Error: Failed to force pc. Req 0x%llx Got 0x%llx\n", pc, actual_pc);
		return -1;
	}
	sim.pc = pc;
	rx_offset = size_size + sizeof(uint64_t);
	memcpy(&sim.npc, rx_buf + rx_offset, sizeof(actual_pc));

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
	int insn_cnt;
	int size_size = sizeof(uint16_t);
	uint64_t val_u64;
	int i;

	if (code_len < 0) return -1;
	if (code_data == NULL) return -1;
	if (insn_max <= 0) return -1;
	if (insn_list == NULL) return -1;

	fd = sim.sock_fd;

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
	for (int i = 0; i < tx_size; i++) {
		printf(" %02x", (unsigned char)tx_buf[i]);
	}
	printf(">\n");
#endif

	if (code_len == 0) { // No response to receive
		return 0;
	}

	rx_size = sim_recv(rx_buf);
	if (rx_size < size_size) {
		return -1;
	}

	/* Extract instruction information */
	insn_cnt = *((uint16_t *)rx_buf) / (sizeof(uint64_t) + sizeof(uint64_t) + INSN_DISASM_MAX_LEN);
	rx_offset = size_size;
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

static void inline _print_u64(void *data, int size)
{
	if (data == NULL) return;
	if (size <= 0) return;
	for (int i = 0; i < size/sizeof(uint64_t); i++) {
		printf(" %016lx", *((uint64_t *)data + i));
	}
	printf("\n");
}

#ifdef SC_DEBUG
static void state_print(int sn)
{
	int i;
	uint8_t *base;
	struct proc_state *s;
	if (sn < 0 || sn >= STATE_CAP) return;

	printf("State[%d]\n", sn);
	s = sim.states + sn;
	printf("  XPR:");
	base = s->xpr_data;
	for (i = 0; i < XPR_SIZE/sizeof(uint64_t); i++) {
		uint64_t val;
		val = ((uint64_t *)base)[i];
		if (val == 0) continue;
		printf(" %2d %016lx", i, val);
	}
	printf("\n");

	printf("  FPR:");
	base = s->fpr_data;
	for (i = 0; i < FPR_SIZE/sizeof(uint64_t); i += 2) {
		uint64_t val0, val1;
		val0 = ((uint64_t *)base)[i];
		val1 = ((uint64_t *)base)[i + 1];
		if (val0 == 0 && val1 == 0) continue;
		printf(" %2d %016lx %016lx", i, val0, val1);
	}
	printf("\n");

	printf("  Mem:\n");
	base = (uint8_t *)s->mup_data;
	for (i = 0; i < s->mup_cnt; i++) {
		printf("[%d] %d %016lx : %016lx -> %016lx\n", i,
			(int)s->mup_data[i].size,
			s->mup_data[i].addr,
			s->mup_data[i].val_old,
			s->mup_data[i].val_new);
	}

	return;
}
#endif

static int mem_get(uint64_t addr, uint8_t size, uint64_t *val)
{
	int i;
	int j;
	uint64_t base = addr >> MEM_BLOCK_SHIFT;
	uint64_t offset = addr & (MEM_BLOCK_SIZE - 1);

#ifdef SC_DEBUG
	printf("SC: Get memory for %016lx %d\n", addr, (int)size);
#endif
	for (i = 0; i < MEM_BLOCK_CNT; i++) {
		if (sim.mems[i].valid == 0) continue;
		if (sim.mems[i].base != base) continue;
		*val = 0;
		for (j = 0; j < size; j++) {
			if (offset + j >= MEM_BLOCK_SIZE) break;
			*val <<= 8;
			*val |= sim.mems[i].data[offset + j];
		}
		return j;
	}
	return 0;
}

static int mem_put(uint64_t addr, uint8_t size, uint64_t val)
{
	int i;
	int j;
	uint64_t base = addr >> MEM_BLOCK_SHIFT;
	uint64_t offset = addr & (MEM_BLOCK_SIZE - 1);

#ifdef SC_DEBUG
	printf("SC: Put memory for %016lx %d = %016lx\n", addr, (int)size, val);
#endif
	/* Search in existing blocks */
	for (i = 0; i < MEM_BLOCK_CNT; i++) {
		if (sim.mems[i].valid == 0) continue;
		if (sim.mems[i].base == base) break;
	}

	/* Create new block if necessary */
	if (i >= MEM_BLOCK_CNT) {
		for (i = 0; i < MEM_BLOCK_CNT; i++) {
			if (sim.mems[i].valid != 0) continue;
#ifdef SC_DEBUG
			printf("SC: New memory block %d for base %016lx\n", i, base << MEM_BLOCK_SHIFT);
#endif
			sim.mems[i].base = base;
			sim.mems[i].valid = 1;
			break;
		}
	}
	if (i >= MEM_BLOCK_CNT)
		return 0;

	/* Save new vale */
	for (j = 0; j < size; j++) {
		if (offset + j >= MEM_BLOCK_SIZE) break;
		sim.mems[i].data[offset + j] = val & 0xFF;
		val >>= 8;
	}
	return j;
}

static void state_push(struct rsp_save_state *rsp)
{
	int h,t;
	struct proc_state *s = NULL;
	uint8_t *base;
	mem_log_t *mlog;
	uint64_t addr;
	uint64_t size;
	uint64_t val_old;
	uint64_t val_new;
	int i;
	int ret;

	if (rsp == NULL) return;

#ifdef SC_DEBUG
	printf("State: %d + %d + %d + %d\n", rsp->xpr_size, rsp->fpr_size,
		    rsp->log_l_size, rsp->log_s_size);
#endif

	h = sim.state_h;
	t = sim.state_t;

	/* Save the new state */
	t = (t == (STATE_CAP - 1)) ? 0 : t + 1;
	h = (h == (STATE_CAP - 1)) ? 0 : h + 1;
	s = sim.states + h;
	s->pc = sim.pc;
	s->npc = sim.npc;
	base = rsp->other_data;
	memcpy(s->xpr_data, base, XPR_SIZE);
	base += rsp->xpr_size;
	memcpy(s->fpr_data, base, FPR_SIZE);
	base += rsp->fpr_size;
	/* Build memory updates based on previous memory state and new memory logs */
	mlog = (mem_log_t *)(base + rsp->log_l_size);
	s->mup_cnt = 0;
	for (i = 0; i < (rsp->log_s_size / sizeof(mem_log_t)); i++) {
		addr = mlog[i].addr;
		val_new = mlog[i].val;
		size = mlog[i].size;
#ifdef SC_DEBUG
		printf("SC: Check memory store log %d: %016lx %d %016lx\n", i, addr, (int)size, val_new);
#endif
		ret = mem_get(addr, (uint8_t)size, &val_old);
		if (ret <= 0) continue;
		if (ret != size) {
			printf("SC Error: Not complete data for memory store log %d\n", i);
			continue;
		}
		if (val_new == val_old) continue;
		if (s->mup_cnt >= MEM_UPDATE_CAP - 1) {
			printf("SC Error: No free update space for memory store log %d\n", i);
			continue;
		}
#ifdef SC_DEBUG
		printf("SC: Save memory store log %d to memory update %d\n", i, s->mup_cnt);
#endif
		s->mup_data[s->mup_cnt].addr = addr;
		s->mup_data[s->mup_cnt].val_new = val_new;
		s->mup_data[s->mup_cnt].val_old = val_old;
		s->mup_data[s->mup_cnt].size = size;
		s->mup_cnt += 1;
	}

	/* Sync new memory value */
	mlog = (mem_log_t *)(base);
	for (i = 0; i < ((rsp->log_l_size + rsp->log_s_size) / sizeof(mem_log_t)); i++) {
		addr = mlog[i].addr;
		val_new = mlog[i].val;
		size = mlog[i].size;
		ret = mem_put(addr, (uint8_t)size, val_new);
		if (ret != size) {
			printf("SC Error: Failed to save value for memory log %d\n", i);
			continue;
		}
	}

#ifdef SC_DEBUG
	state_print(h);
#endif
	sim.state_h = h;
	sim.state_t = t;

	return;
}

int sc_save_state(void)
{
	int fd;
	int tx_size;
	int rx_size;
	//int rx_offset;
	char tx_buf[SOCK_BUF_SIZE];
	char rx_buf[SOCK_BUF_SIZE];
	int ret;
	uint16_t *arg_size = (uint16_t *)(tx_buf + 1);
	int size_size = sizeof(uint16_t);
	struct rsp_save_state *rsp = (struct rsp_save_state *)rx_buf;

	fd = sim.sock_fd;

	tx_buf[0] = 'S';
	tx_size = 1;
	tx_size += sizeof(*arg_size);
	*arg_size = 0;

	ret = send(fd, tx_buf, tx_size, 0);
	if (ret != tx_size) {
		printf("SC Error: Failed to send\n");
		return -1;
	}

	rx_size = sim_recv(rx_buf);
	if (rx_size < size_size) {
		return -1;
	}
	if (rx_size  != (sizeof(*rsp) + rsp->xpr_size + rsp->fpr_size + rsp->log_l_size + rsp->log_s_size)) {
		printf("SC Error: Invalid data size in response. rx_size = %d\n", rx_size);
		return -1;
	}

	state_push(rsp);
	return 1;
}

int sc_recover_state(unsigned long long pc)
{
	return -1;
}
