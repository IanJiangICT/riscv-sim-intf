#ifndef SC_TYPES_H
#define SC_TYPES_H

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
typedef struct insn_info {
	unsigned int len;
	unsigned long insn;
	char disasm[INSN_DISASM_MAX_LEN];
} insn_info_t;

#endif
