/* This file contains auxiliary functions to split and access [multi]containers
 */

// +---------------------------------------------+
// | TODO: comment and format this file properly |
// +---------------------------------------------+

// Given a 'full variable' (for example 'bar' or 'foo:0:"hi there":4'), a place
// to store a variable name and a vector of strings to store all indexes, we get
// the variable name and all its subindexes (if any) into the corresponding
// variables.
void split_vector(string &token, string &var_name, vector<string> &indexes,
                  compiler_state &state) {
  // First of all we want to know if we are dealing with a container variable,
  // that is a LIST of something of a MAP of something. Thus, we look for any
  // ':' in the full variable and store the possition of the (possible) first
  // one.
  size_t pos = token.find(":");
  // If we didn't find any ':', then we are dealing with a scalar variable.
  // We just get its name (the full variable) and an empty vector of indexes.
  if (pos == string::npos) {
    var_name = token;
    indexes = {};
  }
  // If we found a ':', but it's the last character in the full variable, that
  // means that we received something like 'foo:'. That's not right, so we rise
  // an error.
  else if (pos == token.size() - 1)
    error("Incomplete MAP or LIST access found (can't end on ':').");
  else {
    // If none of the above happened, then our container is formatted correctly.
    // Now it's time to split it. As we can have something like foo IS NUMBER
    // LIST LIST bar IS NUMBER LIST and try to access foo using a value stored
    // in bar (for example foo:bar:0:1, meaning the foo[bar[0]][1]), we have to
    // know how many indexes our container expects. We tokenize our container
    // using the tokenize function.
    vector<string> tokens;
    tokenize(token, tokens, state.where, true, ':');
    // We take the first token as the variable name and then discard it.
    var_name = tokens[0];
    tokens.erase(tokens.begin(), tokens.begin() + 1);
    // For each of the remaining, we have to check if its a variable or not. If
    // its a variable name, it may be another container. For each container we
    // find, we append as many indexes that container requires to the previous
    // token we found (the index name). So, if we had foo and bar as stated
    // above, the parsing of foo:bar:0:1 would go like this:
    // - foo is taken as the variable name and thus skipped.
    // - bar is a container that takes one index. We'll then 'skip' one index.
    //   We then push bar to a list of tokens we've already checked.
    // - 1 is an index, and we said we were going to 'skip' one index. What we
    // do
    //   is, instead of pushing 1 to the list of tokens we've already checked,
    //   we append it to the last token we found as an index access. So we take
    //   the 'bar' that's already in our checked list and we turn it into
    //   'bar:1'.
    // - 0 is an index. We are not skipping anymore indexes as we already
    // skipped
    //   '1', so we push it into our checked tokens list.
    // Our checked tokens list will end up like {'bar:1', 0}, that are the
    // indexes of foo we are trying to access.
    vector<string> checked_tokens;
    size_t tokens_to_skip = 0;
    for (size_t i = 0; i < tokens.size(); ++i) {
      if (is_number(tokens[i]) || is_string(tokens[i])) {
        if (tokens_to_skip > 0) {
          checked_tokens.back() += ":" + tokens[i];
          tokens_to_skip--;
        } else {
          checked_tokens.push_back(tokens[i]);
        }
      }
      // If the current token is not a number nor a string, it must be a
      // variable name.
      else {
        // We get the types of the current variable.
        vector<unsigned int> cvar_types = variable_type(tokens[i], state);
        // If the variable doesn't exist, we raise an error.
        if (cvar_types == vector<unsigned int>{0}) {
          error("The variable " + tokens[i] + " doesn't exist in " + token +
                ".");
        }
        // We remove an element of the current variable types as the main type
        // (NUMBER or TEXT) never names a container type and thus cannot be
        // indexed.
        cvar_types.pop_back();
        // If we had to skip this token, we append it to the last token we have.
        if (tokens_to_skip > 0) {
          checked_tokens.back() += ":" + tokens[i];
          tokens_to_skip--;
        } else {
          checked_tokens.push_back(tokens[i]);
        }
        // We tell the splitter to skip this many tokens.
        tokens_to_skip += cvar_types.size();
      }
    }
    // After we've got all indexes we needed, we store our checked tokens list
    // into the indexes vector, that is what we are going to return with the
    // variable name.
    indexes = checked_tokens;
  }
}