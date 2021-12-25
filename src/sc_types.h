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

#define INSN_DISASM_MAX_LEN 64
typedef struct insn_info {
	unsigned int len;
	unsigned long insn;
	char disasm[INSN_DISASM_MAX_LEN];
} insn_info_t;

#endif
