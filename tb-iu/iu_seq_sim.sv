`include "iu_trans.svh"
`include "SimProxy.sv"

class iu_seq_sim extends uvm_sequence#(iu_trans);
	`uvm_object_utils(iu_seq_sim);

	SimProxy sim_proxy;

	function new(string name = "");
		super.new(name);
	endfunction

	function void build_phase(uvm_phase phase);
		`uvm_info("Seq", "build_phase", UVM_DEBUG);
	endfunction

	task body;
		int ret;
		longint pc_curr;
		longint pc_next;
		int insn_curr;
		int insn_next;

		int pc_index;
		logic curr_missed;

		`uvm_info("Seq", "Start", UVM_DEBUG);
		sim_proxy = new();
		ret = sim_proxy.InitSpikeLog("hello.log");
		if (ret != 1) begin
			`uvm_fatal("Seq", "Failed to initiate SimProxy");
		end

		ret = sim_proxy.RunNext(pc_next, insn_next);
		if (ret != 1) begin // End of log
			`uvm_fatal("Seq", "Failed to get 1st log with SimProxy");
		end
		pc_index = 0;
		curr_missed = 1; // The 1st instruction is always missed

		// Advance PC no matter prediction hit or miss.
		do begin
			pc_curr = pc_next;
			insn_curr = insn_next;
			pc_index++;
			ret = sim_proxy.RunNext(pc_next, insn_next);
			if (ret == 0) begin // End of log
				break;
			end else if (ret != 1) begin
				`uvm_fatal("Seq", "Error in getting log with SimProxy");
			end
			req = iu_trans::type_id::create("req");
			req.miss = curr_missed;
			req.pc_curr = pc_curr;
			req.insn_curr = insn_curr;

			start_item(req);
			finish_item(req);
			
			if (req.pc_pre == pc_next) begin
				`uvm_info("Seq", $sformatf("Hit next pc %h", pc_next), UVM_HIGH);
				curr_missed = 0;
			end else begin
				curr_missed = 1;
			end
		end while (1);

		`uvm_info("Seq", "End", UVM_DEBUG);
	endtask
endclass
