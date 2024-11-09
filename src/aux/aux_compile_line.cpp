/* This file contains the behemoth compile_line function that pattern-matches
 * lines and compiles them */

// +---------------------------------------------+
// | TODO: comment and format this file properly |
// +---------------------------------------------+

#include "../ldpl.h"

// Compiles line per line
void compile_line(vector<string> &tokens, compiler_state &state)
{
    // include
    if (line_like("INCLUDE $string", tokens, state))
    {
        if (state.section_state != 0)
            badcode(
                "you can only use the INCLUDE statement before the DATA and "
                "PROCEDURE sections",
                state.where);
        else
        {
            string file_to_compile = tokens[1].substr(1, tokens[1].size() - 2);
            string separators = "/";
#if defined(_WIN32)
            separators += "\\";
#endif
            size_t last_sep = state.where.current_file.find_last_of(separators);
            code_location old_location = state.where;
            if (last_sep != string::npos)
                file_to_compile = state.where.current_file.substr(0, last_sep) + "/" +
                                  file_to_compile;
            load_and_compile(file_to_compile, state);
            state.section_state = 0;
            state.where = old_location;
        }
        return;
    }

    // extension (INCLUDE but for c++ extensions)
    if (line_like("EXTENSION $string", tokens, state))
    {
        if (state.section_state != 0)
            badcode(
                "you can only use the EXTENSION statement before the DATA and "
                "PROCEDURE sections",
                state.where);
        else
        {
            string file_to_add = tokens[1].substr(1, tokens[1].size() - 2);
            string separators = "/";
#if defined(_WIN32)
            separators += "\\";
#endif
            size_t last_sep = state.where.current_file.find_last_of(separators);
            if (last_sep != string::npos)
                file_to_add =
                    state.where.current_file.substr(0, last_sep) + "/" + file_to_add;
            extensions.push_back(file_to_add);
        }
        return;
    }

    // extension flags (for the C++ compiler)
    if (line_like("FLAG $string", tokens, state))
    {
        if (state.section_state != 0)
            badcode(
                "you can only use the FLAG statement before the DATA and PROCEDURE "
                "sections",
                state.where);
        else
        {
            string flag = tokens[1].substr(1, tokens[1].size() - 2);
            extension_flags.push_back(flag);
        }
        return;
    }
    // os-specific extension flags
    if (line_like("FLAG $name $string", tokens, state))
    {
        if (state.section_state != 0)
            badcode(
                "you can only use the FLAG statement before the DATA and PROCEDURE "
                "sections",
                state.where);
        else
        {
            if (tokens[1] == current_os())
            {
                string flag = tokens[2].substr(1, tokens[2].size() - 2);
                extension_flags.push_back(flag);
            }
        }
        return;
    }

    // Sections
    if (line_like("DATA:", tokens, state))
    {
        if (state.section_state == 1)
            badcode("Duplicate DATA section declaration", state.where);
        if (state.section_state >= 2)
            badcode("DATA section declaration within PROCEDURE section", state.where);
        state.section_state = 1;
        return;
    }
    if (line_like("PROCEDURE:", tokens, state))
    {
        if (state.section_state == 2)
            badcode("Duplicate PROCEDURE section declaration", state.where);
        if (state.current_subprocedure != "" && state.section_state >= 3)
            open_subprocedure_code(state);
        state.section_state = 2;
        return;
    }
    if (line_like("PARAMETERS:", tokens, state))
    {
        if (state.current_subprocedure == "")
            badcode("PARAMETERS section outside subprocedure", state.where);
        if (state.section_state == 4)
            badcode("Duplicate PARAMETERS section declaration", state.where);
        if (state.section_state == 1)
            badcode("PARAMETERS section declaration within LOCAL DATA section",
                    state.where);
        if (state.section_state == 2)
            badcode("PARAMETERS section declaration within PROCEDURE section",
                    state.where);
        state.section_state = 4;
        return;
    }
    if (line_like("LOCAL DATA:", tokens, state))
    {
        if (state.current_subprocedure == "")
            badcode("LOCAL DATA section outside subprocedure", state.where);
        if (state.section_state == 1)
            badcode("Duplicate LOCAL DATA section declaration", state.where);
        if (state.section_state == 2)
            badcode("LOCAL DATA section declaration within PROCEDURE section",
                    state.where);
        state.section_state = 1;
        open_subprocedure_code(state);
        return;
    }

    // Variable Declaration
    if (line_like("$name IS $anything", tokens,
                  state)) // If it's a variable declaration
    {
        string extern_keyword = "";       // C++ extern keyword to prepend to the type
                                          // (empty if not EXTERNAL)
        vector<unsigned int> type_number; // All data types in LDPL have a list of
                                          // numbers that represent its type
        string assign_default;            // Default Value to asign to the variable
        size_t i =
            2;                  // i is used to check all the words after 'IS' and thus starts in 2.
        bool valid_type = true; // Used to check if it's a valid tipe
        if (tokens[i] == "EXTERNAL" &&
            state.current_subprocedure ==
                "")
        { // EXTERNAL is only valid in DATA section (not in LOCAL DATA)
            state.externals[tokens[0]] =
                true;                   // Add it to the list of external variables
            extern_keyword = "extern "; // Set the prepended keyword to 'extern'
            ++i;                        // Check the next word.
        }
        if (tokens[i] == "NUMBER")
        {                             // If it's a number...
            type_number.push_back(1); // Then the type number is 1
            if (extern_keyword == "")
                assign_default = " = 0"; // if its not an external variable, set a
                                         // default value for it.
        }
        else if (tokens[i] ==
                 "TEXT")
        {                             // If we are dealing with a text variable...
            type_number.push_back(2); // The type number is 2
            if (extern_keyword == "")
                assign_default = " = \"\""; // And if it's not external, we set it to a
                                            // default value.
        }
        else if ((tokens[i] == "MAP" || tokens[i] == "LIST") &&
                 tokens.size() > i && tokens[i + 1] == "OF")
        {
            // nested 'of' syntax, ex: map of list of number
            while (valid_type && i < tokens.size())
            {
                if (i % 2 == 1)
                {
                    if (tokens[i] != "OF")
                        valid_type = false;
                }
                else if (i % 2 == 0)
                {
                    if (tokens[i] == "MAP" || (tokens[i] == "MAPS" && i > 0))
                    {
                        type_number.push_back(4);
                    }
                    else if (tokens[i] == "LIST" || (tokens[i] == "LISTS" && i > 0))
                    {
                        type_number.push_back(3);
                    }
                    else if (tokens.size() - 1 > i)
                    {
                        // text and number must be the final type listed
                        valid_type = false;
                    }
                    else if (tokens[i] == "TEXT" || tokens[i] == "TEXTS")
                    {
                        type_number.push_back(2);
                    }
                    else if (tokens[i] == "NUMBER" || tokens[i] == "NUMBERS")
                    {
                        type_number.push_back(1);
                    }
                    else
                    {
                        valid_type = false;
                    }
                }
                else
                {
                    valid_type = false;
                }
                ++i;
            }
            reverse(begin(type_number), end(type_number));
        }
        else
        {
            valid_type = false; // If its not a NUMBER, a TEXT or a collection of
                                // these data types
        } // then it's not a valid LDPL data type.
        ++i; // Move to the next keyword.
        while (valid_type &&
               i < tokens.size())
        { // If up to this point we got a valid data
          // type, we check for containers.
            assign_default =
                ""; // Collections are not initialized with any default values.
            if (tokens[i] == "MAP" ||
                tokens[i] == "VECTOR")
            {                             // If its a MAP (aka VECTOR)
                type_number.push_back(4); // We add the MAP type (4) to its type list
            }
            else if (tokens[i] == "LIST")
            {                             // If its a LIST
                type_number.push_back(3); // We add the LIST type (3) to its type list
            }
            else
            {                       // If the container is not a VECTOR nor a MAP
                valid_type = false; // then it's not a valid data type.
            }
            ++i; // Move to the next keyword.
        }
        if (valid_type && i >= tokens.size() - 1)
        {
            if (state.section_state != 1 && state.section_state != 4)
                badcode(
                    "Variable declaration outside DATA, PARAMETERS or LOCAL DATA "
                    "section",
                    state.where);
            if (state.variables[state.current_subprocedure].count(tokens[0]) > 0)
                badcode("Duplicate declaration for variable \"" + tokens[0] + "\"",
                        state.where);
            state.variables[state.current_subprocedure][tokens[0]] = type_number;
            if (state.section_state == 1)
            { // DATA or LOCAL DATA
                string identifier = fix_identifier(tokens[0], true, state);
                string type = state.get_c_type(type_number);
                string code =
                    extern_keyword + type + " " + identifier + assign_default + ";";
                if (state.current_subprocedure == "") // DATA
                    state.add_var_code(code);
                else
                    state.add_code(code, state.where); // LOCAL DATA
            }
            else // PARAMETERS
                state.subprocedures[state.current_subprocedure].emplace_back(tokens[0]);
            return;
        }
    }

    // SUB-PROCEDURE Declaration
    if (line_like("SUB-PROCEDURE $name", tokens, state) ||
        line_like("SUB $name", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("SUB-PROCEDURE declaration outside PROCEDURE section",
                    state.where);
        if (is_subprocedure(tokens[1], state))
            badcode("Duplicate declaration for SUB-PROCEDURE \"" + tokens[1] + "\"",
                    state.where);
        if (state.closing_subprocedure())
            badcode("SUB-PROCEDURE declaration inside SUB-PROCEDURE", state.where);
        else if (state.closing_if())
            badcode("SUB-PROCEDURE declaration inside IF", state.where);
        else if (state.closing_loop())
            badcode("SUB-PROCEDURE declaration inside WHILE or FOR", state.where);
        state.section_state = 3;
        state.open_subprocedure(tokens[1]);
        state.subprocedures.emplace(tokens[1], vector<string>());
        return;
    }
    if (line_like("EXTERNAL SUB-PROCEDURE $external", tokens, state) ||
        line_like("EXTERNAL SUB $external", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("EXTERNAL SUB-PROCEDURE declaration outside PROCEDURE section",
                    state.where);
        if (state.closing_subprocedure())
            badcode("SUB-PROCEDURE declaration inside SUB-PROCEDURE", state.where);
        else if (state.closing_if())
            badcode("SUB-PROCEDURE declaration inside IF", state.where);
        else if (state.closing_loop())
            badcode("SUB-PROCEDURE declaration inside WHILE or FOR", state.where);
        else
            state.open_subprocedure(tokens[2]);
        // C++ Code
        state.add_code("void " + fix_external_identifier(tokens[2], false) + "(){",
                       state.where);
        return;
    }
    if (line_like("END SUB-PROCEDURE", tokens, state) ||
        line_like("END SUB", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("END SUB-PROCEDURE outside PROCEDURE section", state.where);
        if (!state.closing_subprocedure())
            badcode("END SUB-PROCEDURE without SUB-PROCEDURE", state.where);
        // C++ Code
        state.add_code("return;}", state.where);
        // Cierro la subrutina
        state.close_subprocedure();
        return;
    }

    // Control Flow Statements
    if (line_like("STORE $expression IN $var", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("STORE statement outside PROCEDURE section", state.where);
        // C++ Code
        string lhand;
        if (is_num_var(tokens[3], state))
            lhand = get_c_number(state, tokens[1]);
        else
            lhand = get_c_string(state, tokens[1]);
        state.add_code(get_c_variable(state, tokens[3]) + " = " + lhand + ";", state.where);
        return;
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
    if (line_like("END IF", tokens, state) ||
        line_like("END-IF", tokens, state))
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
    if (line_like("FOR $num-var FROM $num-expr TO $num-expr STEP $num-expr DO", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("FOR outside PROCEDURE section", state.where);
        state.open_loop();
        string var = get_c_variable(state, tokens[1]);
        string from = get_c_expression(state, tokens[3]);
        string to = get_c_expression(state, tokens[5]);
        string step = get_c_expression(state, tokens[7]);
        string init = var + " = " + from;
        string condition =
            step + " >= 0 ? " + var + " < " + to + " : " + var + " > " + to;
        string increment = var + " += " + step;
        // C++ Code
        state.add_code("for (" + init + "; " + condition + "; " + increment + ") {",
                       state.where);
        return;
    }
    if (line_like("FOR $num-var FROM $num-expr TO $num-expr INCLUSIVE STEP $num-expr DO", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("FOR outside PROCEDURE section", state.where);
        state.open_loop();
        string var = get_c_variable(state, tokens[1]);
        string from = get_c_expression(state, tokens[3]);
        string to = get_c_expression(state, tokens[5]);
        string step = get_c_expression(state, tokens[8]);
        string init = var + " = " + from;
        string condition =
            step + " >= 0 ? " + var + " <= " + to + " : " + var + " >= " + to;
        string increment = var + " += " + step;
        // C++ Code
        state.add_code("for (" + init + "; " + condition + "; " + increment + ") {",
                       state.where);
        return;
    }
    if (line_like("FOR EACH $anyVar IN $collection DO", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("FOR EACH outside PROCEDURE section", state.where);
        vector<unsigned int> iteration_type = variable_type(tokens[2], state);
        vector<unsigned int> collected_type = variable_type(tokens[4], state);
        unsigned int collection_type = collected_type.back(); // LIST or MAP
        collected_type.pop_back();
        if (collection_type == 3 && iteration_type != collected_type)
            badcode("FOR EACH iteration variable type doesn't match LIST type",
                    state.where);
        else if (collection_type == 4 && iteration_type != vector<unsigned int>{2})
            badcode("FOR EACH iteration variable type must be TEXT on MAP iteration",
                    state.where);
        state.open_loop();
        // C Code
        string range_var = state.new_range_var();
        string collection = get_c_variable(state, tokens[4]) + ".inner_collection";
        string iteration_var = range_var;
        if (collection_type == 4)
        {
            iteration_var += ".first";
        }
        state.add_code("for (auto& " + range_var + " : " + collection + ") {",
                       state.where);
        state.add_code(
            get_c_variable(state, tokens[2]) + " = " + iteration_var + ";",
            state.where);
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
    if (line_like("CALL EXTERNAL $external", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("CALL EXTERNAL outside PROCEDURE section", state.where);
        state.add_code(fix_external_identifier(tokens[2], false) + "();",
                       state.where);
        // prototype of function defined in extension
        state.add_var_code("void " + fix_external_identifier(tokens[2], false) +
                           "();");
        return;
    }
    if (line_like("CALL SUB-PROCEDURE $anything", tokens, state) ||
        line_like("CALL $anything", tokens, state))
    {
        size_t i = 1;
        if (tokens[i] == "SUB-PROCEDURE")
            i++;
        string subprocedure = tokens[i];
        // Valid options: No WITH or WITH with at least one paramter
        if (i == tokens.size() - 1 ||
            (i < tokens.size() - 2 && tokens[i + 1] == "WITH"))
        {
            if (!in_procedure_section(state))
                badcode("CALL outside PROCEDURE section", state.where);
            vector<string> parameters(
                i != tokens.size() - 1 ? tokens.begin() + i + 2 : tokens.end(),
                tokens.end());
            vector<vector<unsigned int>> types;
            for (string &parameter : parameters)
            {
                if (is_number(parameter))
                    types.push_back({1});
                else if (is_string(parameter))
                    types.push_back({2});
                else if (variable_exists(parameter, state))
                    types.push_back(variable_type(parameter, state));
                else
                    badcode("CALL with invalid parameter \"" + parameter + "\"",
                            state.where);
            }
            bool correct_types =
                state.correct_subprocedure_types(subprocedure, types);
            if (!is_subprocedure(subprocedure, state))
            {
                if (!correct_types)
                    badcode("CALL parameter types don't match previous CALL",
                            state.where);
                state.add_expected_subprocedure(
                    subprocedure, fix_identifier(subprocedure, false), types);
            }
            else
            {
                if (!correct_types)
                    badcode("CALL parameter types don't match SUB-PROCEDURE declaration",
                            state.where);
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
    if (line_like("WAIT $num-expr MILLISECONDS", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("WAIT statement outside PROCEDURE section", state.where);
// C++ Code
#if defined(_WIN32)
        state.add_code(
            "_sleep(" + get_c_expression(state, tokens[1]) + ".to_long_long());",
            state.where);
#else
        state.add_code(
            "std::this_thread::sleep_for(std::chrono::milliseconds(" +
                get_c_expression(state, tokens[1]) + ".to_long_long()));",
            state.where);
#endif
        return;
    }
    if (line_like("GOTO $label", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("GOTO outside PROCEDURE section", state.where);
        state.add_code("goto label_" + fix_identifier(tokens[1]) + ";",
                       state.where);
        return;
    }
    if (line_like("LABEL $label", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("LABEL outside PROCEDURE section", state.where);
        state.add_code("label_" + fix_identifier(tokens[1]) + ":", state.where);
        return;
    }

    // Arithmetic Statements
    if (line_like("MODULO $num-expr BY $num-expr IN $num-var", tokens,
                  state)) // TODO move this into the standard library
    {
        if (!in_procedure_section(state))
            badcode("MODULO statement outside PROCEDURE section", state.where);
        // C++ Code
        state.add_code(get_c_variable(state, tokens[5]) + " = modulo(" +
                           get_c_expression(state, tokens[1]) + ", " +
                           get_c_expression(state, tokens[3]) + ");",
                       state.where);
        return;
    }
    if (line_like("RAISE $num-expr TO $num-expr IN $num-var", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("RAISE statement outside PROCEDURE section", state.where);
        // C++ Code
        state.add_code(get_c_variable(state, tokens[5]) + " = pow(" +
                           get_c_expression(state, tokens[1]) + ", " +
                           get_c_expression(state, tokens[3]) + ");",
                       state.where);
        return;
    }
    if (line_like("LOG $num-expr IN $num-var", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("LOG statement outside PROCEDURE section", state.where);
        // C++ Code
        state.add_code(get_c_variable(state, tokens[3]) + " = log(" +
                           get_c_expression(state, tokens[1]) + ");",
                       state.where);
        return;
    }
    if (line_like("SIN $num-expr IN $num-var", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("SIN statement outside PROCEDURE section", state.where);
        // C++ Code
        state.add_code(get_c_variable(state, tokens[3]) + " = sin(" +
                           get_c_expression(state, tokens[1]) + ");",
                       state.where);
        return;
    }
    if (line_like("COS $num-expr IN $num-var", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("COS statement outside PROCEDURE section", state.where);
        // C++ Code
        state.add_code(get_c_variable(state, tokens[3]) + " = cos(" +
                           get_c_expression(state, tokens[1]) + ");",
                       state.where);
        return;
    }
    if (line_like("TAN $num-expr IN $num-var", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("TAN statement outside PROCEDURE section", state.where);
        // C++ Code
        state.add_code(get_c_variable(state, tokens[3]) + " = tan(" +
                           get_c_expression(state, tokens[1]) + ");",
                       state.where);
        return;
    }
    if (line_like("ADD $num-expr AND $num-expr IN $num-var", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("ADD statement outside PROCEDURE section", state.where);
        // C Code
        state.add_code(get_c_variable(state, tokens[5]) + " = " +
                           get_c_expression(state, tokens[1]) + " + " +
                           get_c_expression(state, tokens[3]) + ";",
                       state.where);
        return;
    }
    if (line_like("SUBTRACT $num-expr FROM $num-expr IN $num-var", tokens,
                  state))
    {
        if (!in_procedure_section(state))
            badcode("SUBTRACT statement outside PROCEDURE section", state.where);
        // C Code
        state.add_code(get_c_variable(state, tokens[5]) + " = " +
                           get_c_expression(state, tokens[3]) + " - " +
                           get_c_expression(state, tokens[1]) + ";",
                       state.where);
        return;
    }
    if (line_like("MULTIPLY $num-expr BY $num-expr IN $num-var", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("MULTIPLY statement outside PROCEDURE section", state.where);
        // C Code
        state.add_code(get_c_variable(state, tokens[5]) + " = " +
                           get_c_expression(state, tokens[1]) + " * " +
                           get_c_expression(state, tokens[3]) + ";",
                       state.where);
        return;
    }
    if (line_like("DIVIDE $num-expr BY $num-expr IN $num-var", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("DIVIDE statement outside PROCEDURE section", state.where);
        // C Code
        state.add_code(get_c_variable(state, tokens[5]) + " = " +
                           get_c_expression(state, tokens[1]) + " / " +
                           get_c_expression(state, tokens[3]) + ";",
                       state.where);
        return;
    }
    if (line_like("GET RANDOM IN $num-var", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("RANDOM outside PROCEDURE section", state.where);
        // C++ Code
        state.add_code(get_c_variable(state, tokens[3]) + " = get_random();",
                       state.where);
        return;
    }
    if (line_like("FLOOR $num-var", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("FLOOR statement outside PROCEDURE section", state.where);
        // C++ Code
        state.add_code(get_c_variable(state, tokens[1]) + " = floor(" +
                           get_c_variable(state, tokens[1]) + ");",
                       state.where);
        return;
    }
    if (line_like("CEIL $num-var", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("CEIL statement outside PROCEDURE section", state.where);
        // C++ Code
        state.add_code(get_c_variable(state, tokens[1]) + " = ceil(" +
                           get_c_variable(state, tokens[1]) + ");",
                       state.where);
        return;
    }
    if (line_like("FLOOR $num-var IN $num-var", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("FLOOR statement outside PROCEDURE section", state.where);
        // C++ Code
        state.add_code(get_c_variable(state, tokens[3]) + " = floor(" +
                           get_c_variable(state, tokens[1]) + ");",
                       state.where);
        return;
    }
    if (line_like("CEIL $num-var IN $num-var", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("CEIL statement outside PROCEDURE section", state.where);
        // C++ Code
        state.add_code(get_c_variable(state, tokens[3]) + " = ceil(" +
                           get_c_variable(state, tokens[1]) + ");",
                       state.where);
        return;
    }
    if (line_like("IN $num-var SOLVE $math", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("IN-SOLVE statement outside PROCEDURE section", state.where);

        string code = "";
        for (unsigned int i = 3; i < tokens.size(); ++i)
        {
            if (is_num_var(tokens[i], state))
                code += " " + get_c_variable(state, tokens[i]);
            else if (is_txt_expr(tokens[i], state))
                code += " " + get_c_number(state, tokens[i]);
            else
                code += " " + tokens[i];
        }
        state.add_code(get_c_variable(state, tokens[1]) + " =" + code + ";",
                       state.where);
        return;
    }

    // Text Statements
    if (line_like("JOIN $expression AND $expression IN $str-var", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("JOIN statement outside PROCEDURE section", state.where);
        // C++ Code
        if(tokens[5] == tokens[1])
        {
            // Optimization for appending
            state.add_code(get_c_variable(state, tokens[5]) + " += " + get_c_string(state, tokens[3]) + ";", state.where);
        }else{
            state.add_code("join(" + get_c_string(state, tokens[1]) + ", " + get_c_string(state, tokens[3]) + ", " + get_c_string(state, tokens[5]) + ");", state.where);
        }
        return;
    }
    if (line_like("GET CHARACTER AT $num-expr FROM $str-expr IN $str-var", tokens,
                  state))
    {
        if (!in_procedure_section(state))
            badcode("GET CHARACTER statement outside PROCEDURE section", state.where);
        // C++ Code
        state.add_code(get_c_variable(state, tokens[7]) + " = charat(" +
                           get_c_expression(state, tokens[5]) + ", " +
                           get_c_expression(state, tokens[3]) + ");",
                       state.where);
        return;
    }
    if (line_like("GET LENGTH OF $str-expr IN $num-var", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("GET LENGTH OF outside PROCEDURE section", state.where);
        // C++ Code
        state.add_code(get_c_variable(state, tokens[5]) + " = ((graphemedText)" +
                           get_c_expression(state, tokens[3]) + ").size();",
                       state.where);
        return;
    }
    if (line_like("GET ASCII CHARACTER $num-expr IN $str-var", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("GET ASCII CHARACTER statement outside PROCEDURE section",
                    state.where);
        // C++ Code
        state.add_code(get_c_variable(state, tokens[5]) + " = getAsciiChar(" +
                           get_c_expression(state, tokens[3]) + ");",
                       state.where);
        return;
    }
    if (line_like("GET CHARACTER CODE OF $str-expr IN $num-var", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("GET CHARACTER CODE OF statement outside PROCEDURE section",
                    state.where);
        // C++ Code
        state.add_code(get_c_variable(state, tokens[6]) + " = get_char_num(" +
                           get_c_expression(state, tokens[4]) + ");",
                       state.where);
        return;
    }
    if (line_like("STORE QUOTE IN $str-var", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("STORE QUOTE IN statement outside PROCEDURE section",
                    state.where);
        state.open_quote = true;
        // C++ Code. More strings will get emitted
        state.add_code(get_c_variable(state, tokens[3]) + " = \"\"", state.where);
        return;
    }
    if (line_like("END QUOTE", tokens, state))
        badcode("END QUOTE statement without preceding STORE QUOTE statement",
                state.where);
    if (line_like("IN $str-var JOIN $display", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("IN-JOIN statement outside PROCEDURE section", state.where);
        if (tokens.size() < 5)
            badcode("IN-JOIN expects at least two values to join", state.where);
        // C++ Code
        state.add_code("joinvar = \"\";", state.where);
        for (unsigned int i = 3; i < tokens.size(); ++i)
        {
            state.add_code(
                "joinvar += " + get_c_string(state, tokens[i]) + ";",
                state.where);
        }
        state.add_code(get_c_variable(state, tokens[1]) + " = joinvar;",
                       state.where);
        return;
    }

    // I/O Statements
    if (line_like("DISPLAY $display", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("DISPLAY statement outside PROCEDURE section", state.where);
        // C++ Code
        for (unsigned int i = 1; i < tokens.size(); ++i)
        {
            // TODO ADD COLORS HERE
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
    if (line_like("EXECUTE $str-expr", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("EXECUTE outside PROCEDURE section", state.where);
        // C++ Code
        state.add_code("system(" + get_c_char_array(state, tokens[1]) + ");",
                       state.where);
        return;
    }
    if (line_like("EXECUTE $str-expr AND STORE OUTPUT IN $str-var", tokens,
                  state))
    {
        if (!in_procedure_section(state))
            badcode("EXECUTE outside PROCEDURE section", state.where);
        // C++ Code
        state.add_code(get_c_variable(state, tokens[6]) + " = exec(" +
                           get_c_char_array(state, tokens[1]) + ");",
                       state.where);
        return;
    }
    if (line_like("EXECUTE $str-expr AND STORE EXIT CODE IN $num-var", tokens,
                  state))
    {
        if (!in_procedure_section(state))
            badcode("EXECUTE outside PROCEDURE section", state.where);
        // C++ Code
        state.add_code(get_c_variable(state, tokens[7]) + " = (system(" +
                           get_c_char_array(state, tokens[1]) + ") >> 8) & 0xff;",
                       state.where); // shift wait() val and get lowest 2
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
    if (line_like("LOAD FILE $str-expr IN $str-var", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("LOAD FILE statement outside PROCEDURE section", state.where);
        // C++ Code
        state.add_code("load_file(" + get_c_expression(state, tokens[2]) + ", " +
                           get_c_variable(state, tokens[4]) + ");",
                       state.where);
        return;
    }
    if (line_like("WRITE $expression TO FILE $str-expr", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("WRITE statement outside PROCEDURE section", state.where);
        // C++ Code
        state.add_code("write_file(" + get_c_expression(state, tokens[4]) + ", " +
                           get_c_expression(state, tokens[1]) + ");",
                       state.where);
        return;
    }
    if (line_like("APPEND $expression TO FILE $str-expr", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("APPEND statement outside PROCEDURE section", state.where);
        // C++ Code
        state.add_code("append_to_file(" + get_c_expression(state, tokens[4]) +
                           ", " + get_c_expression(state, tokens[1]) + ");",
                       state.where);
        return;
    }

    if (line_like("REPLACE $str-expr WITH $str-expr FROM $str-expr IN $str-var",
                  tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("REPLACE statement outside PROCEDURE section", state.where);
        // C++ Code
        state.add_code(
            get_c_variable(state, tokens[7]) + " = str_replace(((graphemedText)" +
                get_c_expression(state, tokens[5]) + ").str_rep(), ((graphemedText)" +
                get_c_expression(state, tokens[1]) + ").str_rep(), ((graphemedText)" +
                get_c_expression(state, tokens[3]) + ").str_rep());",
            state.where);
        return;
    }
    // deprecated
    if (line_like("REPLACE $str-expr FROM $str-expr WITH $str-expr IN $str-var",
                  tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("REPLACE statement outside PROCEDURE section", state.where);
        // C++ Code
        state.add_code(
            get_c_variable(state, tokens[7]) + " = str_replace(((graphemedText)" +
                get_c_expression(state, tokens[3]) + ").str_rep(), ((graphemedText)" +
                get_c_expression(state, tokens[1]) + ").str_rep(), ((graphemedText)" +
                get_c_expression(state, tokens[5]) + ").str_rep());",
            state.where);
        return;
    }
    if (line_like("GET INDEX OF $str-expr FROM $str-expr IN $num-var", tokens,
                  state))
    {
        if (!in_procedure_section(state))
            badcode("GET INDEX OF statement outside PROCEDURE section", state.where);
        // C++ Code
        state.add_code(get_c_variable(state, tokens[7]) + " = utf8GetIndexOf(" +
                           get_c_expression(state, tokens[5]) + ", " +
                           get_c_expression(state, tokens[3]) + ");",
                       state.where);
        return;
    }
    if (line_like("COUNT $str-expr FROM $str-expr IN $num-var", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("COUNT statement outside PROCEDURE section", state.where);
        // C++ Code
        state.add_code(get_c_variable(state, tokens[5]) + " = utf8Count(" +
                           get_c_expression(state, tokens[3]) + ", " +
                           get_c_expression(state, tokens[1]) + ");",
                       state.where);
        return;
    }
    if (line_like(
            "SUBSTRING $str-expr FROM $num-expr LENGTH $num-expr IN $str-var",
            tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("SUBSTRING statement outside PROCEDURE section", state.where);
        // C++ Code
        state.add_code("joinvar = " + get_c_expression(state, tokens[1]) + ";",
                       state.where);
        state.add_code(get_c_variable(state, tokens[7]) + " = joinvar.substr(" +
                           get_c_expression(state, tokens[3]) + ", " +
                           get_c_expression(state, tokens[5]) + ");",
                       state.where);
        return;
    }
    if (line_like("TRIM $str-expr IN $str-var", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("TRIM statement outside PROCEDURE section", state.where);
        // C++ Code
        state.add_code(get_c_variable(state, tokens[3]) + " = trimCopy(" +
                           get_c_expression(state, tokens[1]) + ");",
                       state.where);
        return;
    }
    if (line_like("CONVERT $str-expr TO UPPERCASE IN $str-var", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("CONVERT statement outside PROCEDURE section", state.where);
        // C++ Code
        state.add_code(get_c_variable(state, tokens[5]) + " = toUpperCopy(" +
                           get_c_expression(state, tokens[1]) + ");",
                       state.where);
        return;
    }
    if (line_like("CONVERT $str-expr TO LOWERCASE IN $str-var", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("CONVERT statement outside PROCEDURE section", state.where);
        // C++ Code
        state.add_code(get_c_variable(state, tokens[5]) + " = toLowerCopy(" +
                           get_c_expression(state, tokens[1]) + ");",
                       state.where);
        return;
    }
    if (line_like("CLEAR $collection", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("CLEAR statement outside PROCEDURE section", state.where);
        // C++ Code
        state.add_code(
            get_c_variable(state, tokens[1]) + ".inner_collection.clear();",
            state.where);
        return;
    }
    if (line_like("COPY $collection TO $collection", tokens, state))
    {
        if (variable_type(tokens[1], state) == variable_type(tokens[3], state))
        {
            if (!in_procedure_section(state))
                badcode("COPY statement outside PROCEDURE section", state.where);
            // C++ Code
            state.add_code(get_c_variable(state, tokens[3]) + ".inner_collection = " +
                               get_c_variable(state, tokens[1]) +
                               ".inner_collection;",
                           state.where);
            return;
        }
    }
    if (line_like("GET KEY COUNT OF $map IN $num-var", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("GET KEY COUNT statement outside PROCEDURE section", state.where);
        // C++ Code
        state.add_code(get_c_variable(state, tokens[6]) + " = " +
                           get_c_variable(state, tokens[4]) +
                           ".inner_collection.size();",
                       state.where);
        return;
    }
    if (line_like("GET KEYS OF $map IN $str-list", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("GET KEYS statement outside PROCEDURE section", state.where);
        // C++ Code
        state.add_code("get_indices(" + get_c_variable(state, tokens[5]) + ", " +
                           get_c_variable(state, tokens[3]) + ");",
                       state.where);
        return;
    }
    if (line_like("PUSH MAP TO $map-list", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("PUSH statement outside PROCEDURE section", state.where);
        // C++ Code
        state.add_code(
            get_c_variable(state, tokens[3]) + ".inner_collection.emplace_back();",
            state.where);
        return;
    }
    if (line_like("PUSH LIST TO $list-list", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("PUSH statement outside PROCEDURE section", state.where);
        // C++ Code
        state.add_code(
            get_c_variable(state, tokens[3]) + ".inner_collection.emplace_back();",
            state.where);
        return;
    }
    if (line_like("PUSH $expression TO $scalar-list", tokens, state))
    {
        // The type of the pushed element must match the collection type
        if (is_num_expr(tokens[1], state) == is_num_list(tokens[3], state))
        {
            if (!in_procedure_section(state))
                badcode("PUSH statement outside PROCEDURE section", state.where);
            // C++ Code
            state.add_code(get_c_variable(state, tokens[3]) +
                               ".inner_collection.push_back(" +
                               get_c_expression(state, tokens[1]) + ");",
                           state.where);
            return;
        }
        else
        {
            badcode("List - Value type mismatch", state.where);
        }
    }
    if (line_like("GET LENGTH OF $list IN $num-var", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("GET LENGTH OF (list) statement outside PROCEDURE section",
                    state.where);
        // C++ Code
        state.add_code(get_c_variable(state, tokens[5]) + " = " +
                           get_c_variable(state, tokens[3]) +
                           ".inner_collection.size();",
                       state.where);
        return;
    }
    if (line_like("DELETE LAST ELEMENT OF $list", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("DELETE LAST ELEMENT OF statement outside PROCEDURE section",
                    state.where);
        // C++ Code
        state.add_code("if(" + get_c_variable(state, tokens[4]) +
                           ".inner_collection.size() > 0)",
                       state.where);
        state.add_code(
            get_c_variable(state, tokens[4]) + ".inner_collection.pop_back();",
            state.where);
        return;
    }
    if (line_like("REMOVE ELEMENT AT $num-expr FROM $list", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("REMOVE ELEMENT AT statement outside PROCEDURE section",
                    state.where);
        // C++ Code
        state.add_code("if(" + get_c_variable(state, tokens[5]) +
                           ".inner_collection.size() > " +
                           get_c_expression(state, tokens[3]) + ")",
                       state.where);
        state.add_code(
            get_c_variable(state, tokens[5]) + ".inner_collection.erase(" +
                get_c_variable(state, tokens[5]) + ".inner_collection.begin() + " +
                get_c_expression(state, tokens[3]) + ");",
            state.where);
        return;
    }
    if (line_like("SPLIT $str-expr BY $str-expr IN $str-list", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("SPLIT statement outside PROCEDURE section", state.where);
        // C++ Code
        state.add_code("utf8_split_list(" + get_c_variable(state, tokens[5]) + ", " +
                           get_c_expression(state, tokens[1]) + ", " +
                           get_c_expression(state, tokens[3]) + ");",
                       state.where);
        return;
    }
    if (line_like("GET HOUR IN $num-var", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("GET HOUT statement outside PROCEDURE section", state.where);
        // C++ Code
        state.add_code(
            get_c_variable(state, tokens[3]) + " = localtime(&ldpl_time)->tm_hour;",
            state.where);
        return;
    }
    if (line_like("GET MINUTES IN $num-var", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("GET MINUTES statement outside PROCEDURE section", state.where);
        // C++ Code
        state.add_code(
            get_c_variable(state, tokens[3]) + " = localtime(&ldpl_time)->tm_min;",
            state.where);
        return;
    }
    if (line_like("GET SECONDS IN $num-var", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("GET SECONDS statement outside PROCEDURE section", state.where);
        // C++ Code
        state.add_code(
            get_c_variable(state, tokens[3]) + " = localtime(&ldpl_time)->tm_sec;",
            state.where);
        return;
    }
    if (line_like("GET YEAR IN $num-var", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("GET YEAR statement outside PROCEDURE section", state.where);
        // C++ Code
        state.add_code(get_c_variable(state, tokens[3]) +
                           " = localtime(&ldpl_time)->tm_year + 1900;",
                       state.where);
        return;
    }
    if (line_like("GET DAY IN $num-var", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("GET DAY statement outside PROCEDURE section", state.where);
        // C++ Code
        state.add_code(
            get_c_variable(state, tokens[3]) + " = localtime(&ldpl_time)->tm_mday;",
            state.where);
        return;
    }
    if (line_like("GET MONTH IN $num-var", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("GET MONTH statement outside PROCEDURE section", state.where);
        // C++ Code
        state.add_code(get_c_variable(state, tokens[3]) +
                           " = localtime(&ldpl_time)->tm_mon + 1;",
                       state.where);
        return;
    }
    if (line_like("GET MONTH IN $num-var", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("GET MONTH statement outside PROCEDURE section", state.where);
        // C++ Code
        state.add_code(get_c_variable(state, tokens[3]) +
                           " = localtime(&ldpl_time)->tm_mon + 1;",
                       state.where);
        return;
    }
    if (line_like("GET EPOCH IN $num-var", tokens, state))
    {
        if (!in_procedure_section(state))
            badcode("GET EPOCH statement outside PROCEDURE section", state.where);
        // C++ Code
        state.add_code(get_c_variable(state, tokens[3]) + " = ldpl_time;",
                       state.where);
        return;
    }
    // Custom Statements
    if (line_like("CREATE STATEMENT $string EXECUTING $subprocedure", tokens,
                  state))
    {
        if (!in_procedure_section(state))
            badcode("CREATE STATEMENT statement outside PROCEDURE section",
                    state.where);
        if (state.closing_subprocedure())
            badcode("CREATE STATEMENT statement inside SUB-PROCEDURE", state.where);
        else if (state.closing_if())
            badcode("CREATE STATEMENT statement inside IF", state.where);
        else if (state.closing_loop())
            badcode("CREATE STATEMENT statement inside WHILE or FOR", state.where);
        string model_line = tokens[2].substr(1, tokens[2].size() - 2);
        vector<string> model_tokens;
        vector<string> parameters = state.subprocedures[tokens[4]];
        trim(model_line);
        tokenize(model_line, model_tokens, state.where, true, ' ');
        size_t param_count = 0;
        size_t keyword_count = 0;
        string valid_keyword_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        model_line = "";
        for (string &token : model_tokens)
        {
            if (token == "$")
            {
                ++param_count;
                if (param_count > parameters.size())
                    break;
                vector<unsigned int> type =
                    state.variables[tokens[4]][parameters[param_count - 1]];
                if (type == vector<unsigned int>{1})
                    model_line += "$num-expr ";
                else if (type == vector<unsigned int>{2})
                    model_line += "$str-expr ";
                else
                {
                    model_line += "$var-type-";
                    for (size_t i = 0; i < type.size(); ++i)
                    {
                        model_line += to_string(type[i]);
                    }
                    model_line += " ";
                }
            }
            else if (token.find_first_not_of(valid_keyword_chars) == string::npos)
            {
                ++keyword_count;
                model_line += token + " ";
            }
            else
            {
                badcode("CREATE STATEMENT with invalid token \"" + token + "\"",
                        state.where);
            }
        }
        if (param_count != parameters.size())
            badcode("CREATE STATEMENT parameters count doesn't match SUB-PROCEDURE",
                    state.where);
        if (keyword_count == 0)
            badcode("CREATE STATEMENT without keywords", state.where);
        state.custom_statements.emplace_back(model_line, tokens[4]);
        return;
    }
    for (pair<string, string> &statement : state.custom_statements)
    {
        if (line_like(statement.first, tokens, state))
        {
            string prefix = statement.first.substr(0, statement.first.find("$"));
            if (!in_procedure_section(state))
                badcode(prefix + "statement outside PROCEDURE section", state.where);
            vector<string> model_tokens;
            vector<string> parameters;
            tokenize(statement.first, model_tokens, state.where, false, ' ');
            for (size_t i = 0; i < model_tokens.size(); i++)
            {
                if (model_tokens[i][0] == '$')
                    parameters.push_back(tokens[i]);
            }
            add_call_code(statement.second, parameters, state);
            return;
        }
    }

    badcode("Malformed statement", state.where);
}