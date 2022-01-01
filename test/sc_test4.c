#include <stdio.h>
#include "sc_func.h"

#define ELF_ENTYR_PC 0x80000000
#define SKIP_INSN_CNT 32
#define PC_OFFSET_V0_END	0xa8
#define PC_OFFSET_JUMP_V1	0xac
#define PC_OFFSET_JUMP_CHECK	0xb8
#define PC_OFFSET_V1_END	0xcc

/* Test sc_save_state() and sc_recover_state() */

int main(int argc, char **argv)
{
	char elf[] = "mem";
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

	printf("SC Test: Run until entry point at PC 0x%llx\n", (unsigned long long)ELF_ENTYR_PC);
	do {
		ret = sc_run_next(&npc, &pc, &insn);
		if (ret < 0) { printf("SC Test: Error. sc_run_next ret = %d\n", ret); return -1; }
	} while (pc < ELF_ENTYR_PC);

	printf("SC Test: Skip %d instructions\n", SKIP_INSN_CNT);
	for (i = 0; i < SKIP_INSN_CNT; i++) {
		ret = sc_run_next(&npc, &pc, &insn);
		if (ret < 0) { printf("SC Test: Error. sc_run_next ret = %d\n", ret); return -1; }
	}

	printf("SC Test: Go throuth value 0 and 1\n");
	do {
		ret = sc_run_next(&npc, &pc, &insn);
		printf("SC Test: Save state at PC 0x%llx NPC 0x%llx\n", pc, npc);
		if (ret < 0) { printf("SC Test: Error. sc_run_next ret = %d\n", ret); return -1; }
		ret = sc_save_state();
		if (ret < 0) { printf("SC Test: Error. sc_save_state ret = %d\n", ret); return -1; }
	} while (pc < (ELF_ENTYR_PC + PC_OFFSET_V1_END));

	/* To be continued */
	printf("SC Test: Go back to end of value 0\n");
	printf("SC Test: Force PC to check (NOT modify value 1)\n");
	return 0;
}
