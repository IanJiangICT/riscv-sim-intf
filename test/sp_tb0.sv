import sc_cfg::*;

module sim_proxy_tb;

	SimProxy sp;

	function dec_stream(input byte s[DEC_INSN_STREAM_CAP-1:0]);
		byte s_revert[DEC_INSN_STREAM_CAP-1:0];
		byte len[DEC_INSN_LIST_CAP-1:0];
		longint insn[DEC_INSN_LIST_CAP-1:0];
		byte ext[DEC_INSN_LIST_CAP-1:0];
		byte typ[DEC_INSN_LIST_CAP-1:0];
		string disasm[DEC_INSN_LIST_CAP-1:0];

		string out_str = "";
		int i;
		int ret;

		for (i = 0; i < DEC_INSN_STREAM_CAP; i++) begin
			s_revert[i] = s[DEC_INSN_STREAM_CAP-1-i];
			out_str = $sformatf("%s %x", out_str, s_revert[i]);
		end
		$display("Decode %d bytes: %s", $size(s_revert), out_str);
		ret = sp.DecodeIStream(s_revert, len, insn, ext, typ, disasm);
		$display("Got %d instructions:", ret);
		$display("size-in-byte  machine-code  ext type disasm", ret);
		for (i = 0; i < ret; i++) begin
			$display("%d %x %c %c %s", len[i], insn[i], ext[i], typ[i], disasm[i]);
		end

	endfunction

	initial
	begin
		byte t_stream_0[DEC_INSN_STREAM_CAP-1:0] = {
'h97, 'h02, 'h00, 'h00, 'h93, 'h82, 'h42, 'h03, 'h73, 'h90, 'h52, 'h30, 'hf3, 'h22, 'h40, 'hf1,
'h93, 'h92, 'ha2, 'h00, 'h17, 'h21, 'h00, 'h00, 'h13, 'h01, 'hc1, 'h49, 'h33, 'h01, 'h51, 'h00,
'h73, 'h25
		};
		byte t_stream_1[DEC_INSN_STREAM_CAP-1:0] = {
'h97, 'h02, 'h00, 'h00, 'h93, 'h82, 'hc2, 'h02, 'h73, 'h90, 'h52, 'h30, 'hf3, 'h22, 'h40, 'hf1,
'haa, 'h02, 'h17, 'h21, 'h00, 'h00, 'h13, 'h01, 'he1, 'h49, 'h16, 'h91, 'h73, 'h25, 'h40, 'hf1,
'h19, 'he1
		};
		int ret;
		sp = new();
		ret = sp.InitSpikeRuntime("./hello");
		dec_stream(t_stream_0);
		dec_stream(t_stream_1);
		$finish();
	end

endmodule
