import "DPI-C" function int sc_init(input string host, input int port);
import "DPI-C" function int sc_run_next(output longint npc, output longint pc, output longint insn);

module sim_intf(
	input clk,
	input longint next_pc_check,
	output longint next_pc,
	output int next_insn,
	output logic miss);
	
	parameter START_PC = 64'h80000000;

	// Current state but viewed as Next for user
	longint npc;
	longint cpc;
	longint ci;

	initial
	begin
		int ret;
		ret = sc_init("127.0.0.1", 8765);
		if (ret < 0) begin
			$display("Error: Failed sc_init");
			$finish();
		end

		$display("Run until expected PC %x", START_PC);
		do begin
				ret = sc_run_next(npc, cpc, ci);
				if (ret < 0) begin
					$display("Error: Failed sc_run_next");
					$finish();
				end
		end while (cpc != START_PC);
	end

	always_ff @(posedge clk) begin
		check_pc(next_pc_check, next_pc, next_insn, miss);
	end

	function void check_pc;
		input longint pc_i;
		output longint pc_o;
		output int i;
		output logic m;

		int ret;

		if (pc_i == cpc) begin
			ret = sc_run_next(npc, cpc, ci);
			if (ret < 0) begin
				$display("Error: Failed sc_run_next");
				$finish();
			end
			m = 0;
		end
		else begin
			m = 1;
		end
		pc_o = cpc;
		i = ci;
	endfunction;

endmodule
