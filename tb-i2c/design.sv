`include "i2c_eeprom.v"

interface dut_if;
	logic clk;
	logic sda;
endinterface

// Wrap to a unified module name
module dut(dut_if dif);

	i2c_eeprom dut_inst(
		.clk(dif.clk),
		.sda(dif.sda)
	);

endmodule
