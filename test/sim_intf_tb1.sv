module sim_intf_tb;
	logic clk;
	logic miss;
	longint pc_try;
	longint pc_factual;
	int insn;

	int i = 0;
	longint pc_list[] = {
		'h0000000080000000,
		'h0000000080000004,
		'h00000000AAAA0008,
		'h00000000BBBB0008,
		'h0000000080000008,
		'h000000008000000C,
		'h00000000CCCC0010,
		'h00000000DDDD0010,
		'h0000000080000010,
		'h0000000080000014,
		'h0000000080000018,
		'h000000008000001C,
		'h0000000080000020,
		'h0000000080000024,
		'h0000000080000028,
		'h000000008000002C,
		'h0000000080000030,
		'h000000008000018C,
		'h0000000080000190
	};

	sim_intf #(.START_PC('h80000000), .INPUT_ELF("hello")) dut(clk, pc_try, pc_factual, insn, miss);

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
	end

	always begin
		#50;
		clk = ~clk;
	end

	always_ff @(posedge clk) begin
		pc_try = pc_list[i];
		i = i + 1;
		if (i >= $size(pc_list))
			$finish();
	end

	always_ff @(negedge clk) begin
		if (miss)
			$display("%x miss %x", pc_try, pc_factual);
		else
			$display("%x ok, %x, %x", pc_try, pc_factual, insn);
	end

endmodule
