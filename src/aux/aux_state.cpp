/* This file contains auxiliary functions that check the current compilation
 * state */

bool is_num_map(string &token, compiler_state &state) {
  // -- Returns if the variable is a NUMBER MAP or an access to a multicontainer
  // that results in a NUMBER MAP --
  vector<unsigned int> type = variable_type(token, state);
  if (type.size() == 2 && type[0] == 1 && type[1] == 4) return true;
  return false;
}

bool is_txt_map(string &token, compiler_state &state) {
  // -- Returns if the variable is a TEXT MAP or an access to a multicontainer
  // that results in a TEXT MAP --
  vector<unsigned int> type = variable_type(token, state);
  if (type.size() == 2 && type[0] == 2 && type[1] == 4) return true;
  return false;
}

bool is_num_list(string &token, compiler_state &state) {
  // -- Returns if the variable is a NUMBER LIST or an access to a
  // multicontainer that results in a NUMBER LIST --
  vector<unsigned int> type = variable_type(token, state);
  if (type.size() == 2 && type[0] == 1 && type[1] == 3) return true;
  return false;
}

bool is_txt_list(string &token, compiler_state &state) {
  // -- Returns if the variable is a TEXT MAP or an access to a multicontainer
  // that results in a TEXT MAP --
  vector<unsigned int> type = variable_type(token, state);
  if (type.size() == 2 && type[0] == 2 && type[1] == 3) return true;
  return false;
}

bool is_list_list(string &token, compiler_state &state) {
  // -- Returns if the variable is a NUMBER/TEXT LIST LIST multicontainer or a
  // multicontainer access that results in a LIST of LISTs --
  vector<unsigned int> type = variable_type(token, state);
  if (type.size() >= 2 && type[type.size() - 2] == 3 && type.back() == 3)
    return true;
  return false;
}

bool is_map_list(string &token, compiler_state &state) {
  // -- Returns if the variable is a multicontainer NUMBER/TEXT LIST MAP or a
  // multicontainer access that results in a LIST of MAPs --
  vector<unsigned int> type = variable_type(token, state);
  if (type.size() >= 2 && type[type.size() - 2] == 4 && type.back() == 3)
    return true;
  return false;
}

bool is_scalar_map(string &token, compiler_state &state) {
  // -- Returns if the variable is a NUMBER MAP or an access to a multicontainer
  // that results in a NUMBER MAP --
  // -- or if the variable is a TEXT MAP or an access to a multicontainer that
  // results in a TEXT MAP          --
  return is_num_map(token, state) || is_txt_map(token, state);
}

bool is_map_map(string &token, compiler_state &state) {
  // -- Returns if the variable is a NUMBER/TEXT MAP MAP multicontainer or a
  // multicontainer access that results in a MAP of MAPs --
  vector<unsigned int> type = variable_type(token, state);
  if (type.size() >= 2 && type[type.size() - 2] == 4 && type.back() == 4)
    return true;
  return false;
}

bool is_map(string &token, compiler_state &state) {
  // -- Returns true if the variable is a MAP, regardless of a map of what
  // (multicontainer or not) --
  vector<unsigned int> type = variable_type(token, state);
  return type.back() == 4;
}

bool is_scalar_list(string &token, compiler_state &state) {
  // -- Returns if the variable is a NUMBER LIST or an access to a
  // multicontainer that results in a NUMBER LIST --
  // -- or if the variable is a TEXT LIST or an access to a multicontainer that
  // results in a TEXT LIST          --
  return is_num_list(token, state) || is_txt_list(token, state);
}

bool is_num_var(string &token, compiler_state &state) {
  // -- Checks if token is a NUMBER variable (or an access to a container that
  // results in a NUMBER variable) --
  return (variable_type(token, state) == vector<unsigned int>{1});
}

bool is_txt_var(string &token, compiler_state &state) {
  // -- Checks if token is a TEXT variable (or an access to a container that
  // results in a TEXT variable) --
  return (variable_type(token, state) == vector<unsigned int>{2});
}

bool is_scalar_variable(string &token, compiler_state &state) {
  // -- Returns is an identifier is a valid scalar variable or an access that
  // results in one --
  return is_num_var(token, state) || is_txt_var(token, state);
}

bool is_num_expr(string &token, compiler_state &state) {
  // -- Returns is an identifier is a valid scalar variable or number or an
  // access that results in one --
  return is_num_var(token, state) || is_number(token);
}

bool is_txt_expr(string &token, compiler_state &state) {
  // -- Returns is an identifier is a valid scalar variable or text or an access
  // that results in one --
  return is_txt_var(token, state) || is_string(token);
}

bool is_expression(string &token, compiler_state &state) {
  // -- Returns is an identifier is a valid scalar variable or text or number or
  // an access that results in one --
  return is_num_expr(token, state) || is_txt_expr(token, state);
}

bool is_external(string &token, compiler_state &state) {
  // -- Returns if an identifier maps to an external variable --
  return state.externals[token];
}

bool variable_exists(string &token, compiler_state &state) {
  // -- Returns if a variable has been declared or not --
  // (Bear in mind that myList is a variable, myList:0 is not, that's an access
  // for all this function is concerned)
  return variable_type(token, state) != vector<unsigned int>{0};
}

bool is_subprocedure(string &token, compiler_state &state) {
  // -- Returns if an identifier maps to a valid, existing sub-procedure --
  for (auto &subprocedure : state.subprocedures)
    if (subprocedure.first == token) return true;
  return false;
}

bool in_procedure_section(compiler_state &state) {
  // -- Returns if the compiler is currently compiling a procedure section or
  // not --
  if (state.section_state == 3) {
    // We're inside a SUB-PROCEDURE procedure with no sections
    state.section_state = 2;
    open_subprocedure_code(state);
  }
  return state.section_state == 2;
}

vector<unsigned int> variable_type(string &token, compiler_state &state) {
  // -- Returns the LDPL internal representation of the type of a variable --
  //
  // Return the number of the type or {0} if the variable doesn't exist. This
  // function can take full variables (foo:0:"hi"). Returns all the types of the
  // variable. If foo is number map list {1, 4, 3} and we just pass it foo:0, it
  // will return {1, 4}, that is the type foo:0 has.
  //
  // Variables can have mixed types. For example, a LIST of MAPS of NUMBERS
  // called foo is a NUMBER when you access both containers (foo:0:"hi") a
  // NUMBER MAP when you access just the list (foo:0) or a NUMBER MAP LIST when
  // you access nothing (foo) So we first split the full variable with accesses
  // and everything into tokens by :
  vector<string> tokens;
  string varName = "";
  tokenize(token, tokens, state.where, true, ':');
  // So, for example, foo:0:"hi" will be split into {foo, 0, "hi"}
  // We take the first element as the variable name
  varName = tokens[0];
  // Then we check if the variable exists. If it does, we store its types in a
  // variable.
  vector<unsigned int> types;
  if (state.variables[state.current_subprocedure].count(varName) > 0)
    types = state.variables[state.current_subprocedure][varName];
  else if (state.variables[""].count(varName) > 0)
    types = state.variables[""][varName];
  // If the variable wasn't found, we return {0}
  else
    return {0};
  // If it was found, though, we want to get its current type.
  // If, in the example above, we had foo:0, then our current type would be {1,
  // 4} meaning, a NUMBER (1) MAP (4). But the type the variable has stored is
  // {1, 4, 3}, because it is a NUMBER MAP LIST (LIST is 3). We have to remove
  // all accessed elements from the type vector ({1, 4, 3} would turn into {1,
  // 4} because we accessed the list when we did :0). The number of elements to
  // pop from the vector is equal to the number of : found within the full
  // variable (foo:0). As we've already splitted the variable into tokens, the
  // number of elements to pop from the vector is equal to the number of tokens
  // we have minus one. If the container access should contain other container
  // accesses (for example foo:bar:0), the thing changes, and we must make sure
  // to discard those indexes that access the other containers and not the one
  // we are trying to get the types of.
  size_t tokensToSkip = 0;
  for (size_t i = 1; i < tokens.size(); ++i) {
    // If the current token is a scalar literal, we can skip it safely.
    if (is_number(tokens[i]) || is_string(tokens[i])) {
      if (tokensToSkip > 0) tokensToSkip--;
    }
    // If it's not, then it must be a variable name.
    else {
      // If the variable doesn't exist in the current context, we rise an error.
      if (state.variables[state.current_subprocedure].count(tokens[i]) == 0 &&
          state.variables[""].count(tokens[i]) == 0)
        error("The variable " + tokens[i] + " used in " + token +
              " doesn't exist.");
      vector<unsigned int> cvar_types = variable_type(tokens[i], state);
      if (cvar_types.size() > 1)
        // If the variable exists and is a container, then we skip as many
        // tokens as that variable takes.
        tokensToSkip += cvar_types.size() - 1;
      else
        // If the variable exists and is a scalar, we can skip it
        if (tokensToSkip > 0) tokensToSkip--;
    }
    if (tokensToSkip == 0) types.pop_back();
  }
  // We return {0} if there is an incomplete container access
  // that must be complete to resolve as a scalar index
  if (tokensToSkip > 0) return {0};
  // Now we have the types and can return them.
  return types;
}