#!/bin/awk -f
BEGIN{
	print "void add_ldpllib(compiler_state & state){";
}
{
	gsub("\"", "\\\"", $line);
	gsub("\\n", "\\\\n", $line);
	print "    state.add_var_code(\"" $line "\");";
}
END{
	print "}";
}