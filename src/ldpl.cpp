#include "ldpl.h"

int main(int argc, const char* argv[])
{
    //Get command line arguments as string vector
    vector<string> args(argv + 1, argv + argc);
    //Fail if file was not passed
    if(args.size() == 0) error("Filename expected.");
    //For each file
    for(string & filename : args)
    {
        //Load file
        ifstream file(filename);
        //Fail if the file couldn't be loaded
        if(!file.is_open()) error("The file couldn't be opened.");
        //Get file contents
        vector<string> lines;
        string line = "";
        while(getline(file, line)) lines.push_back(line);
        compile(lines);
    }
}

//Shows error message and exits
void error(const string & msg)
{
    cerr << "\033[1;31mLDPL Error: ";
    cerr << msg;
    cerr << "\033[0m" << endl;
    exit(1);
}

//Compilation process
void compile(vector<string> & lines)
{
    //Compiler state (holds variables, sections, functions, etc)
    compiler_state state;
    //For each line in the source code
    for(uint line_num = 0; line_num < lines.size(); ++line_num)
    {
        string & line = lines[line_num];
        trim(line);
        //Split line in various tokens
        vector<string> tokens;
        tokenize(line, line_num, tokens);
        if(tokens.size() == 0) continue;
        //TODO: pasar tokens que no sean strings a uppercase
        compile_line(tokens, line_num, state);
    }
}

//Tokenizes a line
void tokenize(string & line, uint line_num, vector<string> & tokens)
{
    bool in_string = false;
    string current_token = "";
    //For each letter in the line
    for(uint i = 0; i < line.size(); ++i)
    {
        char letter = line[i];
        if(letter == ' ')
        {
            if(in_string) current_token += letter;
            else
            {
                if(current_token.size() > 0)
                    tokens.push_back(current_token);
                current_token = "";
            }
        }
        else if(letter == '"')
        {
            in_string = !in_string;
            current_token += letter;
        }
        else if(letter == '\\')
        {
            if(i < line.size() - 1)
            {
                char next_letter = line[++i];
                if(next_letter == '\\' || next_letter == '"') current_token += next_letter;
                else error("unknown escape sequence on line " + to_string(line_num) + ".");
            }
            else error("\\ found alone on line " + to_string(line_num) + ".");
        }
        else if(letter == '#') //Comment character
        {
            if(in_string) current_token += letter;
            else
            {
                if(current_token.size() > 0)
                    tokens.push_back(current_token);
                return;
            }
        }
        else
        {
            current_token += letter;
        }
        if(i == line.size() - 1 && letter != ' ')
        {
            if(in_string) error("Unterminated string on line " + to_string(line_num) + ".");
            if(current_token.size() > 0)
                    tokens.push_back(current_token);
        }
    }
}

//Compiles line per line
void compile_line(vector<string> & tokens, uint line_num, compiler_state & state)
{
    ++line_num;
    if(line_like("DATA:", tokens, state))
    {
        if(state.section_state == 1)
            error("Duplicate DATA section declaration on line " + to_string(line_num) + ".");
        if(state.section_state == 2)
            error("DATA section declaration within PROCEDURE section on line " + to_string(line_num) + ".");
        state.section_state = 1;
        return;
    }
    if(line_like("PROCEDURE:", tokens, state))
    {
        if(state.section_state == 2)
            error("Duplicate SECTION section declaration on line " + to_string(line_num) + ".");
        state.section_state = 2;
        return;
    }
    if(line_like("$name IS NUMBER", tokens, state))
    {
        if(state.section_state != 1)
            error("Variable declaration outside DATA section on line " + to_string(line_num) + ".");
        if(!variable_exists(tokens[0], state))
            state.variables.push_back(make_pair(tokens[0], 1));
        else
            error("Duplicate declaration for variable " + tokens[0] + " on line " + to_string(line_num) + ".");
        cout << "-> Declaración de variable NUMBER " << tokens[0] << endl; //TODO
        return;
    }
    if(line_like("$name IS TEXT", tokens, state))
    {
        if(state.section_state != 1)
            error("Variable declaration outside DATA section on line " + to_string(line_num) + ".");
        if(!variable_exists(tokens[0], state))
            state.variables.push_back(make_pair(tokens[0], 2));
        else
            error("Duplicate declaration for variable " + tokens[0] + " on line " + to_string(line_num) + ".");
        cout << "-> Declaración de variable TEXT " << tokens[0] << endl; //TODO
        return;
    }
    if(line_like("$name IS NUMBER VECTOR", tokens, state))
    {
        if(state.section_state != 1)
            error("Variable declaration outside DATA section on line " + to_string(line_num) + ".");
        if(!variable_exists(tokens[0], state))
            state.variables.push_back(make_pair(tokens[0], 3));
        else
            error("Duplicate declaration for variable " + tokens[0] + " on line " + to_string(line_num) + ".");
        cout << "-> Declaración de variable NUMBER VECTOR " << tokens[0] << endl; //TODO
        return;
    }
    if(line_like("$name IS TEXT VECTOR", tokens, state))
    {
        if(state.section_state != 1)
            error("Variable declaration outside DATA section on line " + to_string(line_num) + ".");
        if(!variable_exists(tokens[0], state))
            state.variables.push_back(make_pair(tokens[0], 4));
        else
            error("Duplicate declaration for variable " + tokens[0] + " on line " + to_string(line_num) + ".");
        cout << "-> Declaración de variable TEXT VECTOR " << tokens[0] << endl; //TODO
        return;
    }
    if(line_like("DISPLAY $display", tokens, state))
    {
        if(state.section_state != 2)
            error("DISPLAY statement outside PROCEDURE section on line " + to_string(line_num) + ".");
        cout << "-> DISPLAY" << endl; //TODO
        return;
    }
    if(line_like("ACCEPT $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("ACCEPT statement outside PROCEDURE section on line " + to_string(line_num) + ".");
        cout << "-> ACCEPT number variable" << endl; //TODO
        return;
    }
    if(line_like("ACCEPT $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("ACCEPT statement outside PROCEDURE section on line " + to_string(line_num) + ".");
        cout << "-> ACCEPT string variable" << endl; //TODO
        return;
    }
    if(line_like("STORE $num-var IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("STORE statement outside PROCEDURE section on line " + to_string(line_num) + ".");
        cout << "-> STORE num var in num var" << endl; //TODO
        return;
    }
    if(line_like("STORE $number IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("STORE statement outside PROCEDURE section on line " + to_string(line_num) + ".");
        cout << "-> STORE num in num var" << endl; //TODO
        return;
    }
    if(line_like("STORE $str-var IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("STORE statement outside PROCEDURE section on line " + to_string(line_num) + ".");
        cout << "-> STORE txt var in txt var" << endl; //TODO
        return;
    }
    if(line_like("STORE $string IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("STORE statement outside PROCEDURE section on line " + to_string(line_num) + ".");
        cout << "-> STORE txt in txt var" << endl; //TODO
        return;
    }
    if(line_like("ADD $number AND $number IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("ADD statement outside PROCEDURE section on line " + to_string(line_num) + ".");
        cout << "-> ADD" << endl; //TODO
        return;
    }
    if(line_like("ADD $number AND $num-var IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("ADD statement outside PROCEDURE section on line " + to_string(line_num) + ".");
        cout << "-> ADD" << endl; //TODO
        return;
    }
    if(line_like("ADD $num-var AND $number IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("ADD statement outside PROCEDURE section on line " + to_string(line_num) + ".");
        cout << "-> ADD" << endl; //TODO
        return;
    }
    if(line_like("ADD $num-var AND $num-var IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("ADD statement outside PROCEDURE section on line " + to_string(line_num) + ".");
        cout << "-> ADD" << endl; //TODO
        return;
    }
    if(line_like("SUBTRACT $number FROM $number IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("SUBTRACT statement outside PROCEDURE section on line " + to_string(line_num) + ".");
        cout << "-> SUBTRACT" << endl; //TODO
        return;
    }
    if(line_like("SUBTRACT $number FROM $num-var IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("SUBTRACT statement outside PROCEDURE section on line " + to_string(line_num) + ".");
        cout << "-> SUBTRACT" << endl; //TODO
        return;
    }
    if(line_like("SUBTRACT $num-var FROM $number IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("SUBTRACT statement outside PROCEDURE section on line " + to_string(line_num) + ".");
        cout << "-> SUBTRACT" << endl; //TODO
        return;
    }
    if(line_like("SUBTRACT $num-var FROM $num-var IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("SUBTRACT statement outside PROCEDURE section on line " + to_string(line_num) + ".");
        cout << "-> SUBTRACT" << endl; //TODO
        return;
    }
    if(line_like("DIVIDE $number BY $number IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("DIVIDE statement outside PROCEDURE section on line " + to_string(line_num) + ".");
        cout << "-> DIVIDE" << endl; //TODO
        return;
    }
    if(line_like("DIVIDE $number BY $num-var IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("DIVIDE statement outside PROCEDURE section on line " + to_string(line_num) + ".");
        cout << "-> DIVIDE" << endl; //TODO
        return;
    }
    if(line_like("DIVIDE $num-var BY $number IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("DIVIDE statement outside PROCEDURE section on line " + to_string(line_num) + ".");
        cout << "-> DIVIDE" << endl; //TODO
        return;
    }
    if(line_like("DIVIDE $num-var BY $num-var IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("DIVIDE statement outside PROCEDURE section on line " + to_string(line_num) + ".");
        cout << "-> DIVIDE" << endl; //TODO
        return;
    }
    if(line_like("MULTIPLY $number BY $number IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("MULTIPLY statement outside PROCEDURE section on line " + to_string(line_num) + ".");
        cout << "-> MULTIPLY" << endl; //TODO
        return;
    }
    if(line_like("MULTIPLY $number BY $num-var IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("MULTIPLY statement outside PROCEDURE section on line " + to_string(line_num) + ".");
        cout << "-> MULTIPLY" << endl; //TODO
        return;
    }
    if(line_like("MULTIPLY $num-var BY $number IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("MULTIPLY statement outside PROCEDURE section on line " + to_string(line_num) + ".");
        cout << "-> MULTIPLY" << endl; //TODO
        return;
    }
    if(line_like("MULTIPLY $num-var BY $num-var IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("MULTIPLY statement outside PROCEDURE section on line " + to_string(line_num) + ".");
        cout << "-> MULTIPLY" << endl; //TODO
        return;
    }
    if(line_like("MODULO $number BY $number IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("MODULO statement outside PROCEDURE section on line " + to_string(line_num) + ".");
        cout << "-> MODULO" << endl; //TODO
        return;
    }
    if(line_like("MODULO $number BY $num-var IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("MODULO statement outside PROCEDURE section on line " + to_string(line_num) + ".");
        cout << "-> MODULO" << endl; //TODO
        return;
    }
    if(line_like("MODULO $num-var BY $number IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("MODULO statement outside PROCEDURE section on line " + to_string(line_num) + ".");
        cout << "-> MODULO" << endl; //TODO
        return;
    }
    if(line_like("MODULO $num-var BY $num-var IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("MODULO statement outside PROCEDURE section on line " + to_string(line_num) + ".");
        cout << "-> MODULO" << endl; //TODO
        return;
    }
    if(line_like("ABS $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("ABS statement outside PROCEDURE section on line " + to_string(line_num) + ".");
        cout << "-> ABS" << endl; //TODO
        return;
    }
    if(line_like("JOIN $string AND $string IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("JOIN statement outside PROCEDURE section on line " + to_string(line_num) + ".");
        cout << "-> JOIN" << endl; //TODO
        return;
    }
    if(line_like("JOIN $string AND $number IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("JOIN statement outside PROCEDURE section on line " + to_string(line_num) + ".");
        cout << "-> JOIN" << endl; //TODO
        return;
    }
    if(line_like("JOIN $string AND $num-var IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("JOIN statement outside PROCEDURE section on line " + to_string(line_num) + ".");
        cout << "-> JOIN" << endl; //TODO
        return;
    }
    if(line_like("JOIN $string AND $str-var IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("JOIN statement outside PROCEDURE section on line " + to_string(line_num) + ".");
        cout << "-> JOIN" << endl; //TODO
        return;
    }
    
    if(line_like("JOIN $number AND $string IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("JOIN statement outside PROCEDURE section on line " + to_string(line_num) + ".");
        cout << "-> JOIN" << endl; //TODO
        return;
    }
    if(line_like("JOIN $number AND $number IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("JOIN statement outside PROCEDURE section on line " + to_string(line_num) + ".");
        cout << "-> JOIN" << endl; //TODO
        return;
    }
    if(line_like("JOIN $number AND $num-var IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("JOIN statement outside PROCEDURE section on line " + to_string(line_num) + ".");
        cout << "-> JOIN" << endl; //TODO
        return;
    }
    if(line_like("JOIN $number AND $str-var IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("JOIN statement outside PROCEDURE section on line " + to_string(line_num) + ".");
        cout << "-> JOIN" << endl; //TODO
        return;
    }
    
    if(line_like("JOIN $str-var AND $string IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("JOIN statement outside PROCEDURE section on line " + to_string(line_num) + ".");
        cout << "-> JOIN" << endl; //TODO
        return;
    }
    if(line_like("JOIN $str-var AND $number IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("JOIN statement outside PROCEDURE section on line " + to_string(line_num) + ".");
        cout << "-> JOIN" << endl; //TODO
        return;
    }
    if(line_like("JOIN $str-var AND $num-var IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("JOIN statement outside PROCEDURE section on line " + to_string(line_num) + ".");
        cout << "-> JOIN" << endl; //TODO
        return;
    }
    if(line_like("JOIN $str-var AND $str-var IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("JOIN statement outside PROCEDURE section on line " + to_string(line_num) + ".");
        cout << "-> JOIN" << endl; //TODO
        return;
    }
    
    if(line_like("JOIN $num-var AND $string IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("JOIN statement outside PROCEDURE section on line " + to_string(line_num) + ".");
        cout << "-> JOIN" << endl; //TODO
        return;
    }
    if(line_like("JOIN $num-var AND $number IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("JOIN statement outside PROCEDURE section on line " + to_string(line_num) + ".");
        cout << "-> JOIN" << endl; //TODO
        return;
    }
    if(line_like("JOIN $num-var AND $num-var IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("JOIN statement outside PROCEDURE section on line " + to_string(line_num) + ".");
        cout << "-> JOIN" << endl; //TODO
        return;
    }
    if(line_like("JOIN $num-var AND $str-var IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("JOIN statement outside PROCEDURE section on line " + to_string(line_num) + ".");
        cout << "-> JOIN" << endl; //TODO
        return;
    }
    
    error("Malformed statement on line " + to_string(line_num) + ".");
}

// while | if | modulo | subroutine declaration | subroutine-call | get-char | sys-exec

//Check if the tokens of a line passed are like the ones of a model line
bool line_like(string model_line, vector<string> tokens, compiler_state & state)
{
    //Tokenize model line
    vector<string> model_tokens;
    tokenize(model_line, 0, model_tokens);
    //Check that tokens match between line and model line
    if(tokens.size() < model_tokens.size()) return false;
    uint i = 0;
    for(; i < model_tokens.size(); ++i)
    {
        if(model_tokens[i] == "$name") //$name is any word that is not a string or a number
        {
            for(char letter : model_tokens[i]) if(letter == ':') return false;
            if(is_string(tokens[i])) return false;
            if(is_number(tokens[i])) return false;
        }
        else if(model_tokens[i] == "$num-var") //$num-var is NUMBER variable
        {
            if(!is_num_var(tokens[i], state)) return false;
        }
        else if(model_tokens[i] == "$str-var") //$str-var is TEXT variable
        {
            if(!is_txt_var(tokens[i], state)) return false;
        }
        else if(model_tokens[i] == "$string") //$string is a string (?
        {
            if(!is_string(tokens[i])) return false;
        }
        else if(model_tokens[i] == "$number") //$number is a number (?
        {
            if(!is_number(tokens[i])) return false;
        }
        else if(model_tokens[i] == "$display") //multiple NUMBER, TEXT, TEXT-VAR, NUMBER-VAR or CRLF
        {
            for(; i < tokens.size(); ++i){
                if(!is_string(tokens[i])
                && !is_number(tokens[i])
                && !is_variable(tokens[i], state)
                && tokens[i] != "CRLF")
                    return false;
            }
        }
        else if(model_tokens[i] != tokens[i]) return false;
    }
    if(i < tokens.size()) return false;
    return true;
}

bool is_number(string number){
  try {
    stod(number);
  }
  catch (const std::invalid_argument& ia) {
	  return false;
  }
  return true;
}

bool is_string(string & token){
    return (token.size() >= 2 && token[0] == '"' && token[token.size()-1] == '"');
}

bool is_num_var(string & token, compiler_state & state)
{
    for(pair<string, uint> & var : state.variables)
        if(var.first == token && var.second == 1) return true;
    return false;
}

bool is_txt_var(string & token, compiler_state & state)
{
    for(pair<string, uint> & var : state.variables)
        if(var.first == token && var.second == 2) return true;
    return false;
}

bool is_variable(string & token, compiler_state & state)
{
    //TODO agregar que vector:1 también sea una variable
    //(Debería estar en is_num_var e is_txt_var)
    return is_num_var(token, state) || is_txt_var(token, state);
}

/*La diferencia entre is_variable y variable_exists es que is_variable
 no toma los vectores como una variable, sino los índices de los vectores.
 Variable exists te dice si una variable está declarada como tal, por lo que
 un subíndice de vector no sería una variable.*/
bool variable_exists(string & token, compiler_state & state)
{
    for(pair<string, uint> & var : state.variables)
        if(var.first == token) return true;
    return false;
}
