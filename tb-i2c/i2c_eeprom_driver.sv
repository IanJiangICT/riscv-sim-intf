class i2c_eeprom_driver extends uvm_driver#(i2c_eeprom_trans);
	`uvm_component_utils(i2c_eeprom_driver);

	virtual dut_if dut_vif;

	function new(string name, uvm_component parent);
		super.new(name, parent);
	endfunction

	function void build_phase(uvm_phase phase);
		super.build_phase(phase);
		`uvm_info("Driver", "build_phase", UVM_DEBUG);
		if (!uvm_config_db#(virtual dut_if)::get(this, "", "dut_vif", dut_vif)) begin
			`uvm_error("Driver", "Failed to get dut_vif");
		end else begin
			`uvm_info("Driver", "Got dut_vif", UVM_DEBUG);
		end
	endfunction

	task run_phase(uvm_phase phase);
		`uvm_info("Driver", "run_phase", UVM_DEBUG);
		forever begin
			seq_item_port.get_next_item(req);
			@(posedge dut_vif.clk);
			seq_item_port.item_done(req);
		end
	endtask
endclass
