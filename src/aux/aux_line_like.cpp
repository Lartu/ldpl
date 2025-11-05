/* This file contains the line_like function used for pattern matching lines */

// +---------------------------------------------+
// | TODO: comment and format this file properly |
// +---------------------------------------------+

#include "../ldpl.h"

// Check if the tokens of a line passed are like the ones of a model line
bool line_like(string model_line, const vector<string> &tokens, compiler_state &state)
{
    // Tokenize model line
    vector<string> model_tokens;
    tokenize(model_line, model_tokens, state.where, false, ' ');
    // Check that tokens match between line and model line
    if (tokens.size() < model_tokens.size())
    {
        return false;
    }
    unsigned int token_index = 0;
    for (unsigned int model_token_index = 0; model_token_index < model_tokens.size(); ++model_token_index)
    {
        if (model_tokens[model_token_index] == "$name") // $name is a valid identifier for a
                                                        // variable or a sub-procedure
        {
            if (is_number(tokens[token_index]))
            {
                return false;
            }
            for (char letter : tokens[token_index])
            {
                if (!((letter >= 'A' && letter <= 'Z') || (letter >= 'a' && letter <= 'z') || (letter >= '0' && letter <= '9')))
                {
                    return false;
                }
            }
        }
        else if (model_tokens[model_token_index] == "$num-var") // $num-var is NUMBER variable
        {
            if (!is_num_var(tokens[token_index], state))
            {
                return false;
            }
        }
        else if (model_tokens[model_token_index] == "$str-var") // $str-var is TEXT variable
        {
            if (!is_txt_var(tokens[token_index], state))
            {
                return false;
            }
        }
        // $var is either a NUMBER variable or a TEXT variable
        else if (model_tokens[model_token_index] == "$var")
        {
            if (!is_scalar_variable(tokens[token_index], state))
            {
                return false;
            }
        }
        else if (model_tokens[model_token_index] == "$anyVar") // $anyVar is any variable (scalar or container)
        {
            if (!variable_exists(tokens[token_index], state))
            {
                return false;
            }
        }
        else if (model_tokens[model_token_index] == "$map")
        { // $map is TEXT MAP, NUMBER MAP
            if (!is_scalar_map(tokens[token_index], state))
            {
                return false;
            }
        }
        else if (model_tokens[model_token_index] == "$num-map") // $num-vec is NUMBER MAP
        {
            if (!is_num_map(tokens[token_index], state))
            {
                return false;
            }
        }
        else if (model_tokens[model_token_index] == "$str-map") // $str-vec is TEXT MAP
        {
            if (!is_txt_map(tokens[token_index], state))
            {
                return false;
            }
        }
        else if (model_tokens[model_token_index] == "$list")
        { // $list is a LIST
            if (variable_type(tokens[token_index], state).back() != 3)
            {
                return false;
            }
        }
        else if (model_tokens[model_token_index] == "$literal") // $literal is either a NUMBER or a TEXT
        {
            if (!is_string(tokens[token_index]) && !is_number(tokens[token_index]))
            {
                return false;
            }
        }
        else if (model_tokens[model_token_index] == "$string") // $string is a TEXT literal
        {
            if (!is_string(tokens[token_index]))
            {
                return false;
            }
        }
        else if (model_tokens[model_token_index] == "$number") // $number is a NUMBER literal
        {
            if (!is_number(tokens[token_index]))
            {
                return false;
            }
        }
        else if (model_tokens[model_token_index] == "$scalar") // $scalar is NUMBER, TEXT, TEXT-VAR, NUMBER-VAR
        {
            if (!is_scalar_value(tokens[token_index], state))
            {
                return false;
            }
        }
        else if (model_tokens[model_token_index] == "$str-scalar") // $str-scalar is either a TEXT or a TEXT variable
        {
            if (!is_txt(tokens[token_index], state))
                return false;
        }
        else if (model_tokens[model_token_index] == "$num-scalar") // $num-scalar is either a NUMBER or a NUMBER variable
        {
            if (!is_num(tokens[token_index], state))
            {
                return false;
            }
        }
        else if (model_tokens[model_token_index].find("$var-type-") == 0) // variable with a given type number
        {
            vector<unsigned int> actual_type = variable_type(tokens[model_token_index], state);
            string expected_type = model_tokens[model_token_index].substr(10);
            if (actual_type.size() != expected_type.length())
            {
                return false;
            }
            for (size_t t = 0; t < actual_type.size(); ++t)
            {
                if ((int)actual_type[t] != expected_type[t] - '0')
                {
                    return false;
                }
            }
        }
        else if (model_tokens[model_token_index] == "$natural") // $natural is an integer greater than 0
        {
            if (!is_natural(tokens[token_index]))
            {
                return false;
            }
        }
        else if (model_tokens[model_token_index] == "$display") // multiple NUMBER, TEXT, TEXT-VAR, NUMBER-VAR
        {
            for (unsigned int j = token_index; j < tokens.size(); ++j)
            {
                if (!is_scalar_value(tokens[j], state))
                {
                    return false;
                }
                ++token_index;
            }
        }
        else if (model_tokens[model_token_index] == "$subprocedure") // $subprocedure is a SUB-PROCEDURE
        {
            if (!is_subprocedure(tokens[token_index], state))
            {
                return false;
            }
        }
        else if (model_tokens[model_token_index] == "$condition") // $condition is a IF/WHILE condition
        {
            // Skip to the last token (THEN/DO),
            // the condition is validated in get_c_condition
            token_index = tokens.size() - 1;
            continue;
        }
        else if (model_tokens[model_token_index] == "$arguments")
        {
            // TODO hacer algo que no sea esta forrada
            vector<string> argument_tokens;
            string arguments;
            for (size_t t = token_index; t < tokens.size(); ++t)
            {
                arguments += tokens[t] + " ";
            }
            tokenize(arguments, argument_tokens, state.where, true, ',');
            for (size_t t = 0; t < argument_tokens.size(); ++t)
            {
                trim(argument_tokens[t]);
                if (!is_value(argument_tokens[t], state))
                {
                    return false;
                }
            }
            return true;
        }
        else if (model_tokens[model_token_index] != "$anything" && model_tokens[model_token_index] != tokens[token_index])
        {
            return false;
        }
        ++token_index;
    }
    if (token_index < tokens.size())
    {
        return false;
    }
    return true;
}