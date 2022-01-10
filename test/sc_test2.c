#include <stdio.h>
#include "sc_func.h"

/* Test sc_force_pc() */

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

	for (i = 0; i < 64; i++) {
		printf("SC Test: sc_run_next %d\n", i);
		ret = sc_run_next(&npc, &pc, &insn);
		if (ret < 0) { printf("SC Test: Error. sc_run_next ret = %d\n", ret); return -1; }
		printf("SC Test R: npc = 0x%016llx, pc = 0x%016llx, insn = 0x%016llx\n", npc, pc, insn);

 		/* Forced to the other direction which jumps again */
		if (pc != 0x800001c0) continue;
		npc = pc + 4;

   		/* Forced to the other direction which returns */
		//if (pc != 0x800003ec) continue;
		//npc = pc + 4;

		printf("SC Test: Branch found. Force NPC to 0x%llx...\n", npc);
		ret = sc_force_pc(npc, NULL, NULL);
		if (ret < 0) { printf("SC Test: Error. sc_force_pc ret = %d\n", ret); return -1; }
	}

	return 0;
}
