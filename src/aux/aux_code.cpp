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
    if (is_parallel(name, state))
    {
        code = "void * " + fix_identifier(name, false) + "(void * thread_id){";
        state.add_code(code, state.where);
        string id_var_name = "PARALLEL_ID";
        id_var_name = fix_identifier(id_var_name, true, state);
        code = "ldpl_number " + id_var_name + "= (long long int) thread_id;";
        state.add_code(code, state.where);
    }
    else
    {
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
    }
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

void add_call_parallel_code(string &subprocedure, string &var_name, vector<string> &parameters, compiler_state &state)
{
    string code;
    code = "pthread_create(&ldpl_thread_num, NULL, ";
    code += fix_identifier(subprocedure, false) + ", ";
    code += "(void *) ++ldpl_pthread_count);\n";
    state.add_code(code, state.where);
    code = get_c_variable(state, var_name) + " = ldpl_pthread_count;\n";
    code += "ldpl_thread_numbers[ldpl_pthread_count] = ldpl_thread_num;\n";
    state.add_code(code, state.where);
}

void add_stop_parallel_code(string &var_name, compiler_state &state)
{
    string code = "if(ldpl_thread_numbers.find(" + get_c_variable(state, var_name) + ") != ldpl_thread_numbers.end()){\n";
    code += "pthread_cancel(ldpl_thread_numbers[" + get_c_variable(state, var_name) + "]);\n";
    code += "ldpl_thread_numbers.erase(" + get_c_variable(state, var_name) + ");\n";
    code += "}";
    state.add_code(code, state.where);
}

void add_wait_parallel_code(string &var_name, compiler_state &state)
{
    string code = "if(ldpl_thread_numbers.find(" + get_c_variable(state, var_name) + ") != ldpl_thread_numbers.end()){\n";
    code += "pthread_join(ldpl_thread_numbers[" + get_c_variable(state, var_name) + "], NULL);\n";
    code += "ldpl_thread_numbers.erase(" + get_c_variable(state, var_name) + ");\n";
    code += "}";
    state.add_code(code, state.where);
}