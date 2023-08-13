/* This file contains auxiliary functions to check data type compliance */

bool is_number(string &number) {
  // -- Checks if a string is a valid real number --
  unsigned int firstchar = 0;
  if (number[0] == '-') firstchar = 1;
  if (number[firstchar] == '.')
    return false;  // .12 is not a valid decimal in LDPL, 0.12 is.
  if (number[firstchar] == '+')
    return false;  // +5 is not a valid decimal in LDPL, 5 is.
  istringstream iss(number);
  double f;
  iss >> f;
  bool isNumber = iss.eof() && !iss.fail();
  // If it is a number, it might be an octal literal (e.g. 025, 067, 003 etc)
  // so we proceed to fix the original number to make it decimal.
  if (isNumber) {
    string f_number = "";
    unsigned int i;
    for (i = 1; i < number.length(); ++i) {
      if (number[i - 1] != '0') {
        // If prev char not 0
        if (number[i - 1] == '-')
          // if prev char is -, continue check
          f_number += '-';
        else
          // if prev char is number, break
          break;
      } else if (number[i] == '.')
        // If prev number is 0
        f_number += '0';
    }
    f_number += number.substr(i - 1);
    number = f_number;
    return true;
  } else
    return false;
}

bool is_natural(string number) {
  // -- Checks if a string is a valid natural number --
  if (!is_number(number)) return false;
  if (stod(number) <= 0) return false;
  for (char l : number)
    if (l == '.') return false;
  return true;
}

bool is_label(string &token) {
  // -- Checks if a string is a valid label --
  for (char letter : token)
    if (letter == '\"') return false;
  return true;
}

bool is_math_symbol(string &token) {
  // -- Checks if a string is a valid math symbol --
  string syms = "+-*/()";
  return token.size() == 1 && syms.find(token[0]) != string::npos;
}

bool is_string(string &token) {
  // -- Checks if a string is a valid LDPL string --
  if (token.size() < 2 || token[0] != '"' || token[token.size() - 1] != '"')
    return false;
  // Check for unescaped quotes
  for (unsigned int i = 1; i < token.size() - 1; ++i) {
    if (token[i] == '\"' && token[i - 1] != '\\') return false;
  }
  return true;
}