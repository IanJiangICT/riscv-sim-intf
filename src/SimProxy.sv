class SimProxy;
	typedef enum {SPIKE_LOG, SPIKE_RUNTIME} SimType;
	string sim_name;
	SimType sim_type;

	// For Spike runtime
	string spike_host;
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

	function int InitSpikeRuntime(string host, int port);
		$display("[SP]: Start Spike %s:%d", spike_host, spike_port);
		$display("[SP]: TODO");
		sim_type = SPIKE_RUNTIME;
		spike_host = host;
		spike_port = port;
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

	function int RunNext(output longint pc, int insn);
		if (sim_type == SPIKE_LOG)
			return RunNextSpikeLog(pc, insn);
	endfunction

endclass
