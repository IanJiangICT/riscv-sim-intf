#ifndef SC_TYPES_H
#define SC_TYPES_H

#define INSN_DISASM_MAX_LEN 64
typedef struct insn_info {
	unsigned int len;
	unsigned long insn;
	char disasm[INSN_DISASM_MAX_LEN];
} insn_info_t;

#endif
