#include <stdio.h>
#include "sc_func.h"

char code_stream[] = {
0x97, 0x02, 0x00, 0x00, 0x93, 0x82, 0xc2, 0x02, 0x73, 0x90, 0x52, 0x30, 0xf3, 0x22, 0x40, 0xf1,
0xaa, 0x02, 0x17, 0x21, 0x00, 0x00, 0x13, 0x01, 0xe1, 0x49, 0x16, 0x91, 0x73, 0x25, 0x40, 0xf1,
0x19, 0xe1, 0x6f, 0x00, 0xc0, 0x0e, 0x73, 0x00, 0x50, 0x10, 0xf5, 0xbf, 0x19, 0x71, 0x06, 0xe0,
0x2a, 0xe4, 0x2e, 0xe8, 0x32, 0xec, 0x36, 0xf0, 0x3a, 0xf4, 0x3e, 0xf8, 0x42, 0xfc, 0xc6, 0xe0,
0x96, 0xe4, 0x9a, 0xe8, 0x9e, 0xec, 0xf2, 0xf0, 0xf6, 0xf4, 0xfa, 0xf8, 0xfe, 0xfc, 0x0a, 0x85,
0xf3, 0x25, 0x20, 0x34, 0x73, 0x26, 0x10, 0x34, 0xef, 0x00, 0x20, 0x10, 0x82, 0x60, 0x22, 0x65,
0xc2, 0x65, 0x62, 0x66, 0x82, 0x76, 0x22, 0x77, 0xc2, 0x77, 0x62, 0x78, 0x86, 0x68, 0xa6, 0x62,
0x46, 0x63, 0xe6, 0x63, 0x06, 0x7e, 0xa6, 0x7e, 0x46, 0x7f, 0xe6, 0x7f, 0x09, 0x61, 0x73, 0x00,
0x20, 0x30, 0x41, 0x11, 0x06, 0xe4, 0x97, 0x17, 0x00, 0x00, 0x93, 0x87, 0xa7, 0xf7, 0x17, 0x17,
0x00, 0x00, 0x13, 0x07, 0xa7, 0xf7, 0x63, 0x73, 0xf7, 0x00, 0xba, 0x87, 0x17, 0x25, 0x00, 0x00,
0x13, 0x05, 0x45, 0xfc, 0x17, 0x27, 0x00, 0x00, 0x23, 0x3e, 0xf7, 0xf6, 0xef, 0x00, 0xa0, 0x03,
0xa2, 0x60, 0x17, 0x25, 0x00, 0x00, 0x13, 0x05, 0x65, 0xfc, 0x41, 0x01, 0x3d, 0xa8, 0x41, 0x11,
0x17, 0x05, 0x00, 0x00, 0x13, 0x05, 0x85, 0x4f, 0x06, 0xe4, 0xef, 0x00, 0xe0, 0x21, 0xa2, 0x60,
0x01, 0x45, 0x41, 0x01, 0x82, 0x80, 0x02, 0x90, 0x01, 0x45, 0x82, 0x80, 0x02, 0x90, 0x01, 0x45,
0x82, 0x80, 0x02, 0x90, 0x01, 0xa0, 0x03, 0x33, 0x05, 0x00, 0x97, 0x27, 0x00, 0x00, 0x23, 0xbb,
0xa7, 0xf0, 0x63, 0x03, 0x03, 0x00, 0x02, 0x83, 0x82, 0x80, 0x03, 0x33, 0x05, 0x00, 0x97, 0x27,
0x00, 0x00, 0x23, 0xb5, 0xa7, 0xf0, 0x63, 0x03, 0x03, 0x00, 0x02, 0x83, 0x82, 0x80, 0x01, 0x11,
0x22, 0xe8, 0x17, 0x25, 0x00, 0x00, 0x13, 0x05, 0xe5, 0xf8, 0x17, 0x34, 0x00, 0x00, 0x13, 0x04,
0x64, 0xf9, 0x97, 0x07, 0x00, 0x00, 0x93, 0x87, 0xe7, 0x49, 0x33, 0x06, 0xa4, 0x40, 0x81, 0x45,
0x06, 0xec, 0x3e, 0xe0, 0x02, 0xe4, 0xef, 0x00, 0x00, 0x18, 0xef, 0xf0, 0x9f, 0xf4, 0x97, 0x05,
0x00, 0x08, 0x93, 0x85, 0x25, 0xec, 0x81, 0x8d, 0x22, 0x85, 0xef, 0x00, 0xa0, 0x14, 0x8a, 0x85,
0x05, 0x45, 0xef, 0xf0, 0xdf, 0xf6, 0xef, 0x00, 0xc0, 0x12, 0x17, 0x23, 0x00, 0x00, 0x03, 0x33,
0x63, 0xf4, 0x63, 0x03, 0x03, 0x00, 0x02, 0x83, 0x41, 0x11, 0xae, 0x86, 0x32, 0x87, 0x97, 0x05,
0x00, 0x00, 0x93, 0x85, 0xa5, 0x45, 0x13, 0x06, 0xf0, 0x03, 0x17, 0x05, 0x00, 0x00, 0x13, 0x05,
0xe5, 0x46, 0x06, 0xe4, 0xef, 0x00, 0x40, 0x14, 0x7d, 0x55, 0xef, 0x00, 0x80, 0x0f, 0x41, 0x11,
0x17, 0x25, 0x00, 0x00, 0x13, 0x05, 0x05, 0xe8, 0x06, 0xe4, 0x22, 0xe0, 0xef, 0x00, 0x00, 0x3b,
0x17, 0x17, 0x00, 0x00, 0x03, 0x67, 0x07, 0xe6, 0x97, 0x17, 0x00, 0x00, 0x83, 0xe7, 0xc7, 0xe5,
0x82, 0x17, 0x5d, 0x8f, 0x13, 0x15, 0x07, 0x01, 0x05, 0x46, 0x61, 0x93, 0x97, 0x16, 0x00, 0x00,
0x93, 0x86, 0x46, 0xe4, 0x63, 0x17, 0xc7, 0x00, 0xc1, 0x93, 0x93, 0xf7, 0xf7, 0x0f, 0x41, 0x81,
0x91, 0xc3, 0x7d, 0x55, 0x13, 0x74, 0xf5, 0x0f, 0x19, 0xc8, 0x9c, 0x46, 0x81, 0x27, 0x9d, 0xe3,
0x97, 0x17, 0x00, 0x00, 0x23, 0xa4, 0x07, 0xe2, 0xb7, 0x07, 0x00, 0x01, 0xdc, 0xc6, 0x17, 0x25,
0x00, 0x00, 0x13, 0x05, 0x25, 0xe2, 0xef, 0x00, 0x80, 0x38, 0xa2, 0x60, 0x22, 0x85, 0x02, 0x64,
};

int main(int argc, char **argv)
{
	char elf[] = "./hello";
	unsigned int port = 8600;
	int ret;

	int i;
	int j;
	int stream_offset;
	int code_len;
	char *code_data;
	insn_info_t insn_list[16];
	int insn_max = sizeof(insn_list)/sizeof(insn_info_t);
	int insn_cnt;

	//ret = sc_init_sim(elf, port);
	//if (ret < 0) {
		//return -1;
	//}
	printf("SC Test: Init\n");

	stream_offset = 0;
	code_len = 64;
	for (i = 0; i < 3; i++) {
		code_data = code_stream + stream_offset;
		insn_cnt = sc_decode(code_len, code_data, insn_max, insn_list);
		for (j = 0; j < insn_cnt; j++) {
			printf("[%d][%d] %d %lx %s\n", i, j, insn_list[j].len, insn_list[j].insn, insn_list[j].disasm);
		}
		stream_offset += code_len;
	}

	return 0;
}