`ifndef IU_TRANS
`define IU_TRANS
class iu_trans extends uvm_sequence_item;
	`uvm_object_utils(iu_trans);

	rand bit miss;
	rand longint pc_pre;
	rand longint pc_curr;
	rand int insn_curr;

	function new(string name = "");
		super.new(name);
	endfunction

	function copy(iu_trans trans);
		this.miss = trans.miss;
		this.pc_pre = trans.pc_pre;
		this.pc_curr = trans.pc_curr;
		this.insn_curr = trans.insn_curr;
	endfunction
endclass
`endif
