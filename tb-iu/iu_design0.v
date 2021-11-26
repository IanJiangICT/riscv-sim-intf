module iu(
	input logic clk,
	input logic miss,
	input reg[63:0] pc_curr,
	output reg[63:0] pc_pre);
 
	reg[63:0] npc;
	reg[63:0] pc_pre_tmp1;
	reg[63:0] pc_pre_tmp2;

	initial begin
		npc = 0;
		pc_pre_tmp1 = 0;
		pc_pre_tmp2 = 0;
		pc_pre = 0;
	end
 
	always @(posedge clk) begin
		if (miss)
			npc <= dif.pc_curr + 4;
		else
			npc <= npc + 4;
 	
	end
 
	always @(posedge clk) begin
		pc_pre_tmp1 <= npc;
		pc_pre_tmp2 <= pc_pre_tmp1;
		pc_pre <= pc_pre_tmp2;
	end
endmodule

