package iu_tb_pkg;
	import uvm_pkg::*;
	`include "iu_sequence.sv"
	`include "iu_driver.sv"
	`include "iu_monitor.sv"

	class iu_agent extends uvm_agent;
		`uvm_component_utils(iu_agent);

		iu_driver driver;
		uvm_sequencer#(iu_trans) sequencer;
		uvm_analysis_port#(iu_trans) agent_ap;
		iu_monitor monitor;

		function new(string name, uvm_component parent);
			super.new(name, parent);
		endfunction

		function void build_phase(uvm_phase phase);
			super.build_phase(phase);
			`uvm_info("Agent", "build_phase", UVM_DEBUG);
			driver = iu_driver::type_id::create("driver", this);
			sequencer = uvm_sequencer#(iu_trans)::type_id::create("sequencer", this);
			agent_ap = new("agent_ap", this);
			monitor = iu_monitor::type_id::create("monitor", this);
		endfunction

		function void connect_phase(uvm_phase phase);
			`uvm_info("Agent", "connect_phase", UVM_DEBUG);
			driver.seq_item_port.connect(sequencer.seq_item_export);
			monitor.mon_ap.connect(agent_ap);
		endfunction

		task run_phase(uvm_phase phase);
			`uvm_info("Agent", "run_phase", UVM_DEBUG);
			phase.raise_objection(this);
			`ifdef HELLO_SEQ
			begin
				iu_seq_hello seq;
				seq = iu_seq_hello::type_id::create("seq_hello");
				seq.start(sequencer);
			end
			`else
			begin
				iu_seq_sim seq;
				seq = iu_seq_sim::type_id::create("seq_sim");
				seq.start(sequencer);
			end
			`endif
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
