`ifndef I2C_EEPROM_SVH
`define I2C_EEPROM_SVH

`include "I2cTrans.svh"

`define MEM_CAP_MAX 128

class I2cEeprom;
	byte i2c_slv_addr;
	byte memory[];
	byte capacity;
	byte mem_addr;

	function new(byte addr, byte cap);
		if (cap <= 0 || cap > MEM_CAP_MAX)
			capacity = MEM_CAP_MAX; 
		else
			capacity = cap;
		i2c_slv_addr = addr & 8'h7F;
		memory = new[capacity];
		mem_addr = 0;
	endfunction

	function int set_addr(input byte addr)
		if (addr >= capacity) return -1;
		mem_addr = addr;
	endfunction

	function void set_byte(input byte data)
		mem[mem_addr] = data;
	endfunction

	function void get_byte(output byte data)
		data = mem[mem_addr];
	endfunction

	// 1byte command + 1byte address + 1byte data
	function void ref_behavior(I2cTrans trans);
		logic rw;
		byte slv_addr;
		byte rw_addr;

		rw = trans.cmd[0];
		slv_addr = trans.cmd >> 1;
		if(slv_addr != i2c_slv_addr) return;
		if (rw_addr >= capacity) return;
		set_addr(rw_addr);
		if (rw) set_byte(trans.data);
		else get_byte(trans.data);
	endfunction
endclass
`endif
