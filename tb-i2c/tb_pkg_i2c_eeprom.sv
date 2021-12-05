package tb_pkg_i2c_eeprom;
	import uvm_pkg::*;
	`include "i2c_eeprom_sequence.sv"
	`include "i2c_eeprom_driver.sv"
	//`include "i2c_eeprom_monitor.sv"
	`include "i2c_eeprom_scoreboard.sv"

	class i2c_eeprom_agent extends uvm_agent;
		`uvm_component_utils(i2c_eeprom_agent);

		i2c_eeprom_driver driver;
		uvm_sequencer#(i2c_eeprom_trans) sequencer;
		uvm_analysis_port#(i2c_eeprom_trans) agent_ap;
		//i2c_eeprom_monitor monitor;

		function new(string name, uvm_component parent);
			super.new(name, parent);
		endfunction

		function void build_phase(uvm_phase phase);
			super.build_phase(phase);
			`uvm_info("Agent", "build_phase", UVM_DEBUG);
			driver = i2c_eeprom_driver::type_id::create("driver", this);
			sequencer = uvm_sequencer#(i2c_eeprom_trans)::type_id::create("sequencer", this);
			agent_ap = new("agent_ap", this);
			//monitor = i2c_eeprom_monitor::type_id::create("monitor", this);
		endfunction

		function void connect_phase(uvm_phase phase);
			`uvm_info("Agent", "connect_phase", UVM_DEBUG);
			driver.seq_item_port.connect(sequencer.seq_item_export);
			//monitor.mon_ap.connect(agent_ap);
		endfunction

		task run_phase(uvm_phase phase);
			`uvm_info("Agent", "run_phase", UVM_DEBUG);
			phase.raise_objection(this);
			`ifdef HELLO_SEQ
			begin
				i2c_eeprom_seq_hello seq;
				seq = i2c_eeprom_seq_hello::type_id::create("seq_hello");
				seq.start(sequencer);
			end
			`else
			begin
				i2c_eeprom_seq_sim seq;
				seq = i2c_eeprom_seq_sim::type_id::create("seq_sim");
				seq.start(sequencer);
			end
			`endif
			phase.drop_objection(this);
		endtask

	endclass

	class i2c_eeprom_env extends uvm_env;
		`uvm_component_utils(i2c_eeprom_env);

		i2c_eeprom_agent agent;
		i2c_eeprom_scoreboard scoreboard;

		function new(string name, uvm_component parent);
			super.new(name, parent);
		endfunction

		function void build_phase(uvm_phase phase);
			super.build_phase(phase);
			`uvm_info("Env", "build_phase", UVM_DEBUG);
			agent = i2c_eeprom_agent::type_id::create("agent", this);
			scoreboard = i2c_eeprom_scoreboard::type_id::create("scoreboard", this);
		endfunction

		function void connect_phase(uvm_phase phase);
			`uvm_info("Env", "connect_phase", UVM_DEBUG);
			//agent.monitor.mon_ap.connect(scoreboard.ap_imp);
		endfunction

	endclass
	
	// Uniform test name
	class dut_test0 extends uvm_test;
		`uvm_component_utils(dut_test0);

		i2c_eeprom_env env;

		function new(string name, uvm_component parent);
			super.new(name, parent);
		endfunction

		function void build_phase(uvm_phase phase);
			super.build_phase(phase);
			`uvm_info("Test", "build_phase", UVM_DEBUG);
			env = i2c_eeprom_env::type_id::create("env", this);
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
