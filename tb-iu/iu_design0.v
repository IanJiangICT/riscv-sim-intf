module iu(
	input clk,
	input miss,
	input reg[63:0] pc_curr,
	input reg[31:0] insn_curr,
	output reg[63:0] pc_pre,
	output pc_pre_oe);

	parameter WORK_PERIOD = 6; // Clocks needed to do a prediction
 
	localparam [2:0]
		IDLE  = 3'b001,
		START = 3'b010,
		END   = 3'b100;

	reg[2:0] state, nstate;
	reg[7:0] cnt;
	reg[63:0] cpc, npc;
	reg[31:0] insn;

	initial begin
		state = IDLE;
		cnt = 0;
		npc = 0;
	end

	assign pc_pre = (state == END) ? npc : 0;
	assign pc_pre_oe = (state == END) ? 1 : 0;

	always @(*) begin
		case (state)
			IDLE:
			if (miss) begin
				nstate = START;
				cpc = dif.pc_curr;
				insn = dif.insn_curr;
			end else begin
				nstate = START;
				cpc = npc;
			end
			START:
			if (cnt >= WORK_PERIOD) begin
				nstate = END;
			end else begin
				nstate = START;
			end
			END:
				nstate = IDLE;
		endcase
	end

	always @(posedge clk) begin
		state <= nstate;
		cnt <= (state == IDLE) ? 0 : (cnt + 1);
		npc <= cpc + 4; // Simple prediction
	end

endmodule

