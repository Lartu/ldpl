/* This file contains functions used to tokenize an LDPL source file */

// +---------------------------------------------+
// | TODO: comment and format this file properly |
// +---------------------------------------------+

// Tokenizes a line splitting by 'splitChar' with optional convertion of tokens
// to uppercase (except in strings)
void tokenize(string &line, vector<string> &tokens, code_location &where,
              bool uppercase, char splitChar)
{
  bool in_string = false;
  string current_token = "";
  // For each letter in the line
  for (unsigned int i = 0; i < line.size(); ++i)
  {
    char letter = line[i];
    if (letter == splitChar)
    {
      if (in_string)
        current_token += letter;
      else
      {
        if (current_token.size() > 0)
          tokens.push_back(current_token);
        current_token = "";
      }
    }
    else if (letter == '"')
    {
      in_string = !in_string;
      current_token += letter;
    }
    else if (letter == '\\' && in_string)
    {
      if (i < line.size() - 1)
      {
        char next_letter = line[++i];
        switch (next_letter)
        {
        case '\\':
        case '"':
        case '0':
        case 'a':
        case 'b':
        case 't':
        case 'n':
        case 'v':
        case 'f':
        case 'r':
        case 'e':
          current_token += "\\" + string(1, next_letter);
          break;
        default:
          badcode("unknown escape sequence", where);
        }
      }
      else
        badcode("\\ found alone on a string", where);
    }
    else if (letter == '#') // Comment character
    {
      if (in_string)
        current_token += letter;
      else
      {
        if (current_token.size() > 0)
          tokens.push_back(current_token);
        return;
      }
    }
    else
    {
      current_token += (uppercase && !in_string) ? toupper(letter) : letter;
    }
    if (i == line.size() - 1 && letter != splitChar)
    {
      if (in_string)
        badcode("Unterminated string", where);
      if (current_token.size() > 0)
        tokens.push_back(current_token);
    }
  }
}