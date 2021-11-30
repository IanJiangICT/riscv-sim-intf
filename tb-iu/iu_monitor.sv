class iu_monitor extends uvm_monitor;
	`uvm_component_utils(iu_monitor)

	virtual dut_if dut_vif;
	uvm_analysis_port#(iu_trans) mon_ap;

	function new(string name, uvm_component parent);
		super.new(name, parent);
	endfunction

	function void build_phase(uvm_phase phase);
		super.build_phase(phase);
		`uvm_info("Monitor", "build_phase", UVM_DEBUG);
		if (!uvm_config_db#(virtual dut_if)::get(this, "", "dut_vif", dut_vif)) begin
			`uvm_error("Monitor", "Failed to get dut_vif");
		end else begin
			`uvm_info("Monitor", "Got dut_vif", UVM_DEBUG);
		end
		mon_ap = new("mon_ap", this);
	endfunction

	task run_phase(uvm_phase phase);
		iu_trans tx;
		longint pc_pre_copy = 0;
		int state = 0;
		`uvm_info("Monitor", "run_phase", UVM_DEBUG);
		tx = iu_trans::type_id::create("tx");
		forever begin
			@(posedge dut_vif.clk);
			case (state)
				0:
					if(dut_vif.pc_pre_oe) begin
						pc_pre_copy = dut_vif.pc_pre;
						state = 1;
					end else
						state = 0;
				1:
					state = 2;
				2:
					begin
						tx.miss = dut_vif.miss;
						tx.pc_curr = dut_vif.pc_curr;
						tx.insn_curr = dut_vif.insn_curr;
						tx.pc_pre = pc_pre_copy;
						`uvm_info("Monitor", $sformatf("Write to ap: m %d c %x p %x", tx.miss, tx.pc_curr, tx.pc_pre), UVM_DEBUG);
						mon_ap.write(tx);
						state = 0;
					end
			endcase
		end
	endtask
endclass
