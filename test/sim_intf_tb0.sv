module sim_intf_tb;
	logic clk;
	logic miss;
	longint pc_try;
	longint pc_factual;
	int inst;

	sim_intf #(.START_PC('h00001000), .INPUT_ELF("hello"), .SIM_PORT(12400))
		dut(clk, pc_try, pc_factual, inst, miss);

	initial
	begin
		if ($test$plusargs("DUMP_FSDB")) begin
			$display("Dump FSDB: on");
			$fsdbDumpfile("sim_intf_tb.fsdb");
			$fsdbDumpvars("+all");
		end
		else begin
			$display("Dump FSDB: off");
		end

		$display("Start checking PCs");
		clk = 0; pc_try = 'h0000000000001000; #50; clk = 1; #25; if (miss) $display("%x miss %x", pc_try, pc_factual); else $display("%x ok %x %x", pc_try, pc_factual, inst); #25;
		clk = 0; pc_try = 'h0000000000001004; #50; clk = 1; #25; if (miss) $display("%x miss %x", pc_try, pc_factual); else $display("%x ok %x %x", pc_try, pc_factual, inst); #25;
		clk = 0; pc_try = 'h00000000AAAA1008; #50; clk = 1; #25; if (miss) $display("%x miss %x", pc_try, pc_factual); else $display("%x ok %x %x", pc_try, pc_factual, inst); #25;
		clk = 0; pc_try = 'h00000000BBBB1008; #50; clk = 1; #25; if (miss) $display("%x miss %x", pc_try, pc_factual); else $display("%x ok %x %x", pc_try, pc_factual, inst); #25;
		clk = 0; pc_try = 'h0000000000001008; #50; clk = 1; #25; if (miss) $display("%x miss %x", pc_try, pc_factual); else $display("%x ok %x %x", pc_try, pc_factual, inst); #25;
		clk = 0; pc_try = 'h000000000000100C; #50; clk = 1; #25; if (miss) $display("%x miss %x", pc_try, pc_factual); else $display("%x ok %x %x", pc_try, pc_factual, inst); #25;
		clk = 0; pc_try = 'h00000000CCCC1010; #50; clk = 1; #25; if (miss) $display("%x miss %x", pc_try, pc_factual); else $display("%x ok %x %x", pc_try, pc_factual, inst); #25;
		clk = 0; pc_try = 'h00000000DDDD1010; #50; clk = 1; #25; if (miss) $display("%x miss %x", pc_try, pc_factual); else $display("%x ok %x %x", pc_try, pc_factual, inst); #25;
		clk = 0; pc_try = 'h0000000000001010; #50; clk = 1; #25; if (miss) $display("%x miss %x", pc_try, pc_factual); else $display("%x ok %x %x", pc_try, pc_factual, inst); #25;

		$finish();
	end

endmodule
