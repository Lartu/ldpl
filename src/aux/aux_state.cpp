/* This file contains auxiliary functions that check the current compilation
 * state */

#include "../ldpl.h"

bool is_num_map(const string &token, compiler_state &state)
{
    // -- Returns if the variable is a NUMBER MAP --
    vector<unsigned int> type = variable_type(token, state);
    if (type.size() == 2 && type[0] == 1 && type[1] == 4)
        return true;
    return false;
}

bool is_txt_map(const string &token, compiler_state &state)
{
    // -- Returns if the variable is a TEXT MAP or an access to a multicontainer
    // that results in a TEXT MAP --
    vector<unsigned int> type = variable_type(token, state);
    if (type.size() == 2 && type[0] == 2 && type[1] == 4)
        return true;
    return false;
}

bool is_scalar_map(const string &token, compiler_state &state)
{
    // -- Returns if the variable is a NUMBER MAP or if the variable is a TEXT MAP --
    return is_num_map(token, state) || is_txt_map(token, state);
}

bool is_map(const string &token, compiler_state &state)
{
    // -- Returns true if the variable is a MAP, regardless of object type --
    return is_scalar_map(token, state);
}

bool is_num_var(const string &token, compiler_state &state)
{
    // -- Checks if token is a NUMBER variable (or an access to a container that
    // results in a NUMBER variable) --
    return (variable_type(token, state) == vector<unsigned int>{1});
}

bool is_txt_var(const string &token, compiler_state &state)
{
    // -- Checks if token is a TEXT variable (or an access to a container that
    // results in a TEXT variable) --
    return (variable_type(token, state) == vector<unsigned int>{2});
}

bool is_scalar_variable(const string &token, compiler_state &state)
{
    // -- Returns is an identifier is a valid scalar variable or an access that
    // results in one --
    return is_num_var(token, state) || is_txt_var(token, state);
}

bool is_num(const string &token, compiler_state &state)
{
    // -- Returns is an identifier is a valid scalar variable or number or an
    // access that results in one --
    return is_num_var(token, state) || is_number(token);
}

bool is_txt(const string &token, compiler_state &state)
{
    // -- Returns is an identifier is a valid scalar variable or text or an access
    // that results in one --
    return is_txt_var(token, state) || is_string(token);
}

bool is_scalar_value(const string &token, compiler_state &state)
{
    // -- Returns is an identifier is a valid scalar value or
    // a map access that results in one --
    return is_num(token, state) || is_txt(token, state);
}

bool is_value(const string &token, compiler_state &state)
{
    // -- Returns is an identifier is a valid scalar value, map access or map --
    return is_num(token, state) || is_txt(token, state) || is_map(token, state);
}

bool variable_exists(const string &token, compiler_state &state)
{
    // -- Returns if a variable has been declared or not --
    // (Bear in mind that myList is a variable, myList:0 is not, that's an access
    // for all this function is concerned)
    return variable_type(token, state) != vector<unsigned int>{0};
}

bool is_subprocedure(const string &token, compiler_state &state)
{
    // -- Returns if an identifier maps to a valid, existing sub-procedure --
    for (auto &subprocedure : state.subprocedures)
        if (subprocedure.first == token)
            return true;
    return false;
}

bool in_procedure_section(compiler_state &state)
{
    // -- Returns if the compiler is currently compiling a procedure section or
    // not --
    if (state.section_state == 3)
    {
        // We're inside a SUB-PROCEDURE procedure with no sections
        state.section_state = 2;
        open_subprocedure_code(state);
    }
    return state.section_state == 2;
}

vector<unsigned int> variable_type(const string &token, compiler_state &state)
{
    // -- Returns the LDPL internal representation of the type of a variable --
    //
    // Return the number of the type or {0} if the variable doesn't exist.
    //
    vector<string> tokens;
    string varName = "";
    tokenize(token, tokens, state.where, true, ':');
    // So, for example, foo:0
    // We take the first element as the variable name
    varName = tokens[0];
    // Then we check if the variable exists. If it does, we store its types in a
    // variable.
    vector<unsigned int> types;
    if (state.variables[state.current_subprocedure].count(varName) > 0)
    {
        // Local Variable
        types = state.variables[state.current_subprocedure][varName];
    }
    else if (state.variables[""].count(varName) > 0)
    {
        // Global Variable
        types = state.variables[""][varName];
    }
    else
    {
        // If the variable wasn't found, we return {0}
        return {0};
    }
    // If it was found, though, we want to get its current type (the type
    // currently being accessed, like if foo is NUMBER MAP, then foo:0 is number)
    if (tokens.size() > 1)
    {
        // If the current token is a scalar literal, we can skip it safely.
        if (!(is_number(tokens[1]) || is_string(tokens[1])))
        {
            // If the variable doesn't exist in the current context, we rise an error.
            if (state.variables[state.current_subprocedure].count(tokens[1]) == 0 && state.variables[""].count(tokens[1]) == 0)
            {
                error("The variable " + tokens[1] + " used in " + token + " doesn't exist.");
            }
        }
        types.pop_back();
    }
    // Now we have the types and can return them.
    return types;
}