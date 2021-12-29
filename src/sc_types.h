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

typedef struct __attribute__((packed)) {
	uint64_t pc;
	uint64_t npc;
	uint16_t log_cnt_r; // Reg
	uint16_t log_cnt_l; // Load
	uint16_t log_cnt_s; // Store
	uint16_t padding;
	uint64_t log_data[0]; // Following: <reg, val> x log_cnt_r, <addr, val, size> x log_cnt_l, <addr, val, size> x log_cnt_s
} exe_record_t;

#endif
