module iu(
	input clk,
	input miss,
	input reg[63:0] pc_curr,
	output reg[63:0] pc_pre,
	output pc_pre_oe);

	parameter WORK_PERIOD = 6; // Clocks needed to do a prediction
 
	localparam [2:0]
		IDLE  = 3'b001,
		START = 3'b010,
		END   = 3'b100;

	reg[2:0] state;
	reg[7:0] cnt;
	reg[63:0] npc;

	initial begin
		state = IDLE;
		cnt = 0;
		npc = 0;
	end

	assign pc_pre = (state == END) ? npc : 0;
	assign pc_pre_oe = (state == END) ? 1 : 0;

	always @(posedge clk) begin
		case (state)
			IDLE:
			if (miss) begin
				state <= START;
				npc <= dif.pc_curr + 4;
			end else begin
				state <= START;
				npc <= npc + 4;
			end
			START:
			if (cnt >= WORK_PERIOD) begin
				state <= END;
			end else begin
				state <= START;
			end
			END:
				state <= IDLE;
		endcase
	end

	always @(posedge clk) begin
		cnt = (state == IDLE) ? 0 : (cnt + 1);
	end
endmodule

