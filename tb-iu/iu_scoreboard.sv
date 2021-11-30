class iu_scoreboard extends uvm_scoreboard;
	`uvm_component_utils(iu_scoreboard)

	uvm_analysis_imp #(iu_trans, iu_scoreboard) ap_imp;
	longint trans_cnt;
	longint miss_cnt;
	iu_trans miss_queue [$];

	function new(string name, uvm_component parent);
		super.new(name, parent);
		trans_cnt = 0;
		miss_cnt = 0;
	endfunction

	function void build_phase(uvm_phase phase);
		super.build_phase(phase);
		`uvm_info("Scoreboard", "build_phase", UVM_DEBUG);
		ap_imp = new("ap_imp", this);
	endfunction

	function void write(iu_trans trans);
		trans_cnt++;
		if (trans.miss) begin
			miss_cnt++;
			miss_queue = {miss_queue, trans};
		end
	endfunction

	function void report_phase(uvm_phase phase);
		`uvm_info("Scoreboard", $sformatf("total %d, miss %d", trans_cnt, miss_cnt), UVM_LOW);
	endfunction
endclass
