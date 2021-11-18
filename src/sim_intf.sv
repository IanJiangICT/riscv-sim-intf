import "DPI-C" function int sc_init(input string host, input int port);
import "DPI-C" function int sc_init_sim(input string elf, input int port);
import "DPI-C" function int sc_run_next(output longint npc, output longint pc, output longint insn);

module sim_intf(
	input clk,
	input longint next_pc_check,
	output longint next_pc,
	output int next_insn,
	output logic miss);
	
	parameter START_PC = 64'h80000000;
	parameter INPUT_ELF= "";	// "hello" for example
	parameter SIM_PORT = 12300; // or 12400, 12500, 12600

	// Current state but viewed as Next for user
	longint npc;
	longint cpc;
	longint ci;

	initial
	begin
		int ret;
		string elf = INPUT_ELF;

		if (elf.len() > 0)
			ret = sc_init_sim(elf, SIM_PORT);
		else
			ret = sc_init("127.0.0.1", SIM_PORT);
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

		longint new_npc;
		longint new_cpc;
		longint new_ci;
		int ret;

		if (pc_i == cpc) begin
			ret = sc_run_next(new_npc, new_cpc, new_ci);
			if (ret < 0) begin
				$display("Error: Failed sc_run_next");
				$finish();
			end
			m = 0;
			npc <= new_npc;
			cpc <= new_cpc;
			ci <= new_ci;
		end
		else begin
			m = 1;
		end
		pc_o = cpc;
		i = ci;
	endfunction;

endmodule
