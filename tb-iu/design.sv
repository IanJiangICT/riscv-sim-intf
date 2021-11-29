`include "iu_design0.v"

interface dut_if;
	logic clk;
	logic miss;
	longint pc_curr;
	int insn_curr;
	longint pc_pre;
	logic pc_pre_oe;
endinterface

// Wrap to a unified module name
module dut(dut_if dif);

	iu dut_inst(
		.clk(dif.clk),
		.miss(dif.miss),
		.pc_curr(dif.pc_curr),
		.insn_curr(dif.insn_curr),
		.pc_pre(dif.pc_pre),
		.pc_pre_oe(dif.pc_pre_oe)
	);

endmodule
