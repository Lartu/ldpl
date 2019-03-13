#!/bin/awk -f
BEGIN{
	print "void add_ldpllib(compiler_state & state){";
}
{
	gsub("\"", "\\\"");
	gsub(/\\n/, "\\\\\\n");
	print "    state.add_var_code(\"" $0 "\");";
}
END{
	print "}";
}
