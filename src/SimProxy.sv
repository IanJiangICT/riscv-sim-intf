import sc_cfg::*;

import "DPI-C" function int sc_init_sim(input string elf, input int port);
import "DPI-C" function int sc_run_next(output longint npc, output longint pc, output longint insn);
import "DPI-C" function int sc_decode(input int code_len, input byte code_data[DEC_INSN_STREAM_CAP-1:0],
                                      input int insn_max, output byte insn_list[DEC_INSN_LIST_SIZE-1:0]);
import "DPI-C" function int sc_force_pc(input longint new_pc, output longint insn, output longint next_pc);
import "DPI-C" function int sc_save_state();
import "DPI-C" function int sc_recover_state(input longint pc);

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

	function int InitSpikeRuntime(string elf, int port = `SIM_PORT);
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

	function int RunNext(output longint pc, output int insn);
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
		int i;
		int j;
		byte c;

		if (DEC_DISCARD_ISTREAM_REMNANT != 0) begin
			cnt = sc_decode(0, i_stream, insn_max, insn_list);
		end
		cnt = sc_decode(stream_cap, i_stream, insn_max, insn_list);
		offset = 0;
		for (i = 0; i < cnt; i++) begin // Ref to struct insn_info in sc_types.h
			len[i] = insn_list[offset + DEC_INSN_LEN_OFFSET]; // Only the 1st byte is enough at present
			insn[i] = longint'(insn_list[(offset + DEC_INSN_INS_OFFSET) +:DEC_INSN_INS_SIZE]);
			ext[i] = insn_list[offset + DEC_INSN_EXT_OFFSET];
			typ[i] = insn_list[offset + DEC_INSN_TYP_OFFSET];
			disasm[i] = "";
			for (j = 0; j < DEC_INSN_DIS_SIZE; j++) begin
				c = insn_list[offset + DEC_INSN_DIS_OFFSET + j];
				disasm[i] = {disasm[i], c};
				if (c == 0) break;
			end
			offset += DEC_INSN_INFO_SIZE;
		end

		return cnt;
	endfunction

	function int RunUntil(input longint pc);
		longint npc;
		longint cpc;
		longint ci;
		int cnt;
		int ret;

		cnt = 0;
		do begin
			cnt = cnt + 1;
			ret = sc_run_next(npc, cpc, ci);
			if (ret < 0) begin
				$display("Error: Failed sc_run_next %d", cnt);
				return -1;
			end
		end while (cpc != pc);

		ret = sc_save_state();
		if (ret < 0) begin
			$display("Error: Failed sc_save_state at %x", pc);
			return -1;
		end

		return 1;
	endfunction

	function int RunAt(input longint pc, output longint insn, output longint npc);
		int ret;

		ret = sc_force_pc(pc, insn, npc);
		if (ret < 0) begin
			$display("Error: Failed sc_force_pc at %x", pc);
			return -1;
		end
		ret = sc_save_state();
		if (ret < 0) begin
			$display("Error: Failed sc_save_state at %x", pc);
			return -1;
		end

		return 1;
	endfunction

	function int RevertTo(input longint pc);
		int ret;

		ret = sc_recover_state(pc);
		if (ret < 0) begin
			$display("Error: Failed sc_recover_state at %x", pc);
			return -1;
		end
		return 1;

	endfunction
endclass
