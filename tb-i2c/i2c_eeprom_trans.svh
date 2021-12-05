`ifndef I2C_EEPROM_TRANS
`define I2C_EEPROM_TRANS

`include "I2cTrans.svh"

class i2c_eeprom_trans extends uvm_sequence_item;
	`uvm_object_utils(i2c_eeprom_trans);

	I2cTrans trans;

	function new(string name = "");
		super.new(name);
	endfunction

endclass
`endif
