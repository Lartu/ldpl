/* This file contains auxiliary functions that do text or number formatting */

void replace_whitespace(string &code) {
  // -- Replaces all whitespace within string --
  for (char &c : code) {
    if (isspace(c)) {
      c = ' ';
    }
  }
}

string fix_identifier(string ident, bool isVar, compiler_state &state) {
  // -- Replaces invalid C++ characters in internal and external identifiers --
  // when state is provided to fix_identifier it can fix external variables too.
  if (is_external(ident, state)) {
    return fix_external_identifier(ident, isVar);
  } else {
    return fix_identifier(ident, isVar);
  }
}

string fix_identifier(string identifier, bool isVariable) {
  // -- Appends a prefix based on its type to an identifier --
  return string(isVariable ? "VAR_" : "SUBPR_") + fix_identifier(identifier);
}

string fix_identifier(string identifier) {
  // -- Replaces invalid C++ characters in identifiers --
  const string validChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890:";
  ostringstream new_id;
  for (unsigned int i = 0; i < identifier.size(); ++i) {
    if (validChars.find(identifier[i]) != string::npos) {
      new_id << identifier[i];
    } else {
      new_id << "c" << (unsigned int)identifier[i] << "_";
    }
  }
  return new_id.str();
}

string fix_external_identifier(string identifier, bool isVariable) {
  // -- Replaces invalid C++ characters in external identifiers --
  // External identifiers are used by C++ extensions and thus have a simpler but
  // more restrictive name mangling algorithm: The only characters allowed are
  // `A-Z`, `0-9`, and `_`. All other characters are converted to `_`.
  const string validChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890:";
  string new_id;
  for (unsigned int i = 0; i < identifier.size(); ++i) {
    if (validChars.find(identifier[i]) != string::npos) {
      new_id += identifier[i];
    } else {
      new_id += "_";
    }
  }
  return new_id;
}

string &escape_c_quotes(string &str) {
  // -- Replaces all " in the passed string for \" --
  for (unsigned int i = 0; i < str.size(); ++i) {
    if (str[i] == '"') {
      str.erase(i, 1);
      str.insert(i, "\\\"");
      ++i;
    }
  }
  return str;
}

string expandHomeDirectory(string &filename) {
  // -- Expands a home directory (normaly when the character ~ is used) --
#if defined(_WIN32)
  return filename;
#else
  string homeDir = exec("echo $HOME");
  trim(homeDir);
  string newPath = "";
  for (size_t i = 0; i < filename.length(); ++i) {
    if (filename[i] == '~') {
      newPath += homeDir;
    } else {
      newPath += filename[i];
    }
  }
  return newPath;
#endif
}