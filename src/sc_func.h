#ifndef _SC_FUNC_H
#define _SC_FUNC_H

#include "sc_types.h"

int sc_init(char *host, unsigned int port);
int sc_init_sim(char *elf, unsigned int port);
int sc_run_next(unsigned long long *npc, unsigned long long *pc, unsigned long long *insn);
int sc_force_pc(unsigned long long new_pc);
int sc_decode(int code_len, char *code_data, int insn_max, insn_info_t *insn_list);
int sc_save_state(void);
int sc_recover_state(unsigned long long pc);

unsigned char insn_match_extension(const char *disasm);
unsigned char insn_match_type(const char *disasm);

#endif
