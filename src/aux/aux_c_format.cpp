/* This file contains auxiliary functions that turn LDPL identifiers /
 * expressions into C / C++ identifiers / expressions */

// +---------------------------------------------+
// | TODO: comment and format this file properly |
// +---------------------------------------------+

// Given a full variable (with accesses and everything, like foo:0:'hi there' or
// bar) returns the C++ representation of said variable in order to be accessed.
string get_c_variable(compiler_state &state, string &variable) {
  // We want to get two things in order to create the correct C++ representation
  // of a 'full variable': the variable name and all the indexess we are trying
  // to access that correspond to this particular variable (because we could
  // have foo number list list and bar number list and try to access
  // foo:bar:0:1. In this case, we would be interested in getting 'foo', 'bar:0'
  // and '1'.
  string var_name;
  vector<string> indexes;
  // We use the split_vector function to get these values into our variables.
  split_vector(variable, var_name, indexes, state);
  // We 'fix' the variable name, turning all characters not accepted by C++ into
  // LDPL codes that C++ does accept.
  var_name = fix_identifier(var_name, true, state);
  // If split_vector didn't return any indexes, then we are dealing with a
  // scalar variable. We just return the C++ variable name and we are done.
  if (indexes.empty()) return var_name;
  // If our indexes vector is not empty, however, we recreate the correct C++
  // container access, with one dimension for each value in our indexes vector.
  for (size_t i = 0; i < indexes.size(); ++i)
    var_name += '[' + get_c_expression(state, indexes[i]) + ']';
  // Once we are done, we return the variable name.
  return var_name;
}

string get_c_expression(compiler_state &state, string &expression) {
  if (is_scalar_variable(expression, state) ||
      variable_exists(expression, state))
    return get_c_variable(state, expression);
  is_number(expression);  // We fix the expression should it be a number literal
  return expression;
}

// text must be a TEXT or a TEXT variable
string get_c_char_array(compiler_state &state, string &text) {
  if (is_txt_var(text, state))
    return get_c_variable(state, text) + ".str_rep().c_str()";
  return text;
}

string get_c_string(compiler_state &state, string &expression) {
  string c_expression = get_c_expression(state, expression);
  if (is_num_expr(expression, state))
    return "to_ldpl_string(" + c_expression + ")";
  return c_expression;
}

string get_c_number(compiler_state &state, string &expression) {
  string c_expression = get_c_expression(state, expression);
  if (is_txt_expr(expression, state)) return "to_number(" + c_expression + ")";
  return c_expression;
}

// Returns [ERROR] if invalid condtion, otherwise returns C++ condition
string get_c_condition(compiler_state &state, vector<string> tokens) {
  unsigned int ct = 0;
  string condition = get_c_condition(state, tokens, ct);
  if (ct < tokens.size()) return "[ERROR]";
  return condition;
}

#define MATCH(x)                             \
  if (ct < tokens.size() && tokens[ct] == x) \
    ct++;                                    \
  else                                       \
    return "[ERROR]";

string get_c_condition(compiler_state &state, vector<string> tokens,
                       unsigned int &ct) {
  if (ct >= tokens.size()) return "[ERROR]";
  string condition;
  if (tokens[ct] == "(") {
    MATCH("(");
    condition = get_c_condition(state, tokens, ct);
    if (condition == "[ERROR]") return condition;
    MATCH(")");
    condition = "(" + condition + ")";
  } else {
    string first_value = tokens[ct];
    string second_value;
    string rel_op;
    ct++;  // We validate the token after we get the second value
    if (tokens[ct] == "IS") {
      MATCH("IS");
      if (tokens[ct] == "EQUAL") {
        MATCH("EQUAL");
        MATCH("TO");
        rel_op = "EQUAL TO";
      } else if (tokens[ct] == "NOT") {
        MATCH("NOT");
        MATCH("EQUAL");
        MATCH("TO");
        rel_op = "NOT EQUAL TO";
      } else if (tokens[ct] == "GREATER") {
        MATCH("GREATER");
        MATCH("THAN");
        if (ct + 1 < tokens.size() && tokens[ct + 1] == "EQUAL") {
          // We check the next token instead of the curent one
          // because "OR" could be a variable after "GREATER THAN"
          MATCH("OR");
          MATCH("EQUAL");
          MATCH("TO");
          rel_op = "GREATER THAN OR EQUAL TO";
        } else {
          rel_op = "GREATER THAN";
        }
      } else if (tokens[ct] == "LESS") {
        MATCH("LESS");
        MATCH("THAN");
        if (ct + 1 < tokens.size() && tokens[ct + 1] == "EQUAL") {
          // We check the next token instead of the curent one
          // because "OR" could be a variable after "LESS THAN"
          MATCH("OR");
          MATCH("EQUAL");
          MATCH("TO");
          rel_op = "LESS THAN OR EQUAL TO";
        } else {
          rel_op = "LESS THAN";
        }
      } else {
        return "[ERROR]";
      }

      second_value = tokens[ct];
      ++ct;

      string type;
      if (is_num_expr(first_value, state) && is_num_expr(second_value, state))
        type = "NUMBER";
      else if (is_txt_expr(first_value, state) &&
               is_txt_expr(second_value, state))
        type = "TEXT";
      else if (is_num_map(first_value, state) &&
               is_num_map(second_value, state))
        type = "NUMBER MAP";
      else if (is_txt_map(first_value, state) &&
               is_txt_map(second_value, state))
        type = "TEXT MAP";
      else if (is_num_list(first_value, state) &&
               is_num_list(second_value, state))
        type = "NUMBER LIST";
      else if (is_txt_list(first_value, state) &&
               is_txt_list(second_value, state))
        type = "TEXT LIST";
      else if (is_list_list(first_value, state) &&
               is_list_list(second_value, state) &&
               variable_type(first_value, state) ==
                   variable_type(second_value, state))
        type = "LIST LIST";
      else if (is_map_map(first_value, state) &&
               is_map_map(second_value, state) &&
               variable_type(first_value, state) ==
                   variable_type(second_value, state))
        type = "MAP MAP";
      else
        return "[ERROR]";

      first_value = get_c_expression(state, first_value);
      second_value = get_c_expression(state, second_value);
      if (type == "NUMBER") {
        if (rel_op == "EQUAL TO")
          condition = "num_equal(" + first_value + ", " + second_value + ")";
        else if (rel_op == "NOT EQUAL TO")
          condition = "!num_equal(" + first_value + ", " + second_value + ")";
        else if (rel_op == "GREATER THAN")
          condition = first_value + " > " + second_value;
        else if (rel_op == "LESS THAN")
          condition = first_value + " < " + second_value;
        else if (rel_op == "GREATER THAN OR EQUAL TO")
          condition = "(" + first_value + " > " + second_value +
                      " || num_equal(" + first_value + ", " + second_value +
                      "))";
        else
          condition = "(" + first_value + " < " + second_value +
                      " || num_equal(" + first_value + ", " + second_value +
                      "))";
      } else if (type == "TEXT") {
        if (rel_op == "EQUAL TO")
          condition = "str_cmp(" + first_value + ", " + second_value + ") == 0";
        else if (rel_op == "NOT EQUAL TO")
          condition = "str_cmp(" + first_value + ", " + second_value + ") != 0";
        else if (rel_op == "GREATER THAN")
          condition = "str_cmp(" + first_value + ", " + second_value + ") > 0";
        else if (rel_op == "LESS THAN")
          condition = "str_cmp(" + first_value + ", " + second_value + ") < 0";
        else if (rel_op == "GREATER THAN OR EQUAL TO")
          condition = "str_cmp(" + first_value + ", " + second_value + ") >= 0";
        else if (rel_op == "LESS THAN OR EQUAL TO")
          condition = "str_cmp(" + first_value + ", " + second_value + ") <= 0";
        else
          return "[ERROR]";
      } else {
        first_value += ".inner_collection";
        second_value += ".inner_collection";
        if (rel_op == "EQUAL TO")
          condition = first_value + " == " + second_value;
        else if (rel_op == "NOT EQUAL TO")
          condition = first_value + " != " + second_value;
        else  // >, >, <= and >= are only valid in NUMBER and TEXT
          return "[ERROR]";
      }
    } else {
      if (tokens[ct] == "IN") {
        MATCH("IN");
        rel_op = "IN";
      } else if (tokens[ct] == "NOT") {
        MATCH("NOT");
        MATCH("IN");
        rel_op = "NOT IN";
      }

      second_value = tokens[ct];
      ++ct;
      string type;
      if (is_num_expr(first_value, state) && is_num_list(second_value, state))
        type = "NUM-IN-NUM-LIST";
      else if (is_num_expr(first_value, state) &&
               is_txt_list(second_value, state))
        type = "NUM-IN-TEXT-LIST";
      else if (is_txt_expr(first_value, state) &&
               is_num_list(second_value, state))
        type = "TEXT-IN-NUM-LIST";
      else if (is_txt_expr(first_value, state) &&
               is_txt_list(second_value, state))
        type = "TEXT-IN-TEXT-LIST";
      else if (is_num_expr(first_value, state) && is_map(second_value, state))
        type = "NUM-IN-MAP";
      else if (is_string(first_value) && is_map(second_value, state))
        type = "STR-IN-MAP";
      else if (is_txt_var(first_value, state) && is_map(second_value, state))
        type = "TEXTVAR-IN-MAP";
      else
        return "[ERROR]";

      first_value = get_c_expression(state, first_value);
      second_value = get_c_expression(state, second_value);
      if (type == "NUM-IN-NUM-LIST") {
        if (rel_op == "IN")
          condition = "find(" + second_value + ".inner_collection.begin(), " +
                      second_value + ".inner_collection.end(), (ldpl_number) " +
                      first_value + ") != " + second_value +
                      ".inner_collection.end()";
        else if (rel_op == "NOT IN")
          condition = "find(" + second_value + ".inner_collection.begin(), " +
                      second_value + ".inner_collection.end(), (ldpl_number) " +
                      first_value + ") == " + second_value +
                      ".inner_collection.end()";
      } else if (type == "TEXT-IN-NUM-LIST") {
        if (rel_op == "IN")
          condition = "find(" + second_value + ".inner_collection.begin(), " +
                      second_value + ".inner_collection.end(), to_number(" +
                      first_value + ")) != " + second_value +
                      ".inner_collection.end()";
        else if (rel_op == "NOT IN")
          condition = "find(" + second_value + ".inner_collection.begin(), " +
                      second_value + ".inner_collection.end(), to_number(" +
                      first_value + ")) == " + second_value +
                      ".inner_collection.end()";
      } else if (type == "NUM-IN-TEXT-LIST") {
        if (rel_op == "IN")
          condition = "find(" + second_value + ".inner_collection.begin(), " +
                      second_value +
                      ".inner_collection.end(), to_ldpl_string(" + first_value +
                      ")) != " + second_value + ".inner_collection.end()";
        else if (rel_op == "NOT IN")
          condition = "find(" + second_value + ".inner_collection.begin(), " +
                      second_value +
                      ".inner_collection.end(), to_ldpl_string(" + first_value +
                      ")) == " + second_value + ".inner_collection.end()";
      } else if (type == "TEXT-IN-TEXT-LIST") {
        if (rel_op == "IN")
          condition = "find(" + second_value + ".inner_collection.begin(), " +
                      second_value + ".inner_collection.end(), " + first_value +
                      ") != " + second_value + ".inner_collection.end()";
        else if (rel_op == "NOT IN")
          condition = "find(" + second_value + ".inner_collection.begin(), " +
                      second_value + ".inner_collection.end(), " + first_value +
                      ") == " + second_value + ".inner_collection.end()";
      } else if (type == "NUM-IN-MAP") {
        if (rel_op == "IN")
          condition = second_value + ".inner_collection.find(to_ldpl_string(" +
                      first_value + ").str_rep()) != " + second_value +
                      ".inner_collection.end()";
        else if (rel_op == "NOT IN")
          condition = second_value + ".inner_collection.find(to_ldpl_string(" +
                      first_value + ").str_rep()) == " + second_value +
                      ".inner_collection.end()";
      } else if (type == "STR-IN-MAP") {
        if (rel_op == "IN")
          condition = second_value + ".inner_collection.find(" + first_value +
                      ") != " + second_value + ".inner_collection.end()";
        else if (rel_op == "NOT IN")
          condition = second_value + ".inner_collection.find(" + first_value +
                      ") == " + second_value + ".inner_collection.end()";
      } else if (type == "TEXTVAR-IN-MAP") {
        if (rel_op == "IN")
          condition = second_value + ".inner_collection.find(" + first_value +
                      ".str_rep()) != " + second_value +
                      ".inner_collection.end()";
        else if (rel_op == "NOT IN")
          condition = second_value + ".inner_collection.find(" + first_value +
                      ".str_rep()) == " + second_value +
                      ".inner_collection.end()";
      } else
        return "[ERROR]";
    }

    if (ct < tokens.size() && (tokens[ct] == "AND" || tokens[ct] == "OR")) {
      if (tokens[ct] == "AND")
        condition += " && ";
      else if (tokens[ct] == "OR")
        condition += " || ";
      ++ct;
      string next_condition = get_c_condition(state, tokens, ct);
      if (next_condition == "[ERROR]") return next_condition;
      condition += next_condition;
    }
  }
  return condition;
}