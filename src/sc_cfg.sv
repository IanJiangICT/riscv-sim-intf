package sc_cfg;

	// For decoder
	parameter DEC_DISCARD_ISTREAM_REMNANT = 1;
	parameter DEC_INSN_STREAM_BITS= 272;
	parameter DEC_INSN_STREAM_CAP = (DEC_INSN_STREAM_BITS / 8); // in Byte
	parameter DEC_INSN_LEN_OFFSET = 0; // in Byte. Ref to struct insn_info in sc_types.h
	parameter DEC_INSN_LEN_SIZE   = 8;
	parameter DEC_INSN_INS_OFFSET = (DEC_INSN_LEN_OFFSET + DEC_INSN_LEN_SIZE);
	parameter DEC_INSN_INS_SIZE   = 8;
	parameter DEC_INSN_EXT_OFFSET = (DEC_INSN_INS_OFFSET + DEC_INSN_INS_SIZE);
	parameter DEC_INSN_EXT_SIZE   = 1;
	parameter DEC_INSN_TYP_OFFSET = (DEC_INSN_EXT_OFFSET + DEC_INSN_EXT_SIZE);
	parameter DEC_INSN_TYP_SIZE   = 1;
	parameter DEC_INSN_DIS_OFFSET = (DEC_INSN_TYP_OFFSET + DEC_INSN_TYP_SIZE);
	parameter DEC_INSN_DIS_SIZE   = 64;
	parameter DEC_INSN_INFO_SIZE  = (DEC_INSN_DIS_OFFSET + DEC_INSN_DIS_SIZE);
	parameter DEC_INSN_LIST_CAP = 16;
	parameter DEC_INSN_LIST_SIZE = (DEC_INSN_INFO_SIZE * DEC_INSN_LIST_CAP);

endpackage
