module sim_intf_tb;
	logic clk;
	logic miss;
	longint pc_try;
	longint pc_factual;

	int i;
	longint pc_list[] = {
		'h0000000000000000,
		'h0000000000001000,
		'h0000000000001004,
		'h00000000AAAA1008,
		'h00000000BBBB1008,
		'h0000000000001008,
		'h000000000000100C,
		'h00000000CCCC1010,
		'h00000000DDDD1010,
		'h0000000000001010
	};

	sim_intf dut(clk, pc_try, pc_factual, miss);

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
		clk = 0;
		i = 0;
	end

	always begin
		#50;
		clk = ~clk;
	end

	always_ff @(posedge clk) begin
		pc = pc_list[i];
		i = i + 1;
		if (i >= $size(pc_list))
			$finish();
	end

	always_ff @(negedge clk) begin
		if (miss)
			$display("%x miss %x", pc_try, pc_factual);
		else
			$display("%x ok", pc_try);
	end

endmodule
