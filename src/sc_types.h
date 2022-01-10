#ifndef SC_TYPES_H
#define SC_TYPES_H

#include <stdint.h>

enum insn_ext {
	RV0 = 0,
	RVA = 'A',
	RVB,
	RVC,
	RVD,
	RVE,
	RVF,
	RVG,
	RVH,
	RVI,
	RVJ,
	RVK,
	RVL,
	RVM,
	RVN,
	RVO,
	RVP,
	RVQ,
	RVR,
	RVS,
	RVT,
	RVU,
	RVV,
	RVW,
	RVX,
	RVY,
	RVZ
};

enum insn_type {
	ITB = 'B', // Branch
	ITC = 'C', // Compute
	ITF = 'F', // Fence
	ITM = 'M', // Memory load/store
	ITS = 'S', // System
	ITT = 'T', // Trap
	ITO = 'O'  // Others
};

#define INSN_DISASM_MAX_LEN 64
typedef struct __attribute__((packed)) {
	uint64_t len;
	uint64_t insn;
	uint8_t ext;
	uint8_t type;
	uint8_t disasm[INSN_DISASM_MAX_LEN];
} insn_info_t;

typedef struct __attribute__((packed)){
	uint64_t addr;
	uint64_t val;
	uint64_t size;
} mem_log_t;

struct __attribute__((packed)) rsp_save_state {
	uint16_t size;
	uint16_t xpr_size;
	uint16_t fpr_size;
	uint16_t log_l_size;
	uint16_t log_s_size;
	uint8_t other_data[0];
};

#define XPR_SIZE (sizeof(uint64_t) * 32)
#define FPR_SIZE (sizeof(uint64_t) * 64)
#define MEM_UPDATE_CAP 8
#define MEM_BLOCK_SHIFT 12
#define MEM_BLOCK_SIZE (1<<MEM_BLOCK_SHIFT)
#define MEM_BLOCK_CNT 8
#define STATE_CAP 8

struct mem_update {
	uint64_t addr;
	uint64_t val_old;
	uint64_t val_new;
	uint64_t size;
};

struct proc_state {
	uint64_t pc;
	uint64_t npc;
	uint8_t xpr_data[XPR_SIZE];
	uint8_t fpr_data[FPR_SIZE];
	struct mem_update mup_data[MEM_UPDATE_CAP];
	uint8_t mup_cnt;
};

struct mem_block {
	uint8_t data[MEM_BLOCK_SIZE];
	uint64_t base;
	uint8_t valid;
};

struct sim_context {
	uint64_t pc;
	uint64_t insn;
	uint64_t npc;
	struct proc_state states[STATE_CAP];
	int state_h, state_t;
	struct mem_block mems[MEM_BLOCK_CNT];
	int sock_fd;
};

#endif
