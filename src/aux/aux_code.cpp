/* This file contains auxiliary functions that add C++ code to the generated C++ file */

// +---------------------------------------------+
// | TODO: comment and format this file properly |
// +---------------------------------------------+

// This is called when we know all parameters of a subprocedure
void open_subprocedure_code(compiler_state &state)
{
    string name = state.current_subprocedure;
    vector<string> &parameters = state.subprocedures[name];
    vector<vector<unsigned int>> types;
    string code;
    code = "void " + fix_identifier(name, false) + "(";
    for (size_t i = 0; i < parameters.size(); ++i)
    {
        string identifier = fix_identifier(parameters[i], true, state);
        string type = state.get_c_type(state.variables[name][parameters[i]]);
        code += type + " & " + identifier;
        if (i < parameters.size() - 1)
            code += ", ";
        types.push_back(state.variables[name][parameters[i]]);
    }
    if (!state.correct_subprocedure_types(name, types))
        badcode("SUB-PROCEDURE declaration parameter types doesn't match previous CALL", state.where);
    code += "){";
    state.add_code(code, state.where);
    state.remove_expected_subprocedure(name);
}

void add_call_code(string &subprocedure, vector<string> &parameters, compiler_state &state)
{
    string code = fix_identifier(subprocedure, false) + "(";
    for (size_t i = 0; i < parameters.size(); ++i)
    {
        if (is_number(parameters[i]) || is_string(parameters[i]))
        {
            // C++ doen't allow passing literals in  reference parameters, we create vars for them
            string literal_paramater_var = state.new_literal_parameter_var();
            state.add_code((is_number(parameters[i]) ? "ldpl_number " : "chText ") + literal_paramater_var + " = " + parameters[i] + ";", state.where);
            code += literal_paramater_var;
        }
        else
        {
            code += get_c_variable(state, parameters[i]);
        }
        if (i < parameters.size() - 1)
            code += ", ";
    }
    code += ");";
    state.add_code(code, state.where);
}