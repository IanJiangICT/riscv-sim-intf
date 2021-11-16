module sc_test;

import "DPI-C" function int sc_init(input string host, input int port);
import "DPI-C" function int sc_run(output longint npc);

initial
begin
	int ret;
	longint npc;
	$display("SC Test Start");
	ret = sc_init("127.0.0.1", 8765);
	ret = sc_run(npc); $display("R %x", npc);
	ret = sc_run(npc); $display("R %x", npc);
	ret = sc_run(npc); $display("R %x", npc);
	ret = sc_run(npc); $display("R %x", npc);
	ret = sc_run(npc); $display("R %x", npc);
	ret = sc_run(npc); $display("R %x", npc);
	ret = sc_run(npc); $display("R %x", npc);
	ret = sc_run(npc); $display("R %x", npc);
	$display("%d", ret);
	$display("SC Test End");

end

endmodule
