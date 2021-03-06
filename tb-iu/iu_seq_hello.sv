`include "iu_trans.svh"

class iu_seq_hello extends uvm_sequence#(iu_trans);
	`uvm_object_utils(iu_seq_hello);

	function new(string name = "");
		super.new(name);
	endfunction

	task body;
		longint pc_curr = 'h1000;
		longint pc_next;
		int pc_index = 0;
		int i;
		logic curr_missed = 1; // The 1st instruction is always missed.

		`uvm_info("Seq", "body", UVM_DEBUG);
		for (i = 0; i< 128; i++) begin
				pc_next = pc_curr + 4;
				req = iu_trans::type_id::create("req");
				req.miss = curr_missed;
				req.pc_curr = pc_curr;

				start_item(req);

				finish_item(req);
				if (req.pc_pre == pc_next) begin
					`uvm_info("Seq", $sformatf("Hit next pc %h", pc_next), UVM_DEBUG);
					curr_missed = 0;
				end else begin
					curr_missed = 1;
				end
				// Advance PC
				pc_index++;
				if (pc_index % 6 == 0) pc_curr += 64; // One branch in a group of instructions
				else pc_curr += 4;
		end
	endtask
endclass
