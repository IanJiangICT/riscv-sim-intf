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
typedef struct __attribute__((packed)) insn_info {
	unsigned long len;
	unsigned long insn;
	unsigned char ext;
	unsigned char type;
	char disasm[INSN_DISASM_MAX_LEN];
} insn_info_t;

#endif
