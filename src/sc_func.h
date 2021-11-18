#ifndef _SC_FUNC_H
#define _SC_FUNC_H

int sc_init(char *host, unsigned int port);
int sc_run_next(unsigned long long *npc, unsigned long long *pc, unsigned long long *insn);

#endif
