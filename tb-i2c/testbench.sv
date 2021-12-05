`include "uvm_macros.svh"
`include "tb_pkg_i2c_eeprom.sv"

module top
	import uvm_pkg::*;
	import tb_pkg_i2c_eeprom::*;

	dut_if i_dut_if();
	dut i_dut(.dif(i_dut_if));

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
