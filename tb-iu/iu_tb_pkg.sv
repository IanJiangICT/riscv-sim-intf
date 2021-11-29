package iu_tb_pkg;
	import uvm_pkg::*;
	`include "iu_sequence.sv"
	`include "iu_driver.sv"

	class iu_agent extends uvm_agent;
		`uvm_component_utils(iu_agent);

		iu_driver driver;
		uvm_sequencer#(iu_trans) sequencer;

		function new(string name, uvm_component parent);
			super.new(name, parent);
		endfunction

		function void build_phase(uvm_phase phase);
			super.build_phase(phase);
			`uvm_info("Agent", "build_phase", UVM_DEBUG);
			driver = iu_driver::type_id::create("driver", this);
			sequencer = uvm_sequencer#(iu_trans)::type_id::create("sequencer", this);
		endfunction

		function void connect_phase(uvm_phase phase);
			`uvm_info("Agent", "connect_phase", UVM_DEBUG);
			driver.seq_item_port.connect(sequencer.seq_item_export);
		endfunction

		task run_phase(uvm_phase phase);
			`uvm_info("Agent", "run_phase", UVM_DEBUG);
			phase.raise_objection(this);
			begin
				iu_seq_hello seq;
				seq = iu_seq_hello::type_id::create("seq_hello");
				seq.start(sequencer);
			end
			phase.drop_objection(this);
		endtask

	endclass

	class iu_env extends uvm_env;
		`uvm_component_utils(iu_env);

		iu_agent agent;

		function new(string name, uvm_component parent);
			super.new(name, parent);
		endfunction

		function void build_phase(uvm_phase phase);
			super.build_phase(phase);
			`uvm_info("Env", "build_phase", UVM_DEBUG);
			agent = iu_agent::type_id::create("agent", this);
		endfunction

	endclass
	
	// Uniform test name
	class dut_test0 extends uvm_test;
		`uvm_component_utils(dut_test0);

		iu_env env;

		function new(string name, uvm_component parent);
			super.new(name, parent);
		endfunction

		function void build_phase(uvm_phase phase);
			super.build_phase(phase);
			`uvm_info("Test", "build_phase", UVM_DEBUG);
			env = iu_env::type_id::create("env", this);
		endfunction

		task run_phase(uvm_phase phase);
			`uvm_info("Test", "run_phase", UVM_DEBUG);
			phase.raise_objection(this);
			#10;
			`uvm_warning("Test", "Starting")
			phase.drop_objection(this);
		endtask

	endclass

endpackage
