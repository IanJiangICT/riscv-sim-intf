`include "uvm_macros.svh"
`include "iu_tb_pkg.sv"

module top
	import uvm_pkg::*;
	import iu_tb_pkg::*;

	dut_if i_dut_if();
	dut	i_dut(.dif(i_dut_if));

	initial begin
		i_dut_if.clk = 0;
		forever #5 i_dut_if.clk = ~i_dut_if.clk;
	end

	initial begin
		uvm_config_db#(virtual dut_if)::set(null, "*", "dut_vif", i_dut_if);
		run_test("dut_test0");
	end

	initial begin
		$dumpfile("dump.vcd");
		$dumpvars(0, top);
	end

endmodule
