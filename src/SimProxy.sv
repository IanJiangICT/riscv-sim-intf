import sc_cfg::*;

import "DPI-C" function int sc_init_sim(input string elf, input int port);
import "DPI-C" function int sc_run_next(output longint npc, output longint pc, output longint insn);
import "DPI-C" function int sc_decode(input int code_len, input byte code_data[DEC_INSN_STREAM_CAP-1:0],
                                      input int insn_max, output byte insn_list[DEC_INSN_LIST_SIZE-1:0]);

`define SIM_PORT 12300

class SimProxy;
	typedef enum {SPIKE_LOG, SPIKE_RUNTIME} SimType;
	string sim_name;
	SimType sim_type;

	// For Spike runtime
	string elf_filename;
	int spike_port;

	// For Spike log
	string log_filename;
	int log_fd;
	longint log_line_num;

	function new();
	endfunction

	function int InitSpikeLog(string logfile);
		sim_type = SPIKE_LOG;
		log_fd = $fopen(logfile, "r");
		if (log_fd == 0) begin
			$display("[SP] Error: Failed to open logfile %s", logfile);
			return -1;
		end
		$display("[SP]: Open logfile %s OK", logfile);
		log_filename = logfile;
		log_line_num = 0;
		return 1;
	endfunction

	function int InitSpikeRuntime(string elf, int port = SIM_PORT);
		int ret;

		$display("[SP]: Start Spike %d %s", port, elf);
		sim_type = SPIKE_RUNTIME;
		elf_filename = elf;
		spike_port = port;

		ret = sc_init_sim(elf, port);
		if (ret < 0) begin
			$display("[SP] Error: Failed to start Spike %d %s", port, elf);
			return -1;
		end
		return 1;
	endfunction

	local function int RunNextSpikeLog(output longint pc, output int insn);
		string log_line;
		longint v_pc;
		int v_insn;
		int ret;

		if (log_fd == 0) return -1;	// TODO assert()
		while(1) begin
			if ($feof(log_fd)) begin
				$display("[SP]: End of logfile");
				return 0;
			end
			$fgets(log_line, log_fd);
			ret = $sscanf(log_line, "0x%h 0x%h", v_pc, v_insn);
			if (ret == 2) begin
				pc = v_pc;
				insn = v_insn;
				return 1;
			end else
				continue;
		end
		return 1;	
	endfunction

	local function int RunNextSpikeRuntime(output longint pc, output int insn);
		longint npc;
		longint cpc;
		longint ci;
		int ret;

		ret = sc_run_next(npc, cpc, ci);
		if (ret < 0) begin
			$display("Error: Failed sc_run_next");
			return -1;
		end
		pc = cpc;
		insn = ci;
		return 1;
	endfunction

	function int RunNext(output longint pc, int insn);
		if (sim_type == SPIKE_LOG)
			return RunNextSpikeLog(pc, insn);
		else if (sim_type == SPIKE_RUNTIME)
			return RunNextSpikeRuntime(pc, insn);
		else
			return -1;
	endfunction

	function int DecodeIStream(input byte i_stream[DEC_INSN_STREAM_CAP-1:0],
                        output byte len[DEC_INSN_LIST_CAP-1:0],
                        output longint insn[DEC_INSN_LIST_CAP-1:0],
                        output byte ext[DEC_INSN_LIST_CAP-1:0],
                        output byte typ[DEC_INSN_LIST_CAP-1:0],
                        output string disasm[DEC_INSN_LIST_CAP-1:0]);
		int stream_cap = DEC_INSN_STREAM_CAP;
		byte insn_list[DEC_INSN_LIST_SIZE-1:0];
		int insn_max = DEC_INSN_LIST_CAP;
		int offset;
		int cnt;

		cnt = sc_decode(stream_cap, i_stream, insn_max, insn_list);
		offset = 0;
		for (int i = 0; i < cnt; i++) begin
			len[i] = insn_list[offset+0];
			insn[i] = longint'(insn_list[(offset+15):(offset+8)]);
			ext[i] = insn_list[offset+16];
			typ[i] = insn_list[offset+17];
			disasm[i] = string'({<<byte{insn_list[(offset+81):(offset+18)]}});
			offset += DEC_INSN_INFO_SIZE;
		end

		return cnt;
	endfunction

endclass
