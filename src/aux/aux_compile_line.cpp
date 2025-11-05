/* This file contains the behemoth compile_line function that pattern-matches
 * lines and compiles them */

// +---------------------------------------------+
// | TODO: comment and format this file properly |
// +---------------------------------------------+

#include "../ldpl.h"

// Compiles line per line
void compile_line(vector<string> &tokens, compiler_state &state)
{
    // Sections
    if (line_like("-- DATA --", tokens, state))
    {
        if (state.section_state == 1)
            badcode("Duplicate DATA section declaration", state.where);
        if (state.section_state >= 2)
            badcode("DATA section declaration within PROCEDURE section", state.where);
        state.section_state = 1;
        return;
    }
    if (line_like("-- PROCEDURE --", tokens, state))
    {
        if (state.section_state == 2)
            badcode("Duplicate PROCEDURE section declaration", state.where);
        if (state.current_subprocedure != "" && state.section_state >= 3)
            open_subprocedure_code(state);
        state.section_state = 2;
        return;
    }
    if (line_like("-- ARGS --", tokens, state))
    {
        if (state.current_subprocedure == "")
            badcode("ARGS section outside subprocedure", state.where);
        if (state.section_state == 4)
            badcode("Duplicate ARGS section declaration", state.where);
        if (state.section_state == 1)
            badcode("ARGS section declaration within LOCAL DATA section",
                    state.where);
        if (state.section_state == 2)
            badcode("ARGS section declaration within PROCEDURE section",
                    state.where);
        state.section_state = 4;
        return;
    }
    if (line_like("-- LOCAL --", tokens, state))
    {
        if (state.current_subprocedure == "")
            badcode("LOCAL data section outside subprocedure", state.where);
        if (state.section_state == 1)
            badcode("Duplicate LOCAL data section declaration", state.where);
        if (state.section_state == 2)
            badcode("LOCAL data section declaration within PROCEDURE section",
                    state.where);
        state.section_state = 1;
        open_subprocedure_code(state);
        return;
    }
    // Variable Declaration
    if (line_like("VAR $anything $anything", tokens, state)) // If it's a variable declaration
    {
        string extern_keyword = "";       // C++ extern keyword to prepend to the type
                                          // (empty if not EXTERNAL)
        vector<unsigned int> type_number; // All data types in LDPL have a list of
                                          // numbers that represent its type
        string assign_default;            // Default Value to asign to the variable
        bool valid_type = true;           // Used to check if it's a valid tipe
        if (!line_like("$name", {tokens[1]}, state))
        {
            badcode("Invalid variable name '" + tokens[1] + "'", state.where);
        }
        if (tokens[2] == "NUM")
        {                            // If it's a number...
            type_number = {1};       // Number, Map
            assign_default = " = 0"; // if its not an external variable, set a
                                     // default value for it.
        }
        else if (tokens[2] == "STR")
        {                      // If we are dealing with a text variable...
            type_number = {2}; // String
            assign_default = " = \"\"";
        }
        else if (tokens[2] == "NUMMAP")
        {                         // If we are dealing with a text variable...
            type_number = {1, 4}; // Number, Map
            assign_default = " = \"\"";
        }
        else if (tokens[2] == "STRMAP")
        {                         // If we are dealing with a text variable...
            type_number = {2, 4}; // String, Map
            assign_default = " = \"\"";
        }
        else
        {                       // If the container is not a VECTOR nor a MAP
            valid_type = false; // then it's not a valid data type.
        }
        if (valid_type)
        {
            if (state.section_state != 1 && state.section_state != 4)
            {
                badcode("Variable declaration outside DATA, PARAMETERS or LOCAL DATA section", state.where);
            }
            if (state.variables[state.current_subprocedure].count(tokens[1]) > 0)
            {
                badcode("Duplicate declaration for variable '" + tokens[1] + "'", state.where);
            }
            state.variables[state.current_subprocedure][tokens[1]] = type_number;
            if (state.section_state == 1)
            {
                // DATA or LOCAL DATA
                string identifier = fix_identifier(tokens[1], true, state);
                string type = state.get_c_type(type_number);
                string code = extern_keyword + type + " " + identifier + assign_default + ";";
                if (state.current_subprocedure == "") // DATA
                {
                    state.add_var_code(code);
                }
                else
                {
                    state.add_code(code, state.where); // LOCAL DATA
                }
            }
            else // PARAMETERS
            {
                state.subprocedures[state.current_subprocedure].emplace_back(tokens[1]);
            }
            return;
        }
        else
        {
            badcode("Unknown type '" + tokens[2] + "'", state.where);
        }
    }

    // SUB-PROCEDURE Declaration
    if (line_like("DEF $anything", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("SUB declaration outside PROCEDURE section",
                    state.where);
        if (is_subprocedure(tokens[1], state))
            badcode("Duplicate declaration for DEF \"" + tokens[1] + "\"",
                    state.where);
        if (state.closing_subprocedure())
            badcode("DEF declaration inside DEF", state.where);
        else if (state.closing_if())
            badcode("DEF declaration inside IF", state.where);
        else if (state.closing_loop())
            badcode("DEF declaration inside WHILE or FOR", state.where);
        state.section_state = 3;
        state.open_subprocedure(tokens[1]);
        state.subprocedures.emplace(tokens[1], vector<string>());
        return;
    }
    if (line_like("END DEF", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("END DEF outside PROCEDURE section", state.where);
        if (!state.closing_subprocedure())
            badcode("END DEF without DEF", state.where);
        // C++ Code
        state.add_code("return;\n}", state.where);
        // Cierro la subrutina
        state.close_subprocedure();
        return;
    }
    if (line_like("IN $anything PUT $scalar", tokens, state))
    {
        if (!in_procedure_section(state))
        {
            badcode("IN/PUT statement outside PROCEDURE section", state.where);
        }
        if (!line_like("$var", {tokens[1]}, state))
        {
            if (line_like("$anyVar", {tokens[1]}, state))
            {
                badcode("Missing index when assigning to map '" + tokens[1] + "'", state.where);
            }
            else
            {
                badcode("Unknown variable '" + tokens[1] + "'", state.where);
            }
        }
        // C++ Code
        string lhand;
        if (is_num_var(tokens[1], state))
        {
            lhand = get_c_number(state, tokens[3]);
        }
        else
        {
            lhand = get_c_string(state, tokens[3]);
        }
        state.add_code(get_c_variable(state, tokens[1]) + " = " + lhand + ";", state.where);
        return;
    }
    // Error 1 for IN - PUT
    if (line_like("IN $anything PUT $anyVar", tokens, state))
    {
        badcode("Missing index when reading from map '" + tokens[3] + "'", state.where);
    }
    // Error 2 for IN - PUT
    if (line_like("IN $anything PUT $anything", tokens, state))
    {
        badcode("Unknown variable or invalid value '" + tokens[3] + "'", state.where);
    }
    if (line_like("IF $condition THEN", tokens, state))
    {
        string condition = get_c_condition(
            state, vector<string>(tokens.begin() + 1, tokens.end() - 1));
        if (condition != "[ERROR]")
        {
            if (!in_procedure_section(state))
                badcode("IF outside PROCEDURE section", state.where);
            // C++ Code
            state.open_if();
            state.add_code("if (" + condition + "){", state.where);
            return;
        }
    }
    if (line_like("ELSE IF $condition THEN", tokens, state))
    {
        string condition = get_c_condition(
            state, vector<string>(tokens.begin() + 2, tokens.end() - 1));
        if (condition != "[ERROR]")
        {
            if (!in_procedure_section(state))
                badcode("ELSE IF outside PROCEDURE section", state.where);
            if (!state.closing_if())
                badcode("ELSE IF without IF", state.where);
            // C++ Code
            state.add_code("} else if (" + condition + "){", state.where);
            return;
        }
    }
    if (line_like("ELSE", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("ELSE outside PROCEDURE section", state.where);
        if (!state.closing_if())
            badcode("ELSE without IF", state.where);
        // C++ Code
        state.open_else();
        state.add_code("}else{", state.where);
        return;
    }
    if (line_like("END IF", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("END IF outside PROCEDURE section", state.where);
        if (!state.closing_if() && !state.closing_else())
            badcode("END IF without IF", state.where);
        // C++ Code
        state.close_if();
        state.add_code("}", state.where);
        return;
    }
    if (line_like("WHILE $condition DO", tokens, state))
    {
        string condition = get_c_condition(
            state, vector<string>(tokens.begin() + 1, tokens.end() - 1));
        if (condition != "[ERROR]")
        {
            if (!in_procedure_section(state))
                badcode("WHILE outside PROCEDURE section", state.where);
            // C++ Code
            state.open_loop();
            state.add_code("while (" + condition + "){", state.where);
            return;
        }
    }
    if (line_like("FOREVER DO", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("FOREVER DO outside PROCEDURE section", state.where);
        // C++ Code
        state.open_loop();
        state.add_code("while (true){", state.where);
        return;
    }
    if (line_like("REPEAT", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("REPEAT outside PROCEDURE section", state.where);
        if (!state.closing_loop())
            badcode("REPEAT without WHILE or FOR", state.where);
        // C++ Code
        state.close_loop();
        state.add_code("}", state.where);
        return;
    }
    if (line_like("BREAK", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("BREAK outside PROCEDURE section", state.where);
        if (state.open_loops == 0)
            badcode("BREAK without WHILE or FOR", state.where);
        // C++ Code
        state.add_code("break;", state.where);
        return;
    }
    if (line_like("CONTINUE", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("CONTINUE outside PROCEDURE section", state.where);
        if (state.open_loops == 0)
            badcode("CONTINUE without WHILE or FOR", state.where);
        // C++ Code
        state.add_code("continue;", state.where);
        return;
    }
    if (line_like("CALL $name", tokens, state) || line_like("CALL $name WITH $arguments", tokens, state))
    {
        size_t i = 1;
        string subprocedure = tokens[i];
        // Valid options: No WITH or WITH with at least one paramter
        if (i == tokens.size() - 1 || (i < tokens.size() - 2 && tokens[i + 1] == "WITH"))
        {
            if (!in_procedure_section(state))
            {
                badcode("CALL outside PROCEDURE section", state.where);
            }
            vector<string> parameters(i != tokens.size() - 1 ? tokens.begin() + i + 2 : tokens.end(), tokens.end());
            vector<string> argument_tokens;
            string arguments;
            for (size_t t = 0; t < parameters.size(); ++t)
            {
                arguments += parameters[t] + " ";
            }
            trim(arguments);
            tokenize(arguments, argument_tokens, state.where, true, ',');
            vector<vector<unsigned int>> types;
            for (size_t parameter_index = 0; parameter_index < argument_tokens.size(); ++parameter_index)
            {
                string &parameter = argument_tokens[parameter_index];
                trim(parameter);
                if (is_number(parameter))
                {
                    types.push_back({1});
                }
                else if (is_string(parameter))
                {
                    types.push_back({2});
                }
                else if (variable_exists(parameter, state))
                {
                    types.push_back(variable_type(parameter, state));
                }
                else
                {
                    badcode("CALL with invalid parameter \"" + parameter + "\"", state.where);
                }
            }
            bool correct_types = state.correct_subprocedure_types(subprocedure, types);
            if (!is_subprocedure(subprocedure, state))
            {
                if (!correct_types)
                {
                    badcode("CALL parameter types don't match previous CALL", state.where);
                }
                state.add_expected_subprocedure(subprocedure, fix_identifier(subprocedure, false), types);
            }
            else
            {
                if (!correct_types)
                {
                    badcode("CALL parameter types don't match subroutine declaration", state.where);
                }
            }
            add_call_code(subprocedure, parameters, state);
            return;
        }
    }
    if (line_like("RETURN", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("RETURN outside PROCEDURE section", state.where);
        if (state.current_subprocedure == "")
            badcode("RETURN found outside subprocedure", state.where);
        // C++ Code
        state.add_code("return;", state.where);
        return;
    }
    if (line_like("EXIT", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("EXIT outside PROCEDURE section", state.where);
        // C++ Code
        state.add_code("exit(0);", state.where);
        return;
    }
    // Arithmetic Statements
    if (line_like("IN $num-var DO $num-scalar % $num-scalar", tokens,
                  state)) // TODO move this into the standard library
    {
        if (!in_procedure_section(state))
            badcode("IN/MODULO statement outside PROCEDURE section", state.where);
        // C++ Code
        state.add_code(get_c_variable(state, tokens[1]) + " = modulo(" +
                           get_c_expression(state, tokens[3]) + ", " +
                           get_c_expression(state, tokens[5]) + ");",
                       state.where);
        return;
    }
    if (line_like("IN $num-var DO $num-scalar + $num-scalar", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("IN/ADD statement outside PROCEDURE section", state.where);
        // C Code
        state.add_code(get_c_variable(state, tokens[1]) + " = " +
                           get_c_expression(state, tokens[3]) + " + " +
                           get_c_expression(state, tokens[5]) + ";",
                       state.where);
        return;
    }
    if (line_like("IN $num-var DO $num-scalar - $num-scalar", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("IN/SUBTRACT statement outside PROCEDURE section", state.where);
        // C Code
        state.add_code(get_c_variable(state, tokens[1]) + " = " +
                           get_c_expression(state, tokens[5]) + " - " +
                           get_c_expression(state, tokens[3]) + ";",
                       state.where);
        return;
    }
    if (line_like("IN $num-var DO $num-scalar * $num-scalar", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("IN/MULTIPLY statement outside PROCEDURE section", state.where);
        // C Code
        state.add_code(get_c_variable(state, tokens[1]) + " = " +
                           get_c_expression(state, tokens[3]) + " * " +
                           get_c_expression(state, tokens[5]) + ";",
                       state.where);
        return;
    }
    if (line_like("IN $num-var DO $num-scalar / $num-scalar", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("IN/DIVIDE statement outside PROCEDURE section", state.where);
        // C Code
        state.add_code(get_c_variable(state, tokens[1]) + " = " +
                           get_c_expression(state, tokens[3]) + " / " +
                           get_c_expression(state, tokens[5]) + ";",
                       state.where);
        return;
    }
    if (line_like("RANDOMIZE $num-var", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("RANDOMIZE outside PROCEDURE section", state.where);
        // C++ Code
        state.add_code(get_c_variable(state, tokens[1]) + " = get_random();",
                       state.where);
        return;
    }
    if (line_like("IN $num-var FLOOR $num-var", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("IN/FLOOR statement outside PROCEDURE section", state.where);
        // C++ Code
        state.add_code(get_c_variable(state, tokens[1]) + " = floor(" +
                           get_c_variable(state, tokens[3]) + ");",
                       state.where);
        return;
    }
    if (line_like("IN $num-var CEIL $num-var", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("IN/CEIL statement outside PROCEDURE section", state.where);
        // C++ Code
        state.add_code(get_c_variable(state, tokens[1]) + " = ceil(" +
                           get_c_variable(state, tokens[3]) + ");",
                       state.where);
        return;
    }

    // Text Statements
    if (line_like("IN $str-var CHAR AT $num-scalar OF $str-scalar", tokens,
                  state))
    {
        if (!in_procedure_section(state))
            badcode("IN/GET CHARACTER statement outside PROCEDURE section", state.where);
        // C++ Code
        state.add_code(get_c_variable(state, tokens[1]) + " = charat(" +
                           get_c_expression(state, tokens[5]) + ", " +
                           get_c_expression(state, tokens[3]) + ");",
                       state.where);
        return;
    }
    if (line_like("IN $num-var LEN $scalar", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("IN/GET LENGTH OF outside PROCEDURE section", state.where);
        // C++ Code
        state.add_code(get_c_variable(state, tokens[1]) + " = ((graphemedText)" +
                           get_c_expression(state, tokens[3]) + ").size();",
                       state.where);
        return;
    }
    if (line_like("IN $str-var JOIN $display", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("IN-JOIN statement outside PROCEDURE section", state.where);
        if (tokens.size() < 5)
            badcode("IN-JOIN expects at least two values to join", state.where);
        // C++ Code
        state.add_code("ldpl_local_joinvar_mutex.lock();");
        state.add_code("ldpl_local_joinvar = \"\";", state.where);
        for (unsigned int i = 3; i < tokens.size(); ++i)
        {
            state.add_code(
                "ldpl_local_joinvar += " + get_c_string(state, tokens[i]) + ";",
                state.where);
        }
        state.add_code(get_c_variable(state, tokens[1]) + " = ldpl_local_joinvar;",
                       state.where);
        state.add_code("ldpl_local_joinvar_mutex.unlock();");
        return;
    }

    // I/O Statements
    if (line_like("PRINT $display", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("PRINT statement outside PROCEDURE section", state.where);
        // C++ Code
        for (unsigned int i = 1; i < tokens.size(); ++i)
        {
            if (is_scalar_variable(tokens[i], state))
            {
                state.add_code(
                    "cout << " + get_c_variable(state, tokens[i]) + " << flush;",
                    state.where);
            }
            else
            {
                state.add_code("cout << " + tokens[i] + " << flush;", state.where);
            }
        }
        return;
    }
    if (line_like("ACCEPT $var", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("ACCEPT statement outside PROCEDURE section", state.where);
        // C++ Code
        if (is_num_var(tokens[1], state))
            state.add_code(get_c_variable(state, tokens[1]) + " = input_number();",
                           state.where);
        else
            state.add_code(get_c_variable(state, tokens[1]) + " = input_string();",
                           state.where);
        return;
    }
    if (line_like("EXECUTE $str-scalar", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("EXECUTE outside PROCEDURE section", state.where);
        // C++ Code
        state.add_code("exec(" + get_c_char_array(state, tokens[1]) + ");",
                       state.where);
        return;
    }
    if (line_like("ACCEPT $str-var UNTIL EOF", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("ACCEPT statement outside PROCEDURE section", state.where);
        // C++ Code
        state.add_code(get_c_variable(state, tokens[1]) + " = input_until_eof();",
                       state.where);
        return;
    }
    if (line_like("IN $str-var LOAD $scalar", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("IN/LOAD FILE statement outside PROCEDURE section", state.where);
        // C++ Code
        state.add_code("load_file(" + get_c_expression(state, tokens[3]) + ", " +
                           get_c_variable(state, tokens[1]) + ");",
                       state.where);
        return;
    }
    if (line_like("WRITE $scalar TO $scalar", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("WRITE statement outside PROCEDURE section", state.where);
        // C++ Code
        state.add_code("write_file(" + get_c_expression(state, tokens[3]) + ", " +
                           get_c_expression(state, tokens[1]) + ");",
                       state.where);
        return;
    }
    if (line_like("APPEND $scalar TO $scalar", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("APPEND statement outside PROCEDURE section", state.where);
        // C++ Code
        state.add_code("append_to_file(" + get_c_expression(state, tokens[3]) +
                           ", " + get_c_expression(state, tokens[1]) + ");",
                       state.where);
        return;
    }
    if (line_like("CLEAR $map", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("CLEAR statement outside PROCEDURE section", state.where);
        // C++ Code
        state.add_code(
            get_c_variable(state, tokens[1]) + ".inner_collection.clear();",
            state.where);
        return;
    }
    if (line_like("IN $num-var COUNT $map", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("IN/GET KEY COUNT statement outside PROCEDURE section", state.where);
        // C++ Code
        state.add_code(get_c_variable(state, tokens[1]) + " = " +
                           get_c_variable(state, tokens[3]) +
                           ".inner_collection.size();",
                       state.where);
        return;
    }
    if (line_like("IN $str-map KEYS $map", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("IN/GET KEYS statement outside PROCEDURE section", state.where);
        // C++ Code
        state.add_code("get_indices(" + get_c_variable(state, tokens[1]) + ", " +
                           get_c_variable(state, tokens[3]) + ");",
                       state.where);
        return;
    }
    if (line_like("INC $num-var", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("INREMENT statement outside PROCEDURE section", state.where);
        // C Code
        state.add_code(get_c_variable(state, tokens[1]) + " += 1;",
                       state.where);
        return;
    }
    if (line_like("DEC $num-var", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("DEREMENT statement outside PROCEDURE section", state.where);
        // C Code
        state.add_code(get_c_variable(state, tokens[1]) + " -= 1;",
                       state.where);
        return;
    }
    badcode("Malformed statement", state.where);
}