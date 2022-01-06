#include <stdio.h>
#include "sc_func.h"

#define ELF_ENTYR_PC 0x80000000
#define SKIP_INSN_CNT 32
#define PC_OFFSET_V0_END	0xb0
#define PC_OFFSET_JUMP_V1	0xb4
#define PC_OFFSET_JUMP_V1_FORCE	(PC_OFFSET_JUMP_V1 + 4)
#define PC_OFFSET_V1_END	0xd4
#define PC_OFFSET_CHECK0_END	0xe4

#define PC_OFFSET_VA_END	0x124
#define PC_OFFSET_JUMP_VB	0x128
#define PC_OFFSET_JUMP_VB_FORCE	(PC_OFFSET_JUMP_VB + 4)
#define PC_OFFSET_VB_END	0x144
#define PC_OFFSET_CHECK1_END	0x158

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

	printf("SC Test: Go through value 0 and 1\n");
	do {
		ret = sc_run_next(&npc, &pc, &insn);
		if (ret < 0) { printf("SC Test: Error. sc_run_next ret = %d\n", ret); return -1; }
		printf("SC Test: Save state at PC 0x%llx NPC 0x%llx\n", pc, npc);
		ret = sc_save_state();
		if (ret < 0) { printf("SC Test: Error. sc_save_state ret = %d\n", ret); return -1; }
	} while (pc < (ELF_ENTYR_PC + PC_OFFSET_V1_END));

	pc = (unsigned long long)(ELF_ENTYR_PC + PC_OFFSET_JUMP_V1);
	printf("SC Test: Recover (go back) to jump after value 0. PC = 0x%llx\n", pc);
	ret = sc_recover_state(pc);
	if (ret < 0) { printf("SC Test: Error. sc_recover_state ret = %d\n", ret); return -1; }

	pc = (unsigned long long)(ELF_ENTYR_PC + PC_OFFSET_JUMP_V1_FORCE);
	printf("SC Test: Force to not jump (NOT modify value 1). PC = 0x%llx\n", pc);
	ret = sc_force_pc(pc);
	if (ret < 0) { printf("SC Test: Error. sc_force_pc ret = %d\n", ret); return -1; }
	printf("SC Test: Save state at PC 0x%llx\n", pc);
	ret = sc_save_state();
	if (ret < 0) { printf("SC Test: Error. sc_save_state ret = %d\n", ret); return -1; }

	printf("SC Test: Go through checking 0\n");
	do {
		ret = sc_run_next(&npc, &pc, &insn);
		printf("SC Test: Save state at PC 0x%llx NPC 0x%llx\n", pc, npc);
		if (ret < 0) { printf("SC Test: Error. sc_run_next ret = %d\n", ret); return -1; }
		ret = sc_save_state();
		if (ret < 0) { printf("SC Test: Error. sc_save_state ret = %d\n", ret); return -1; }
	} while (pc < (ELF_ENTYR_PC + PC_OFFSET_CHECK0_END));

	printf("SC Test: Go through value A and B\n");
	do {
		ret = sc_run_next(&npc, &pc, &insn);
		if (ret < 0) { printf("SC Test: Error. sc_run_next ret = %d\n", ret); return -1; }
		printf("SC Test: Save state at PC 0x%llx NPC 0x%llx\n", pc, npc);
		ret = sc_save_state();
		if (ret < 0) { printf("SC Test: Error. sc_save_state ret = %d\n", ret); return -1; }
	} while (pc < (ELF_ENTYR_PC + PC_OFFSET_VB_END));

	pc = (unsigned long long)(ELF_ENTYR_PC + PC_OFFSET_JUMP_VB);
	printf("SC Test: Recover (go back) to jump after value B. PC = 0x%llx\n", pc);
	ret = sc_recover_state(pc);
	if (ret < 0) { printf("SC Test: Error. sc_recover_state ret = %d\n", ret); return -1; }

	pc = (unsigned long long)(ELF_ENTYR_PC + PC_OFFSET_JUMP_VB_FORCE);
	printf("SC Test: Force to not jump (NOT modify value B). PC = 0x%llx\n", pc);
	ret = sc_force_pc(pc);
	if (ret < 0) { printf("SC Test: Error. sc_force_pc ret = %d\n", ret); return -1; }
	printf("SC Test: Save state at PC 0x%llx\n", pc);
	ret = sc_save_state();
	if (ret < 0) { printf("SC Test: Error. sc_save_state ret = %d\n", ret); return -1; }

	printf("SC Test: Go through checking 1\n");
	do {
		ret = sc_run_next(&npc, &pc, &insn);
		printf("SC Test: Save state at PC 0x%llx NPC 0x%llx\n", pc, npc);
		if (ret < 0) { printf("SC Test: Error. sc_run_next ret = %d\n", ret); return -1; }
		ret = sc_save_state();
		if (ret < 0) { printf("SC Test: Error. sc_save_state ret = %d\n", ret); return -1; }
	} while (pc < (ELF_ENTYR_PC + PC_OFFSET_CHECK1_END));
	return 0;
}
