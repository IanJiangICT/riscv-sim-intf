`include "i2c_eeprom_trans.svh"

class i2c_eeprom_seq extends uvm_sequence#(iu_trans);
	`uvm_object_utils(i2c_eeprom_seq);

	function new(string name = "");
		super.new(name);
	endfunction

	task body;
		int i;
		`uvm_info("Seq", "body", UVM_DEBUG);
		for (i = 0; i< 8; i++) begin
			req = iu_trans::type_id::create("req");
			start_item(req);
			finish_item(req);
		end
	endtask
endclass
