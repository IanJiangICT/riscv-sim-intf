#include <stdio.h>
#include "sc_func.h"

int main(int argc, char **argv)
{
	char host[] = "127.0.0.1";
	unsigned int port = 8765;
	unsigned long long npc;
	unsigned long long insn;
	int ret;

	ret = sc_init(host, port);
	if (ret < 0) {
		return -1;
	}
	printf("SC Test: Init\n");

	ret = sc_run(&npc, &insn); if (ret < 0) { return -1; } printf("SC Test R: npc = 0x%016llx, insn = 0x%016llx\n", npc, insn);
	ret = sc_run(&npc, &insn); if (ret < 0) { return -1; } printf("SC Test R: npc = 0x%016llx, insn = 0x%016llx\n", npc, insn);
	ret = sc_run(&npc, &insn); if (ret < 0) { return -1; } printf("SC Test R: npc = 0x%016llx, insn = 0x%016llx\n", npc, insn);
	ret = sc_run(&npc, &insn); if (ret < 0) { return -1; } printf("SC Test R: npc = 0x%016llx, insn = 0x%016llx\n", npc, insn);
	ret = sc_run(&npc, &insn); if (ret < 0) { return -1; } printf("SC Test R: npc = 0x%016llx, insn = 0x%016llx\n", npc, insn);
	ret = sc_run(&npc, &insn); if (ret < 0) { return -1; } printf("SC Test R: npc = 0x%016llx, insn = 0x%016llx\n", npc, insn);
	ret = sc_run(&npc, &insn); if (ret < 0) { return -1; } printf("SC Test R: npc = 0x%016llx, insn = 0x%016llx\n", npc, insn);
	ret = sc_run(&npc, &insn); if (ret < 0) { return -1; } printf("SC Test R: npc = 0x%016llx, insn = 0x%016llx\n", npc, insn);
	ret = sc_run(&npc, &insn); if (ret < 0) { return -1; } printf("SC Test R: npc = 0x%016llx, insn = 0x%016llx\n", npc, insn);
	
	return 0;
}
