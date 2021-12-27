package sc_cfg;

	// For decoder
	parameter DEC_INSN_STREAM_BITS= 272;
	parameter DEC_INSN_STREAM_CAP = (DEC_INSN_STREAM_BITS / 8); // in Byte
	parameter DEC_INSN_INFO_SIZE = (8 + 8 + 1 + 1 + 64); // in Byte. Ref to struct insn_info in sc_types.h
	parameter DEC_INSN_LIST_CAP = 16;
	parameter DEC_INSN_LIST_SIZE = (DEC_INSN_INFO_SIZE * DEC_INSN_LIST_CAP);

endpackage
