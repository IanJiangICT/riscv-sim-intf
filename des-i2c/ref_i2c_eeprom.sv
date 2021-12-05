`include "I2cEeprom.svh"

module ref_i2c_eeprom(input logic scl, inout sda)
	parameter I2C_SLV_ADDR = 7'h10;
	parameter MEM_CAP = 8'h80;

	I2cEeprom inst;

	initial begin
		inst = new(I2C_SLV_ADDR, MEM_CAP);
	end
	// TODO Sequential blocks base on calling I2cEeprom::functions
endmodule
