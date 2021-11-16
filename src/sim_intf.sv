import "DPI-C" function int sc_init(input string host, input int port);
import "DPI-C" function int sc_run(output longint npc);

module sim_intf(
	input clk,
	input longint pc_in,
	output longint pc_out,
	output logic miss);
	
	longint npc;

	initial
	begin
		int ret;
		ret = sc_init("127.0.0.1", 8765);
		if (ret < 0) begin
			$display("Error: Failed sc_init");
			$finish();
		end
	end

	always_ff @(posedge clk) begin
		check_pc(pc_in, pc_out, miss);
	end

	function void check_pc;
		input longint pc_i;
		output longint pc_o;
		output logic m;

		int ret;

		if (pc_i == npc) begin
			ret = sc_run(npc);
			if (ret < 0) begin
				$display("Error: Failed sc_run");
				$finish();
			end
			m = 0;
		end
		else begin
			m = 1;
		end
		pc_o = npc;
	endfunction;

endmodule
