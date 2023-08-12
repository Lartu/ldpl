/* This file contains the line_like function used for pattern matching lines */

// +---------------------------------------------+
// | TODO: comment and format this file properly |
// +---------------------------------------------+

// Check if the tokens of a line passed are like the ones of a model line
bool line_like(string model_line, vector<string> &tokens,
               compiler_state &state) {
  // Tokenize model line
  vector<string> model_tokens;
  tokenize(model_line, model_tokens, state.where, false, ' ');
  // Check that tokens match between line and model line
  if (tokens.size() < model_tokens.size()) return false;
  unsigned int i = 0;
  unsigned int j = 0;
  for (; i < model_tokens.size(); ++i) {
    if (model_tokens[i] == "$name")  // $name is a valid identifier for a
                                     // variable or a sub-procedure
    {
      for (char letter : tokens[j])
        if (letter == ':') return false;
      for (char letter : tokens[j])
        if (letter == '\"') return false;
      for (char letter : tokens[j])
        if (letter == '(') return false;
      for (char letter : tokens[j])
        if (letter == ')') return false;
      if (is_number(tokens[j])) return false;
      if (tokens[j] == "+") return false;
      if (tokens[j] == "-") return false;
      if (tokens[j] == "*") return false;
      if (tokens[j] == "/") return false;
    } else if (model_tokens[i] == "$num-var")  // $num-var is NUMBER variable
    {
      if (!is_num_var(tokens[j], state)) return false;
    } else if (model_tokens[i] == "$str-var")  // $str-var is TEXT variable
    {
      if (!is_txt_var(tokens[j], state)) return false;
    } else if (model_tokens[i] ==
               "$var")  // $var is either a NUMBER variable or a TEXT variable
    {
      if (!is_scalar_variable(tokens[j], state)) return false;
    } else if (model_tokens[i] == "$anyVar")  // $anyVar is any variable
    {
      if (!variable_exists(tokens[j], state)) return false;
    } else if (model_tokens[i] ==
               "$scalar-map") {  // $scalar-map is TEXT MAP, NUMBER MAP
      if (!is_scalar_map(tokens[j], state)) return false;
    } else if (model_tokens[i] == "$num-vec")  // $num-vec is NUMBER vector
    {
      if (!is_num_map(tokens[j], state)) return false;
    } else if (model_tokens[i] == "$str-vec")  // $str-vec is TEXT vector
    {
      if (!is_txt_map(tokens[j], state)) return false;
    } else if (model_tokens[i] == "$list") {  // $list is a LIST
      if (variable_type(tokens[j], state).back() != 3) return false;
    } else if (model_tokens[i] == "$map") {  // $map is a MAP
      if (variable_type(tokens[j], state).back() != 4) return false;
    } else if (model_tokens[i] ==
               "$scalar-list") {  // $scalar-list is a LIST of scalar values
      if (!is_scalar_list(tokens[j], state)) return false;
    } else if (model_tokens[i] == "$num-list")  // $num-vec is NUMBER list
    {
      if (!is_num_list(tokens[j], state)) return false;
    } else if (model_tokens[i] == "$str-list")  // $str-vec is TEXT list
    {
      if (!is_txt_list(tokens[j], state)) return false;
    } else if (model_tokens[i] == "$list-list")  // $str-vec is a LIST of LISTs
    {
      if (!is_list_list(tokens[j], state)) return false;
    } else if (model_tokens[i] == "$map-list")  // $str-vec is LIST of MAPs
    {
      if (!is_map_list(tokens[j], state)) return false;
    } else if (model_tokens[i] ==
               "$collection")  // $collection is either a MAP or a LIST
    {
      // if(!is_scalar_map(tokens[j], state) && !is_scalar_list(tokens[j],
      // state) && (variable_type(tokens[j], state).size() < 2)) return false;
      if (variable_type(tokens[j], state).size() < 2) return false;
    } else if (model_tokens[i] ==
               "$literal")  // $literal is either a NUMBER or a TEXT
    {
      if (!is_string(tokens[j]) && !is_number(tokens[j])) return false;
    } else if (model_tokens[i] == "$string")  // $string is a TEXT literal
    {
      if (!is_string(tokens[j])) return false;
    } else if (model_tokens[i] == "$number")  // $number is a NUMBER literal
    {
      if (!is_number(tokens[j])) return false;
    } else if (model_tokens[i] == "$expression")  // $expression is NUMBER,
                                                  // TEXT, TEXT-VAR, NUMBER-VAR
    {
      if (!is_expression(tokens[j], state)) return false;
    } else if (model_tokens[i] ==
               "$str-expr")  // $str-expr is either a TEXT or a TEXT variable
    {
      if (!is_txt_expr(tokens[j], state)) return false;
    } else if (model_tokens[i] == "$num-expr")  // $num-expr is either a NUMBER
                                                // or a NUMBER variable
    {
      if (!is_num_expr(tokens[j], state)) return false;
    } else if (model_tokens[i].find("$var-type-") ==
               0)  // variable with a given type number
    {
      vector<unsigned int> actual_type = variable_type(tokens[j], state);
      string expected_type = model_tokens[i].substr(10);
      if (actual_type.size() != expected_type.length()) return false;
      for (size_t t = 0; t < actual_type.size(); ++t) {
        if ((int)actual_type[t] != expected_type[t] - '0') return false;
      }
    } else if (model_tokens[i] ==
               "$natural")  // $natural is an integer greater than 0
    {
      if (!is_natural(tokens[j])) return false;
    } else if (model_tokens[i] ==
               "$display")  // multiple NUMBER, TEXT, TEXT-VAR, NUMBER-VAR
    {
      for (; j < tokens.size(); ++j) {
        if (!is_expression(tokens[j], state)) return false;
      }
    } else if (model_tokens[i] ==
               "$subprocedure")  // $subprocedure is a SUB-PROCEDURE
    {
      if (!is_subprocedure(tokens[j], state)) return false;
    } else if (model_tokens[i] ==
               "$external")  // $external is a C++ function defined elsewhere
    {
      return !is_subprocedure(tokens[j], state) &&
             !is_expression(tokens[j], state);
    } else if (model_tokens[i] == "$label")  // $label is a GOTO label
    {
      return is_label(tokens[j]);
    } else if (model_tokens[i] == "$math")  // $math is a math expression
    {
      vector<string> maths;  // further tokenize math expressions
      string math_token = "";
      for (; j < tokens.size(); ++j) {
        for (unsigned int z = 0; z < tokens[j].size(); ++z) {
          if (tokens[j][z] == '(' || tokens[j][z] == ')') {
            if (!math_token.empty()) maths.push_back(math_token);
            math_token = tokens[j][z];
            maths.push_back(math_token);
            math_token = "";
          } else {
            math_token += tokens[j][z];
          }
        }
        if (!math_token.empty()) maths.push_back(math_token);
        math_token = "";
      }
      // replace LDPL line tokens with new math tokens
      tokens.erase(tokens.begin() + i, tokens.end());
      tokens.insert(tokens.end(), maths.begin(), maths.end());

      // validate the new tokens
      for (unsigned int z = i; z < tokens.size(); ++z) {
        if (!is_math_symbol(tokens[z]) && !is_expression(tokens[z], state))
          return false;
      }
      return true;
    } else if (model_tokens[i] ==
               "$condition")  // $condition is a IF/WHILE condition
    {
      // Skip to the last token (THEN/DO),
      // the condition is validated in get_c_condition
      j = tokens.size() - 1;
      continue;
    } else if (model_tokens[i] == "$anything")
      return true;
    else if (model_tokens[i] != tokens[j])
      return false;
    ++j;
  }
  if (j < tokens.size()) return false;
  return true;
}