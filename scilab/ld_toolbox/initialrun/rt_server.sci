//
// ORTD_SOCKET_open(1,"localhost",10000);
// ORTD_SOCKET_write(1,"set_param oscinput #  0");
//
//


function ORTD_SOCKET_open(id,adress,port)
	SOCKET_close(id);
	TCL_EvalStr(["set tclsocket"+string(id)+" [socket "+adress+" "+string(port)+"]";"fconfigure $tclsocket"+string(id)+" -blocking 0 -translation auto"]);
endfunction


function ORTD_SOCKET_close(id)
	TCL_EvalStr("catch {close $tclsocket"+string(id)+"}");
endfunction

function mat=ORTD_SOCKET_read(id)
	mat=[];
	cmd=["set tclsocketlong 0;";
	"while {$tclsocketlong >= 0} {";
	"set tclsocketlong [gets $tclsocket"+string(id)+" tclsocketligne];";
	"if {$tclsocketlong >= 0} {";
//	"flush $tclsocket"+string(id)+";";
// bug en cas de ""
	"ScilabEval ""mat=\[\""[string map {\"" \""\""} $tclsocketligne]\"";mat\];""";
//	"ScilabEval ""mat=\[TCL_GetVar(''tclsocketligne'');mat\];""";
	"}";
	"}"];
	TCL_EvalStr(strcat(cmd," "));
	
endfunction


function ORTD_SOCKET_write(id,commande)
	TCL_EvalStr([	"puts $tclsocket"+string(id)+" """+commande+ascii(10)+"""";
					"flush $tclsocket"+string(id)]);

endfunction

// Set a parameter of length 1 via the remote interface
function ortd_remset_param_simple(ho, port, parname, val)
  unix("echo " + char(34) + "set_param " + parname + " # " + string(val) + " " + char(34) + " | netcat " + ho + " " + string(port) );
endfunction


function dls = ortd_rem_ls(ho, port)
  dls = unix_g("echo ls" + "  | netcat " + ho + " " + string(port) );
endfunction

function vec = ortd_rem_readstream(ho, port, name, len)
  msg = unix_g("echo stream_fetch " + name + " " + string(len) + "  | netcat " + ho + " " + string(port) );

  // parse the string matrix
  [n,m] = size(msg); 
  n=n-1; // Do not use the last element because its the end marker
  vec = zeros(n, 1);
  for i = 1:(n)
      vec(i) = msscanf( msg(i), " %f " );
  end
endfunction
