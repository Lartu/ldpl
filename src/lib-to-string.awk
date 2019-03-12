#!/bin/awk -f
BEGIN{
	print "void add_ldpllib(compiler_state & state){";
}
{
	gsub("\"", "\\\"", $0);
	gsub(/\\n/, "\\\\n", $0);
	print "    state.add_var_code(\"" $0 "\");";
}
END{
	print "}";
}