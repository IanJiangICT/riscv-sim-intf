module i2c_eeprom(input clk, inout sda)
	parameter I2C_SLV_ADDR = 7'h10;
	parameter MEM_CAP = 8'h80;

	reg [7:0] i2c_slv_addr;
	reg [7:0] memory[(MEM_CAP-1):0];
	reg [7:0] mem_addr;

	initial begin
		int i;
		i2c_slv_addr = I2C_SLV_ADDR;
		mem_addr = 8'h00;
		for (i = 0; i < MEM_CAP; i++) begin
			memory[i] = 8'h00;
		end
	end

	// TODO Sequential blocks
endmodule
