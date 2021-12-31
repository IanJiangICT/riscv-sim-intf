#include <stdio.h>
#include "sc_func.h"

/* Test sc_save_state() and sc_recover_state() */

int main(int argc, char **argv)
{
	char elf[] = "./hello";
	unsigned int port = 8600;
	unsigned long long npc;
	unsigned long long pc;
	unsigned long long insn;
	int ret;
	int i;

	ret = sc_init_sim(elf, port);
	if (ret < 0) {
		return -1;
	}
	printf("SC Test: Init\n");

	for (i = 0; i < 8; i++) {
		printf("SC Test: sc_run_next %d\n", i);
		ret = sc_run_next(&npc, &pc, &insn);
		if (ret < 0) { printf("SC Test: Error. sc_run_next ret = %d\n", ret); return -1; }
		printf("SC Test R: npc = 0x%016llx, pc = 0x%016llx, insn = 0x%016llx\n", npc, pc, insn);
		ret = sc_save_state();
		if (ret < 0) { printf("SC Test: Error. sc_save_state ret = %d\n", ret); return -1; }
	}

	return 0;
}
