//TODO:
//Documentar stdlib
//Hacer que si recibe - por parémtro ejecute el código que recibe por stdin

#include "ldpl.h"

//Show internal representation
bool show_ir = false;

int main(int argc, const char* argv[])
{
    //Get command line arguments as string vector
    vector<string> args(argv + 1, argv + argc);
    if(args.size() == 1 && args[0] == "-v"){
        cout << "This is " << "\033[35;1mLDPL " << STANDARD << " version " << VERSION << "\033[0m" << endl << endl;
        cout << "Copyright 2018-2019, Martín del Río" << endl << endl;
        cout << "Standard and documentation can be found on \033[36;1mldpl.lartu.net\033[0m." << endl;
        cout << "Source code can be found at \033[36;1mhttps://github.com/lartu/ldpl\033[0m." << endl << endl;
        cout << "LDPL may be copied only under the terms of the GNU General Public License 3.0." << endl <<endl;
        cout << "Compiled on " << COMPILEDATE << endl;
        return 0;
    }
    //Check arguments
    if(args.size() > 1){
        for(string arg : args){
            if(arg == "-r"){
                show_ir = true;
            }
        }
    }
    //Fail if file was not passed
    if(args.size() == 0) error("Filename expected.");
    //For each file, compile each file into one big code
	compiler_state state; //Compiler state (holds variables, sections, functions, etc)
    for(string & filename : args)
    {
        //If it's an argument
        if(filename[0] == '-') continue;
		//Reset state section for this file
		state.section_state = 0;
		state.current_file = filename;
        load_and_compile(filename, state);
    }
	//Run the compiled code
	nvm(state.output_code);
}

void load_and_compile(string & filename, compiler_state & state)
{
    //Load file
    ifstream file(filename);
    //Fail if the file couldn't be loaded
    if(!file.is_open()) error("The file '" + filename + "' couldn't be opened.");
    //Get file contents
    vector<string> lines;
    string line = "";
    while(getline(file, line))
	{
		replace_whitespace(line);
		lines.push_back(line);
	}
    compile(lines, state);
}

//Replace all whitespace within string
void replace_whitespace(string & code)
{
	for(char & c : code){
		if(isspace(c)){
			c = ' ';
		}
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
void compile(vector<string> & lines, compiler_state & state)
{
    //For each line in the source code
    for(unsigned int line_num = 0; line_num < lines.size(); ++line_num)
    {
        string & line = lines[line_num];
        trim(line);
        //Split line in various tokens
        vector<string> tokens;
        tokenize(line, line_num, tokens, state.current_file);
        capitalize_tokens(tokens);
        if(tokens.size() == 0) continue;
        //TODO: pasar tokens que no sean strings a uppercase
        compile_line(tokens, line_num, state);
    }
    if(show_ir){
		cout << "\033[35;1mLDPL - Showing internal representation code:\033[0m" << endl;
		for(string line : state.output_code) cout << line << endl;
		exit(0);
	}
    //TODO: si llega acá y hay ifs sin cerrar o procedures sin cerrar, te comés puteada
}

//Tokenizes a line
void tokenize(string & line, unsigned int line_num, vector<string> & tokens, string & current_file)
{
    bool in_string = false;
    string current_token = "";
    //For each letter in the line
    for(unsigned int i = 0; i < line.size(); ++i)
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
                else error("unknown escape sequence (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
            }
            else error("\\ found alone (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
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
            if(in_string) error("Unterminated string (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
            if(current_token.size() > 0)
                    tokens.push_back(current_token);
        }
    }
}

//Tokens to upper case
void capitalize_tokens(vector<string> & tokens)
{
    for(string & token : tokens)
    {
            if(!is_number(token) && !is_string(token))
            {
                for(char & l : token){
                    l = toupper(l);
                }
            }
    }
}

//Compiles line per line
void compile_line(vector<string> & tokens, unsigned int line_num, compiler_state & state)
{
	string & current_file = state.current_file;
    ++line_num;
    if(line_like("DATA:", tokens, state))
    {
        if(state.section_state == 1)
            error("Duplicate DATA section declaration (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(state.section_state == 2)
            error("DATA section declaration within PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        state.section_state = 1;
        return;
    }
    if(line_like("PROCEDURE:", tokens, state))
    {
        if(state.section_state == 2)
            error("Duplicate SECTION section declaration (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        state.section_state = 2;
        return;
    }
    if(line_like("$name IS NUMBER", tokens, state))
    {
        if(state.section_state != 1)
            error("Variable declaration outside DATA section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(!variable_exists(tokens[0], state))
            state.variables.push_back(make_pair(tokens[0], 1));
        else
            error("Duplicate declaration for variable " + tokens[0] + " (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        state.add_code("0");
        set_var_value(state, tokens[0]);
        return;
    }
    if(line_like("$name IS TEXT", tokens, state))
    {
        if(state.section_state != 1)
            error("Variable declaration outside DATA section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(!variable_exists(tokens[0], state))
            state.variables.push_back(make_pair(tokens[0], 2));
        else
            error("Duplicate declaration for variable " + tokens[0] + " (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        state.add_code("\"\"");
        set_var_value(state, tokens[0]);
        return;
    }
    if(line_like("$name IS NUMBER VECTOR", tokens, state))
    {
        if(state.section_state != 1)
            error("Variable declaration outside DATA section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(!variable_exists(tokens[0], state)){
            state.variables.push_back(make_pair(tokens[0], 3));
        }
        else
            error("Duplicate declaration for variable " + tokens[0] + " (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        return;
    }
    if(line_like("$name IS TEXT VECTOR", tokens, state))
    {
        if(state.section_state != 1)
            error("Variable declaration outside DATA section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(!variable_exists(tokens[0], state)){
            state.variables.push_back(make_pair(tokens[0], 4));
        }
        else
            error("Duplicate declaration for variable " + tokens[0] + " (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        return;
    }
    if(line_like("DISPLAY $display", tokens, state))
    {
        if(state.section_state != 2)
            error("DISPLAY statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        for(unsigned int i = 1; i < tokens.size(); ++i){
            if(tokens[i] == "CRLF"){
                state.add_code("\"\"");
                state.add_code("PRINTLN");
            }else if(is_variable(tokens[i], state)){
                get_var_value(state, tokens[i]);
                state.add_code("PRINT");
            }
            else{
                state.add_code(tokens[i]);
                state.add_code("PRINT");
            }
        }
        return;
    }
    if(line_like("ACCEPT $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("ACCEPT statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        state.add_code("INPUT-NUM");
        set_var_value(state, tokens[1]);
        return;
    }
    if(line_like("ACCEPT $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("ACCEPT statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        state.add_code("INPUT");
        set_var_value(state, tokens[1]);
        return;
    }
    if(line_like("ACCEPT $str-var UNTIL EOF", tokens, state))
    {
        if(state.section_state != 2)
            error("ACCEPT statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        state.add_code("INPUT-FULL");
        set_var_value(state, tokens[1]);
        return;
    }
    if(line_like("STORE $num-var IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("STORE statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        get_var_value(state, tokens[1]);
        set_var_value(state, tokens[3]);
        return;
    }
    if(line_like("STORE $number IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("STORE statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        state.add_code(tokens[1]);
        set_var_value(state, tokens[3]);
        return;
    }
    if(line_like("STORE $str-var IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("STORE statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        get_var_value(state, tokens[1]);
        set_var_value(state, tokens[3]);
        return;
    }
    if(line_like("STORE $string IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("STORE statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        state.add_code(tokens[1]);
        set_var_value(state, tokens[3]);
        return;
    }
    if(line_like("ADD $number AND $number IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("ADD statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        state.add_code(tokens[1]);
        state.add_code(tokens[3]);
        state.add_code("+");
        set_var_value(state, tokens[5]);
        return;
    }
    if(line_like("ADD $number AND $num-var IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("ADD statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        state.add_code(tokens[1]);
        get_var_value(state, tokens[3]);
        state.add_code("+");
        set_var_value(state, tokens[5]);
        return;
    }
    if(line_like("ADD $num-var AND $number IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("ADD statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        get_var_value(state, tokens[1]);
        state.add_code(tokens[3]);
        state.add_code("+");
        set_var_value(state, tokens[5]);
        return;
    }
    if(line_like("ADD $num-var AND $num-var IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("ADD statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        get_var_value(state, tokens[1]);
        get_var_value(state, tokens[3]);
        state.add_code("+");
        set_var_value(state, tokens[5]);
        return;
    }
    if(line_like("SUBTRACT $number FROM $number IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("SUBTRACT statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        state.add_code(tokens[3]);
        state.add_code(tokens[1]);
        state.add_code("-");
        set_var_value(state, tokens[5]);
        return;
    }
    if(line_like("SUBTRACT $number FROM $num-var IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("SUBTRACT statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        get_var_value(state, tokens[3]);
        state.add_code(tokens[1]);
        state.add_code("-");
        set_var_value(state, tokens[5]);
        return;
    }
    if(line_like("SUBTRACT $num-var FROM $number IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("SUBTRACT statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        state.add_code(tokens[3]);
        get_var_value(state, tokens[1]);
        state.add_code("-");
        set_var_value(state, tokens[5]);
        return;
    }
    if(line_like("SUBTRACT $num-var FROM $num-var IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("SUBTRACT statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        get_var_value(state, tokens[3]);
        get_var_value(state, tokens[1]);
        state.add_code("-");
        set_var_value(state, tokens[5]);
        return;
    }
    if(line_like("DIVIDE $number BY $number IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("DIVIDE statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        state.add_code(tokens[1]);
        state.add_code(tokens[3]);
        state.add_code("/");
        set_var_value(state, tokens[5]);
        return;
    }
    if(line_like("DIVIDE $number BY $num-var IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("DIVIDE statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        state.add_code(tokens[1]);
        get_var_value(state, tokens[3]);
        state.add_code("/");
        set_var_value(state, tokens[5]);
        return;
    }
    if(line_like("DIVIDE $num-var BY $number IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("DIVIDE statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        get_var_value(state, tokens[1]);
        state.add_code(tokens[3]);
        state.add_code("/");
        set_var_value(state, tokens[5]);
        return;
    }
    if(line_like("DIVIDE $num-var BY $num-var IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("DIVIDE statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        get_var_value(state, tokens[1]);
        get_var_value(state, tokens[3]);
        state.add_code("/");
        set_var_value(state, tokens[5]);
        return;
    }
    if(line_like("MULTIPLY $number BY $number IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("MULTIPLY statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        state.add_code(tokens[1]);
        state.add_code(tokens[3]);
        state.add_code("*");
        set_var_value(state, tokens[5]); 
        return;
    }
    if(line_like("MULTIPLY $number BY $num-var IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("MULTIPLY statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        state.add_code(tokens[1]);
        get_var_value(state, tokens[3]);
        state.add_code("*");
        set_var_value(state, tokens[5]);
        return;
    }
    if(line_like("MULTIPLY $num-var BY $number IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("MULTIPLY statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        get_var_value(state, tokens[1]);
        state.add_code(tokens[3]);
        state.add_code("*");
        set_var_value(state, tokens[5]);
        return;
    }
    if(line_like("MULTIPLY $num-var BY $num-var IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("MULTIPLY statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        get_var_value(state, tokens[1]);
        get_var_value(state, tokens[3]);
        state.add_code("*");
        set_var_value(state, tokens[5]);
        return;
    }
    if(line_like("MODULO $number BY $number IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("MODULO statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        state.add_code(tokens[1]);
        state.add_code(tokens[3]);
        state.add_code("%");
        set_var_value(state, tokens[5]);
        return;
    }
    if(line_like("MODULO $number BY $num-var IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("MODULO statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        state.add_code(tokens[1]);
        get_var_value(state, tokens[3]);
        state.add_code("%");
        set_var_value(state, tokens[5]);
        return;
    }
    if(line_like("MODULO $num-var BY $number IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("MODULO statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        get_var_value(state, tokens[1]);
        state.add_code(tokens[3]);
        state.add_code("%");
        set_var_value(state, tokens[5]);
        return;
    }
    if(line_like("MODULO $num-var BY $num-var IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("MODULO statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        get_var_value(state, tokens[1]);
        get_var_value(state, tokens[3]);
        state.add_code("%");
        set_var_value(state, tokens[5]);
        return;
    }
    if(line_like("ABS $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("ABS statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        get_var_value(state, tokens[1]);
        state.add_code("ABS");
        set_var_value(state, tokens[1]);
        return;
    }
    if(line_like("JOIN $string AND $string IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("JOIN statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        state.add_code(tokens[1]);
        state.add_code(tokens[3]);
        state.add_code("JOIN");
        set_var_value(state, tokens[5]);
        return;
    }
    if(line_like("JOIN $string AND $number IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("JOIN statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        state.add_code(tokens[1]);
        state.add_code(tokens[3]);
        state.add_code("TO-STR");
        state.add_code("JOIN");
        set_var_value(state, tokens[5]);
        return;
    }
    if(line_like("JOIN $string AND $num-var IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("JOIN statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        state.add_code(tokens[1]);
        get_var_value(state, tokens[3]);
        state.add_code("TO-STR");
        state.add_code("JOIN");
        set_var_value(state, tokens[5]);
        return;
    }
    if(line_like("JOIN $string AND $str-var IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("JOIN statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        state.add_code(tokens[1]);
        get_var_value(state, tokens[3]);
        state.add_code("JOIN");
        set_var_value(state, tokens[5]);
        return;
    }
    
    if(line_like("JOIN $number AND $string IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("JOIN statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        state.add_code(tokens[1]);
        state.add_code("TO-STR");
        state.add_code(tokens[3]);
        state.add_code("JOIN");
        set_var_value(state, tokens[5]);
        return;
    }
    if(line_like("JOIN $number AND $number IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("JOIN statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        state.add_code(tokens[1]);
        state.add_code("TO-STR");
        state.add_code(tokens[3]);
        state.add_code("TO-STR");
        state.add_code("JOIN");
        set_var_value(state, tokens[5]);
        return;
    }
    if(line_like("JOIN $number AND $num-var IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("JOIN statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        state.add_code(tokens[1]);
        state.add_code("TO-STR");
        get_var_value(state, tokens[3]);
        state.add_code("TO-STR");
        state.add_code("JOIN");
        set_var_value(state, tokens[5]);
        return;
    }
    if(line_like("JOIN $number AND $str-var IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("JOIN statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        state.add_code(tokens[1]);
        state.add_code("TO-STR");
        get_var_value(state, tokens[3]);
        state.add_code("JOIN");
        set_var_value(state, tokens[5]);
        return;
    }
    
    if(line_like("JOIN $str-var AND $string IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("JOIN statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        get_var_value(state, tokens[1]);
        state.add_code(tokens[3]);
        state.add_code("JOIN");
        set_var_value(state, tokens[5]);
        return;
    }
    if(line_like("JOIN $str-var AND $number IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("JOIN statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        get_var_value(state, tokens[1]);
        state.add_code(tokens[3]);
        state.add_code("TO-STR");
        state.add_code("JOIN");
        set_var_value(state, tokens[5]);
        return;
    }
    if(line_like("JOIN $str-var AND $num-var IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("JOIN statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        get_var_value(state, tokens[1]);
        get_var_value(state, tokens[3]);
        state.add_code("TO-STR");
        state.add_code("JOIN");
        set_var_value(state, tokens[5]);
        return;
    }
    if(line_like("JOIN $str-var AND $str-var IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("JOIN statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        get_var_value(state, tokens[1]);
        get_var_value(state, tokens[3]);
        state.add_code("JOIN");
        set_var_value(state, tokens[5]);
        return;
    }
    
    if(line_like("JOIN $num-var AND $string IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("JOIN statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        get_var_value(state, tokens[1]);
        state.add_code("TO-STR");
        state.add_code(tokens[3]);
        state.add_code("JOIN");
        set_var_value(state, tokens[5]);
        return;
    }
    if(line_like("JOIN $num-var AND $number IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("JOIN statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        get_var_value(state, tokens[1]);
        state.add_code("TO-STR");
        state.add_code(tokens[3]);
        state.add_code("TO-STR");
        state.add_code("JOIN");
        set_var_value(state, tokens[5]);
        return;
    }
    if(line_like("JOIN $num-var AND $num-var IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("JOIN statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        get_var_value(state, tokens[1]);
        state.add_code("TO-STR");
        get_var_value(state, tokens[3]);
        state.add_code("TO-STR");
        state.add_code("JOIN");
        set_var_value(state, tokens[5]);
        return;
    }
    if(line_like("JOIN $num-var AND $str-var IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("JOIN statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        get_var_value(state, tokens[1]);
        state.add_code("TO-STR");
        get_var_value(state, tokens[3]);
        state.add_code("JOIN");
        set_var_value(state, tokens[5]);
        return;
    }
    if(line_like("GET CHARACTER AT $num-var FROM $str-var IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("GET CHARACTER statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        get_var_value(state, tokens[3]);
        get_var_value(state, tokens[5]);
        state.add_code("CHARAT");
        set_var_value(state, tokens[7]);
        return;
    }
    if(line_like("GET CHARACTER AT $number FROM $str-var IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("GET CHARACTER statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        state.add_code(tokens[3]);
        get_var_value(state, tokens[5]);
        state.add_code("CHARAT");
        set_var_value(state, tokens[7]);
        return;
    }
    if(line_like("GET CHARACTER AT $num-var FROM $string IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("GET CHARACTER statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        get_var_value(state, tokens[3]);
        state.add_code(tokens[5]);
        state.add_code("CHARAT");
        set_var_value(state, tokens[7]);
        return;
    }
    if(line_like("GET CHARACTER AT $number FROM $string IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("GET CHARACTER statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        state.add_code(tokens[3]);
        state.add_code(tokens[5]);
        state.add_code("CHARAT");
        set_var_value(state, tokens[7]);
        return;
    }
    if(line_like("SUB-PROCEDURE $name", tokens, state))
    {
        if(state.section_state != 2)
            error("SUB-PROCEDURE declaration outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(!is_subprocedure(tokens[1], state))
            state.subprocedures.push_back(tokens[1]);
        else
            error("Duplicate declaration for subprocedure " + tokens[1] + " (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(state.open_subprocedure != "")
            error("Subprocedure declaration inside subprocedure (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        else
            state.open_subprocedure = tokens[1];
        //NVM
        state.add_code("JMP:"+tokens[1]+"_end");
        state.add_code("@sub_"+tokens[1]);
        return;
    }
    if(line_like("RETURN", tokens, state))
    {
        if(state.section_state != 2)
            error("RETURN outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(state.open_subprocedure == "")
            error("RETURN found outside subprocedure (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        state.add_code("JMP-IP-POP");
        return;
    }
    if(line_like("END SUB-PROCEDURE", tokens, state))
    {
        if(state.section_state != 2)
            error("END SUB-PROCEDURE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(state.open_subprocedure == "")
            error("END SUB-PROCEDURE found outside subprocedure (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        state.add_code("JMP-IP-POP");
        state.add_code("@"+state.open_subprocedure+"_end");
        //Cierro la subrutina
        state.open_subprocedure = "";
        return;
    }
    
    if(line_like("IF $number IS EQUAL TO $number THEN", tokens, state))
    {
        if(state.section_state != 2)
            error("IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int if_num = state.add_if();
        state.add_code("@if" + to_string(if_num));
        state.add_code(tokens[1]);
        state.add_code(tokens[5]);
        state.add_code("==");
        state.add_code("NOT");
        state.add_code("IF:else"+to_string(if_num));
        return;
    }
    if(line_like("IF $number IS EQUAL TO $num-var THEN", tokens, state))
    {
        if(state.section_state != 2)
            error("IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int if_num = state.add_if();
        state.add_code("@if" + to_string(if_num));
        state.add_code(tokens[1]);
        get_var_value(state, tokens[5]);
        state.add_code("==");
        state.add_code("NOT");
        state.add_code("IF:else"+to_string(if_num));
        return;
    }
    if(line_like("IF $num-var IS EQUAL TO $number THEN", tokens, state))
    {
        if(state.section_state != 2)
            error("IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int if_num = state.add_if();
        state.add_code("@if" + to_string(if_num));
        get_var_value(state, tokens[1]);
        state.add_code(tokens[5]);
        state.add_code("==");
        state.add_code("NOT");
        state.add_code("IF:else"+to_string(if_num));
        return;
    }
    if(line_like("IF $num-var IS EQUAL TO $num-var THEN", tokens, state))
    {
        if(state.section_state != 2)
            error("IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int if_num = state.add_if();
        state.add_code("@if" + to_string(if_num));
        get_var_value(state, tokens[1]);
        get_var_value(state, tokens[5]);
        state.add_code("==");
        state.add_code("NOT");
        state.add_code("IF:else"+to_string(if_num));
        return;
    }
    
    if(line_like("IF $number IS NOT EQUAL TO $number THEN", tokens, state))
    {
        if(state.section_state != 2)
            error("IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int if_num = state.add_if();
        state.add_code("@if" + to_string(if_num));
        state.add_code(tokens[1]);
        state.add_code(tokens[6]);
        state.add_code("==");
        state.add_code("IF:else"+to_string(if_num));
        return;
    }
    if(line_like("IF $number IS NOT EQUAL TO $num-var THEN", tokens, state))
    {
        if(state.section_state != 2)
            error("IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int if_num = state.add_if();
        state.add_code("@if" + to_string(if_num));
        state.add_code(tokens[1]);
        get_var_value(state, tokens[6]);
        state.add_code("==");
        state.add_code("IF:else"+to_string(if_num));
        return;
    }
    if(line_like("IF $num-var IS NOT EQUAL TO $number THEN", tokens, state))
    {
        if(state.section_state != 2)
            error("IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int if_num = state.add_if();
        state.add_code("@if" + to_string(if_num));
        get_var_value(state, tokens[1]);
        state.add_code(tokens[6]);
        state.add_code("==");
        state.add_code("IF:else"+to_string(if_num));
        return;
    }
    if(line_like("IF $num-var IS NOT EQUAL TO $num-var THEN", tokens, state))
    {
        if(state.section_state != 2)
            error("IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int if_num = state.add_if();
        state.add_code("@if" + to_string(if_num));
        get_var_value(state, tokens[1]);
        get_var_value(state, tokens[6]);
        state.add_code("==");
        state.add_code("IF:else"+to_string(if_num));
        return;
    }
    
    if(line_like("IF $number IS GREATER THAN $number THEN", tokens, state))
    {
        if(state.section_state != 2)
            error("IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int if_num = state.add_if();
        state.add_code("@if" + to_string(if_num));
        state.add_code(tokens[1]);
        state.add_code(tokens[5]);
        state.add_code(">");
        state.add_code("NOT");
        state.add_code("IF:else"+to_string(if_num));
        return;
    }
    if(line_like("IF $number IS GREATER THAN $num-var THEN", tokens, state))
    {
        if(state.section_state != 2)
            error("IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int if_num = state.add_if();
        state.add_code("@if" + to_string(if_num));
        state.add_code(tokens[1]);
        get_var_value(state, tokens[5]);
        state.add_code(">");
        state.add_code("NOT");
        state.add_code("IF:else"+to_string(if_num));
        return;
    }
    if(line_like("IF $num-var IS GREATER THAN $number THEN", tokens, state))
    {
        if(state.section_state != 2)
            error("IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int if_num = state.add_if();
        state.add_code("@if" + to_string(if_num));
        get_var_value(state, tokens[1]);
        state.add_code(tokens[5]);
        state.add_code(">");
        state.add_code("NOT");
        state.add_code("IF:else"+to_string(if_num));
        return;
    }
    if(line_like("IF $num-var IS GREATER THAN $num-var THEN", tokens, state))
    {
        if(state.section_state != 2)
            error("IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int if_num = state.add_if();
        state.add_code("@if" + to_string(if_num));
        get_var_value(state, tokens[1]);
        get_var_value(state, tokens[5]);
        state.add_code(">");
        state.add_code("NOT");
        state.add_code("IF:else"+to_string(if_num));
        return;
    }
    
    if(line_like("IF $number IS LESS THAN $number THEN", tokens, state))
    {
        if(state.section_state != 2)
            error("IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int if_num = state.add_if();
        state.add_code("@if" + to_string(if_num));
        state.add_code(tokens[1]);
        state.add_code(tokens[5]);
        state.add_code("<");
        state.add_code("NOT");
        state.add_code("IF:else"+to_string(if_num));
        return;
    }
    if(line_like("IF $number IS LESS THAN $num-var THEN", tokens, state))
    {
        if(state.section_state != 2)
            error("IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int if_num = state.add_if();
        state.add_code("@if" + to_string(if_num));
        state.add_code(tokens[1]);
        get_var_value(state, tokens[5]);
        state.add_code("<");
        state.add_code("NOT");
        state.add_code("IF:else"+to_string(if_num));
        return;
    }
    if(line_like("IF $num-var IS LESS THAN $number THEN", tokens, state))
    {
        if(state.section_state != 2)
            error("IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int if_num = state.add_if();
        state.add_code("@if" + to_string(if_num));
        get_var_value(state, tokens[1]);
        state.add_code(tokens[5]);
        state.add_code("<");
        state.add_code("NOT");
        state.add_code("IF:else"+to_string(if_num));
        return;
    }
    if(line_like("IF $num-var IS LESS THAN $num-var THEN", tokens, state))
    {
        if(state.section_state != 2)
            error("IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int if_num = state.add_if();
        state.add_code("@if" + to_string(if_num));
        get_var_value(state, tokens[1]);
        get_var_value(state, tokens[5]);
        state.add_code("<");
        state.add_code("NOT");
        state.add_code("IF:else"+to_string(if_num));
        return;
    }
    
    if(line_like("IF $number IS GREATER THAN OR EQUAL TO $number THEN", tokens, state))
    {
        if(state.section_state != 2)
            error("IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int if_num = state.add_if();
        state.add_code("@if" + to_string(if_num));
        state.add_code(tokens[1]);
        state.add_code(tokens[8]);
        state.add_code(">=");
        state.add_code("NOT");
        state.add_code("IF:else"+to_string(if_num));
        return;
    }
    if(line_like("IF $number IS GREATER THAN OR EQUAL TO $num-var THEN", tokens, state))
    {
        if(state.section_state != 2)
            error("IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int if_num = state.add_if();
        state.add_code("@if" + to_string(if_num));
        state.add_code(tokens[1]);
        get_var_value(state, tokens[8]);
        state.add_code(">=");
        state.add_code("NOT");
        state.add_code("IF:else"+to_string(if_num));
        return;
    }
    if(line_like("IF $num-var IS GREATER THAN OR EQUAL TO $number THEN", tokens, state))
    {
        if(state.section_state != 2)
            error("IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int if_num = state.add_if();
        state.add_code("@if" + to_string(if_num));
        get_var_value(state, tokens[1]);
        state.add_code(tokens[8]);
        state.add_code(">=");
        state.add_code("NOT");
        state.add_code("IF:else"+to_string(if_num));
        return;
    }
    if(line_like("IF $num-var IS GREATER THAN OR EQUAL TO $num-var THEN", tokens, state))
    {
        if(state.section_state != 2)
            error("IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int if_num = state.add_if();
        state.add_code("@if" + to_string(if_num));
        get_var_value(state, tokens[1]);
        get_var_value(state, tokens[8]);
        state.add_code(">=");
        state.add_code("NOT");
        state.add_code("IF:else"+to_string(if_num));
        return;
    }
    
    if(line_like("IF $number IS LESS THAN OR EQUAL TO $number THEN", tokens, state))
    {
        if(state.section_state != 2)
            error("IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int if_num = state.add_if();
        state.add_code("@if" + to_string(if_num));
        state.add_code(tokens[1]);
        state.add_code(tokens[8]);
        state.add_code("<=");
        state.add_code("NOT");
        state.add_code("IF:else"+to_string(if_num));
        return;
    }
    if(line_like("IF $number IS LESS THAN OR EQUAL TO $num-var THEN", tokens, state))
    {
        if(state.section_state != 2)
            error("IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int if_num = state.add_if();
        state.add_code("@if" + to_string(if_num));
        state.add_code(tokens[1]);
        get_var_value(state, tokens[8]);
        state.add_code("<=");
        state.add_code("NOT");
        state.add_code("IF:else"+to_string(if_num));
        return;
    }
    if(line_like("IF $num-var IS LESS THAN OR EQUAL TO $number THEN", tokens, state))
    {
        if(state.section_state != 2)
            error("IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int if_num = state.add_if();
        state.add_code("@if" + to_string(if_num));
        get_var_value(state, tokens[1]);
        state.add_code(tokens[8]);
        state.add_code("<=");
        state.add_code("NOT");
        state.add_code("IF:else"+to_string(if_num));
        return;
    }
    if(line_like("IF $num-var IS LESS THAN OR EQUAL TO $num-var THEN", tokens, state))
    {
        if(state.section_state != 2)
            error("IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int if_num = state.add_if();
        state.add_code("@if" + to_string(if_num));
        get_var_value(state, tokens[1]);
        get_var_value(state, tokens[8]);
        state.add_code("<=");
        state.add_code("NOT");
        state.add_code("IF:else"+to_string(if_num));
        return;
    }
    
    if(line_like("IF $string IS EQUAL TO $string THEN", tokens, state))
    {
        if(state.section_state != 2)
            error("IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int if_num = state.add_if();
        state.add_code("@if" + to_string(if_num));
        state.add_code(tokens[1]);
        state.add_code(tokens[5]);
        state.add_code("==");
        state.add_code("NOT");
        state.add_code("IF:else"+to_string(if_num));
        return;
    }
    if(line_like("IF $string IS EQUAL TO $str-var THEN", tokens, state))
    {
        //NVM
        int if_num = state.add_if();
        state.add_code("@if" + to_string(if_num));
        state.add_code(tokens[1]);
        get_var_value(state, tokens[5]);
        state.add_code("==");
        state.add_code("NOT");
        state.add_code("IF:else"+to_string(if_num));
        return;
    }
    if(line_like("IF $str-var IS EQUAL TO $string THEN", tokens, state))
    {
        if(state.section_state != 2)
            error("IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int if_num = state.add_if();
        state.add_code("@if" + to_string(if_num));
        get_var_value(state, tokens[1]);
        state.add_code(tokens[5]);
        state.add_code("==");
        state.add_code("NOT");
        state.add_code("IF:else"+to_string(if_num));
        return;
    }
    if(line_like("IF $str-var IS EQUAL TO $str-var THEN", tokens, state))
    {
        if(state.section_state != 2)
            error("IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int if_num = state.add_if();
        state.add_code("@if" + to_string(if_num));
        get_var_value(state, tokens[1]);
        get_var_value(state, tokens[5]);
        state.add_code("==");
        state.add_code("NOT");
        state.add_code("IF:else"+to_string(if_num));
        return;
    }
    
    if(line_like("IF $string IS NOT EQUAL TO $string THEN", tokens, state))
    {
        if(state.section_state != 2)
            error("IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int if_num = state.add_if();
        state.add_code("@if" + to_string(if_num));
        state.add_code(tokens[1]);
        state.add_code(tokens[6]);
        state.add_code("==");
        state.add_code("IF:else"+to_string(if_num));
        return;
    }
    if(line_like("IF $string IS NOT EQUAL TO $str-var THEN", tokens, state))
    {
        if(state.section_state != 2)
            error("IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int if_num = state.add_if();
        state.add_code("@if" + to_string(if_num));
        state.add_code(tokens[1]);
        get_var_value(state, tokens[6]);
        state.add_code("==");
        state.add_code("IF:else"+to_string(if_num));
        return;
    }
    if(line_like("IF $str-var IS NOT EQUAL TO $string THEN", tokens, state))
    {
        if(state.section_state != 2)
            error("IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int if_num = state.add_if();
        state.add_code("@if" + to_string(if_num));
        get_var_value(state, tokens[1]);
        state.add_code(tokens[6]);
        state.add_code("==");
        state.add_code("IF:else"+to_string(if_num));
        return;
    }
    if(line_like("IF $str-var IS NOT EQUAL TO $str-var THEN", tokens, state))
    {
        if(state.section_state != 2)
            error("IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int if_num = state.add_if();
        state.add_code("@if" + to_string(if_num));
        get_var_value(state, tokens[1]);
        get_var_value(state, tokens[6]);
        state.add_code("==");
        state.add_code("IF:else"+to_string(if_num));
        return;
    }
    
    if(line_like("ELSE", tokens, state))
    {
        if(state.section_state != 2)
            error("ELSE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(state.if_stack.size() == 0)
            error("ELSE without IF (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int ifNum = state.if_stack.top();
        state.if_stack.pop();
        state.add_code("JMP:endif"+to_string(ifNum));
        state.add_code("@else"+to_string(ifNum));
        return;
    }
    if(line_like("END IF", tokens, state) || line_like("END-IF", tokens, state))
    {
        if(state.section_state != 2)
            error("END IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(state.if_stack.size() == 0)
            error("END IF without IF (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int ifNum = state.if_stack.top();
        state.if_stack.pop();
        //Si no tenía ELSE
        if(state.if_stack.size() > 0 && state.if_stack.top() == ifNum){
            state.if_stack.pop();
            state.add_code("@else"+to_string(ifNum));
        }
        state.add_code("@endif"+to_string(ifNum));
        return;
    }
    
        if(line_like("WHILE $number IS EQUAL TO $number DO", tokens, state))
    {
        if(state.section_state != 2)
            error("WHILE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int while_num = state.add_while();
        state.add_code("@while" + to_string(while_num));
        state.add_code(tokens[1]);
        state.add_code(tokens[5]);
        state.add_code("==");
        state.add_code("NOT");
        state.add_code("IF:exit_loop"+to_string(while_num));
        return;
    }
    if(line_like("WHILE $number IS EQUAL TO $num-var DO", tokens, state))
    {
        if(state.section_state != 2)
            error("WHILE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int while_num = state.add_while();
        state.add_code("@while" + to_string(while_num));
        state.add_code(tokens[1]);
        get_var_value(state, tokens[5]);
        state.add_code("==");
        state.add_code("NOT");
        state.add_code("IF:exit_loop"+to_string(while_num));
        return;
    }
    if(line_like("WHILE $num-var IS EQUAL TO $number DO", tokens, state))
    {
        if(state.section_state != 2)
            error("WHILE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int while_num = state.add_while();
        state.add_code("@while" + to_string(while_num));
        get_var_value(state, tokens[1]);
        state.add_code(tokens[5]);
        state.add_code("==");
        state.add_code("NOT");
        state.add_code("IF:exit_loop"+to_string(while_num));
        return;
    }
    if(line_like("WHILE $num-var IS EQUAL TO $num-var DO", tokens, state))
    {
        if(state.section_state != 2)
            error("WHILE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int while_num = state.add_while();
        state.add_code("@while" + to_string(while_num));
        get_var_value(state, tokens[1]);
        get_var_value(state, tokens[5]);
        state.add_code("==");
        state.add_code("NOT");
        state.add_code("IF:exit_loop"+to_string(while_num));
        return;
    }
    
    if(line_like("WHILE $number IS NOT EQUAL TO $number DO", tokens, state))
    {
        if(state.section_state != 2)
            error("WHILE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int while_num = state.add_while();
        state.add_code("@while" + to_string(while_num));
        state.add_code(tokens[1]);
        state.add_code(tokens[6]);
        state.add_code("==");
        state.add_code("IF:exit_loop"+to_string(while_num));
        return;
    }
    if(line_like("WHILE $number IS NOT EQUAL TO $num-var DO", tokens, state))
    {
        if(state.section_state != 2)
            error("WHILE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int while_num = state.add_while();
        state.add_code("@while" + to_string(while_num));
        state.add_code(tokens[1]);
        get_var_value(state, tokens[6]);
        state.add_code("==");
        state.add_code("IF:exit_loop"+to_string(while_num));
        return;
    }
    if(line_like("WHILE $num-var IS NOT EQUAL TO $number DO", tokens, state))
    {
        if(state.section_state != 2)
            error("WHILE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int while_num = state.add_while();
        state.add_code("@while" + to_string(while_num));
        get_var_value(state, tokens[1]);
        state.add_code(tokens[6]);
        state.add_code("==");
        state.add_code("IF:exit_loop"+to_string(while_num));
        return;
    }
    if(line_like("WHILE $num-var IS NOT EQUAL TO $num-var DO", tokens, state))
    {
        if(state.section_state != 2)
            error("WHILE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int while_num = state.add_while();
        state.add_code("@while" + to_string(while_num));
        get_var_value(state, tokens[1]);
        get_var_value(state, tokens[6]);
        state.add_code("==");
        state.add_code("IF:exit_loop"+to_string(while_num));
        return;
    }
    
    if(line_like("WHILE $number IS GREATER THAN $number DO", tokens, state))
    {
        if(state.section_state != 2)
            error("WHILE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int while_num = state.add_while();
        state.add_code("@while" + to_string(while_num));
        state.add_code(tokens[1]);
        state.add_code(tokens[5]);
        state.add_code(">");
        state.add_code("NOT");
        state.add_code("IF:exit_loop"+to_string(while_num));
        return;
    }
    if(line_like("WHILE $number IS GREATER THAN $num-var DO", tokens, state))
    {
        if(state.section_state != 2)
            error("WHILE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int while_num = state.add_while();
        state.add_code("@while" + to_string(while_num));
        state.add_code(tokens[1]);
        get_var_value(state, tokens[5]);
        state.add_code(">");
        state.add_code("NOT");
        state.add_code("IF:exit_loop"+to_string(while_num));
        return;
    }
    if(line_like("WHILE $num-var IS GREATER THAN $number DO", tokens, state))
    {
        if(state.section_state != 2)
            error("WHILE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int while_num = state.add_while();
        state.add_code("@while" + to_string(while_num));
        get_var_value(state, tokens[1]);
        state.add_code(tokens[5]);
        state.add_code(">");
        state.add_code("NOT");
        state.add_code("IF:exit_loop"+to_string(while_num));
        return;
    }
    if(line_like("WHILE $num-var IS GREATER THAN $num-var DO", tokens, state))
    {
        if(state.section_state != 2)
            error("WHILE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int while_num = state.add_while();
        state.add_code("@while" + to_string(while_num));
        get_var_value(state, tokens[1]);
        get_var_value(state, tokens[5]);
        state.add_code(">");
        state.add_code("NOT");
        state.add_code("IF:exit_loop"+to_string(while_num));
        return;
    }
    
    if(line_like("WHILE $number IS LESS THAN $number DO", tokens, state))
    {
        if(state.section_state != 2)
            error("WHILE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int while_num = state.add_while();
        state.add_code("@while" + to_string(while_num));
        state.add_code(tokens[1]);
        state.add_code(tokens[5]);
        state.add_code("<");
        state.add_code("NOT");
        state.add_code("IF:exit_loop"+to_string(while_num));
        return;
    }
    if(line_like("WHILE $number IS LESS THAN $num-var DO", tokens, state))
    {
        if(state.section_state != 2)
            error("WHILE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int while_num = state.add_while();
        state.add_code("@while" + to_string(while_num));
        state.add_code(tokens[1]);
        get_var_value(state, tokens[5]);
        state.add_code("<");
        state.add_code("NOT");
        state.add_code("IF:exit_loop"+to_string(while_num));
        return;
    }
    if(line_like("WHILE $num-var IS LESS THAN $number DO", tokens, state))
    {
        if(state.section_state != 2)
            error("WHILE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int while_num = state.add_while();
        state.add_code("@while" + to_string(while_num));
        get_var_value(state, tokens[1]);
        state.add_code(tokens[5]);
        state.add_code("<");
        state.add_code("NOT");
        state.add_code("IF:exit_loop"+to_string(while_num));
        return;
    }
    if(line_like("WHILE $num-var IS LESS THAN $num-var DO", tokens, state))
    {
        if(state.section_state != 2)
            error("WHILE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int while_num = state.add_while();
        state.add_code("@while" + to_string(while_num));
        get_var_value(state, tokens[1]);
        get_var_value(state, tokens[5]);
        state.add_code("<");
        state.add_code("NOT");
        state.add_code("IF:exit_loop"+to_string(while_num));
        return;
    }
    
    if(line_like("WHILE $number IS GREATER THAN OR EQUAL TO $number DO", tokens, state))
    {
        if(state.section_state != 2)
            error("WHILE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int while_num = state.add_while();
        state.add_code("@while" + to_string(while_num));
        state.add_code(tokens[1]);
        state.add_code(tokens[8]);
        state.add_code(">=");
        state.add_code("NOT");
        state.add_code("IF:exit_loop"+to_string(while_num));
        return;
    }
    if(line_like("WHILE $number IS GREATER THAN OR EQUAL TO $num-var DO", tokens, state))
    {
        if(state.section_state != 2)
            error("WHILE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int while_num = state.add_while();
        state.add_code("@while" + to_string(while_num));
        state.add_code(tokens[1]);
        get_var_value(state, tokens[8]);
        state.add_code(">=");
        state.add_code("NOT");
        state.add_code("IF:exit_loop"+to_string(while_num));
        return;
    }
    if(line_like("WHILE $num-var IS GREATER THAN OR EQUAL TO $number DO", tokens, state))
    {
        if(state.section_state != 2)
            error("WHILE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int while_num = state.add_while();
        state.add_code("@while" + to_string(while_num));
        get_var_value(state, tokens[1]);
        state.add_code(tokens[8]);
        state.add_code(">=");
        state.add_code("NOT");
        state.add_code("IF:exit_loop"+to_string(while_num));
        return;
    }
    if(line_like("WHILE $num-var IS GREATER THAN OR EQUAL TO $num-var DO", tokens, state))
    {
        if(state.section_state != 2)
            error("WHILE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int while_num = state.add_while();
        state.add_code("@while" + to_string(while_num));
        get_var_value(state, tokens[1]);
        get_var_value(state, tokens[8]);
        state.add_code(">=");
        state.add_code("NOT");
        state.add_code("IF:exit_loop"+to_string(while_num));
        return;
    }
    
    if(line_like("WHILE $number IS LESS THAN OR EQUAL TO $number DO", tokens, state))
    {
        if(state.section_state != 2)
            error("WHILE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int while_num = state.add_while();
        state.add_code("@while" + to_string(while_num));
        state.add_code(tokens[1]);
        state.add_code(tokens[8]);
        state.add_code("<=");
        state.add_code("NOT");
        state.add_code("IF:exit_loop"+to_string(while_num));
        return;
    }
    if(line_like("WHILE $number IS LESS THAN OR EQUAL TO $num-var DO", tokens, state))
    {
        if(state.section_state != 2)
            error("WHILE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int while_num = state.add_while();
        state.add_code("@while" + to_string(while_num));
        state.add_code(tokens[1]);
        get_var_value(state, tokens[8]);
        state.add_code("<=");
        state.add_code("NOT");
        state.add_code("IF:exit_loop"+to_string(while_num));
        return;
    }
    if(line_like("WHILE $num-var IS LESS THAN OR EQUAL TO $number DO", tokens, state))
    {
        if(state.section_state != 2)
            error("WHILE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int while_num = state.add_while();
        state.add_code("@while" + to_string(while_num));
        get_var_value(state, tokens[1]);
        state.add_code(tokens[8]);
        state.add_code("<=");
        state.add_code("NOT");
        state.add_code("IF:exit_loop"+to_string(while_num));
        return;
    }
    if(line_like("WHILE $num-var IS LESS THAN OR EQUAL TO $num-var DO", tokens, state))
    {
        if(state.section_state != 2)
            error("WHILE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int while_num = state.add_while();
        state.add_code("@while" + to_string(while_num));
        get_var_value(state, tokens[1]);
        get_var_value(state, tokens[8]);
        state.add_code("<=");
        state.add_code("NOT");
        state.add_code("IF:exit_loop"+to_string(while_num));
        return;
    }
    
    if(line_like("WHILE $string IS EQUAL TO $string DO", tokens, state))
    {
        if(state.section_state != 2)
            error("WHILE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int while_num = state.add_while();
        state.add_code("@while" + to_string(while_num));
        state.add_code(tokens[1]);
        state.add_code(tokens[5]);
        state.add_code("==");
        state.add_code("NOT");
        state.add_code("IF:exit_loop"+to_string(while_num));
        return;
    }
    if(line_like("WHILE $string IS EQUAL TO $str-var DO", tokens, state))
    {
        //NVM
        int while_num = state.add_while();
        state.add_code("@while" + to_string(while_num));
        state.add_code(tokens[1]);
        get_var_value(state, tokens[5]);
        state.add_code("==");
        state.add_code("NOT");
        state.add_code("IF:exit_loop"+to_string(while_num));
        return;
    }
    if(line_like("WHILE $str-var IS EQUAL TO $string DO", tokens, state))
    {
        if(state.section_state != 2)
            error("WHILE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int while_num = state.add_while();
        state.add_code("@while" + to_string(while_num));
        get_var_value(state, tokens[1]);
        state.add_code(tokens[5]);
        state.add_code("==");
        state.add_code("NOT");
        state.add_code("IF:exit_loop"+to_string(while_num));
        return;
    }
    if(line_like("WHILE $str-var IS EQUAL TO $str-var DO", tokens, state))
    {
        if(state.section_state != 2)
            error("WHILE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int while_num = state.add_while();
        state.add_code("@while" + to_string(while_num));
        get_var_value(state, tokens[1]);
        get_var_value(state, tokens[5]);
        state.add_code("==");
        state.add_code("NOT");
        state.add_code("IF:exit_loop"+to_string(while_num));
        return;
    }
    
    if(line_like("WHILE $string IS NOT EQUAL TO $string DO", tokens, state))
    {
        if(state.section_state != 2)
            error("WHILE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int while_num = state.add_while();
        state.add_code("@while" + to_string(while_num));
        state.add_code(tokens[1]);
        state.add_code(tokens[6]);
        state.add_code("==");
        state.add_code("IF:exit_loop"+to_string(while_num));
        return;
    }
    if(line_like("WHILE $string IS NOT EQUAL TO $str-var DO", tokens, state))
    {
        if(state.section_state != 2)
            error("WHILE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int while_num = state.add_while();
        state.add_code("@while" + to_string(while_num));
        state.add_code(tokens[1]);
        get_var_value(state, tokens[6]);
        state.add_code("==");
        state.add_code("IF:exit_loop"+to_string(while_num));
        return;
    }
    if(line_like("WHILE $str-var IS NOT EQUAL TO $string DO", tokens, state))
    {
        if(state.section_state != 2)
            error("WHILE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int while_num = state.add_while();
        state.add_code("@while" + to_string(while_num));
        get_var_value(state, tokens[1]);
        state.add_code(tokens[6]);
        state.add_code("==");
        state.add_code("IF:exit_loop"+to_string(while_num));
        return;
    }
    if(line_like("WHILE $str-var IS NOT EQUAL TO $str-var DO", tokens, state))
    {
        if(state.section_state != 2)
            error("WHILE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int while_num = state.add_while();
        state.add_code("@while" + to_string(while_num));
        get_var_value(state, tokens[1]);
        get_var_value(state, tokens[6]);
        state.add_code("==");
        state.add_code("IF:exit_loop"+to_string(while_num));
        return;
    }
    
    if(line_like("REPEAT", tokens, state))
    {
        if(state.section_state != 2)
            error("REPEAT outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(state.while_stack.size() == 0)
            error("REPEAT without WHILE (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int while_num = state.while_stack.top();
        state.while_stack.pop();
        state.add_code("JMP:while"+to_string(while_num));
        state.add_code("@exit_loop"+to_string(while_num));
        return;
    }
    
    if(line_like("BREAK", tokens, state))
    {
        if(state.section_state != 2)
            error("BREAK outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(state.while_stack.size() == 0)
            error("BREAK without WHILE (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int while_num = state.while_stack.top();
        state.add_code("JMP:exit_loop"+to_string(while_num));
        return;
    }
    
    if(line_like("CONTINUE", tokens, state))
    {
        if(state.section_state != 2)
            error("CONTINUE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(state.while_stack.size() == 0)
            error("CONTINUE without WHILE (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        int while_num = state.while_stack.top();
        state.add_code("JMP:while"+to_string(while_num));
        return;
    }
    
    if(line_like("CALL SUB-PROCEDURE $subprocedure", tokens, state))
    {
        if(state.section_state != 2)
            error("CALL SUB-PROCEDURE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        state.add_code("IP");
        state.add_code("3");
        state.add_code("+");
        state.add_code("JMP:sub_"+tokens[2]);
        return;
    }
    
    if(line_like("EXECUTE $string", tokens, state))
    {
        if(state.section_state != 2)
            error("EXECUTE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        state.add_code(tokens[1]);
        state.add_code("SYS-EXEC");
        state.add_code("POP");
        return;
    }
    if(line_like("EXECUTE $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("EXECUTE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        get_var_value(state, tokens[1]);
        state.add_code("SYS-EXEC");
        state.add_code("POP");
        return;
    }
    if(line_like("EXECUTE $string AND STORE OUTPUT IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("EXECUTE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        state.add_code(tokens[1]);
        state.add_code("SYS-EXEC-OUT");
        set_var_value(state, tokens[6]);
        return;
    }
    if(line_like("EXECUTE $str-var AND STORE OUTPUT IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("EXECUTE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        get_var_value(state, tokens[1]);
        state.add_code("SYS-EXEC-OUT");
        set_var_value(state, tokens[6]);
        return;
    }
    if(line_like("EXECUTE $string AND STORE EXIT CODE IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("EXECUTE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        state.add_code(tokens[1]);
        state.add_code("SYS-EXEC");
        set_var_value(state, tokens[7]);
        return;
    }
    if(line_like("EXECUTE $str-var AND STORE EXIT CODE IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("EXECUTE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        get_var_value(state, tokens[1]);
        state.add_code("SYS-EXEC");
        set_var_value(state, tokens[7]);
        return;
    }
    if(line_like("EXIT", tokens, state))
    {
        if(state.section_state != 2)
            error("EXIT outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        state.add_code("HALT");
        return;
    }
    if(line_like("STORE LENGTH OF $str-var IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("EXECUTE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        get_var_value(state, tokens[3]);
        state.add_code("LENGTH");
        set_var_value(state, tokens[5]);
        return;
    }
    if(line_like("STORE LENGTH OF $string IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("EXECUTE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        state.add_code(tokens[3]);
        state.add_code("LENGTH");
        set_var_value(state, tokens[5]);
        return;
    }
	//Desde acá faltan en el standard
    if(line_like("STORE RANDOM IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("RANDOM outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        state.add_code("RANDOM");
        set_var_value(state, tokens[3]);
        return;
    }
    if(line_like("FLOOR $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("FLOOR statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        get_var_value(state, tokens[1]);
        state.add_code("FLOOR");
        set_var_value(state, tokens[1]);
        return;
    }
    if(line_like("CEIL $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("CEIL statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        get_var_value(state, tokens[1]);
        state.add_code("CEIL");
        set_var_value(state, tokens[1]);
        return;
    }
	if(line_like("STORE $number IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("STORE statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        state.add_code(tokens[1]);
		state.add_code("TO-STR");
        set_var_value(state, tokens[3]);
        return;
    }
	if(line_like("STORE $num-var IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("STORE statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        get_var_value(state, tokens[1]);
		state.add_code("TO-STR");
        set_var_value(state, tokens[3]);
        return;
    }
	if(line_like("STORE $string IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("STORE statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        state.add_code(tokens[1]);
		state.add_code("TO-NUM");
        set_var_value(state, tokens[3]);
        return;
    }
	if(line_like("STORE $str-var IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("STORE statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        get_var_value(state, tokens[1]);
		state.add_code("TO-NUM");
        set_var_value(state, tokens[3]);
        return;
    }

    if(line_like("STORE CHARACTER $number IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("STORE statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        state.add_code(tokens[2]);
        state.add_code("CHR");
        set_var_value(state, tokens[4]);
        return;
    }
    if(line_like("STORE CHARACTER $num-var IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("STORE statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        get_var_value(state, tokens[2]);
        state.add_code("CHR");
        set_var_value(state, tokens[4]);
        return;
    }

    if(line_like("STORE CHARACTER CODE OF $string IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("STORE statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        state.add_code(tokens[4]);
        state.add_code("ORD");
        set_var_value(state, tokens[6]);
        return;
    }
    if(line_like("STORE CHARACTER CODE OF $str-var IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("STORE statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //NVM
        get_var_value(state, tokens[4]);
        state.add_code("ORD");
        set_var_value(state, tokens[6]);
        return;
    }
    
    
    error("Malformed statement (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
}

//Check if the tokens of a line passed are like the ones of a model line
bool line_like(string model_line, vector<string> tokens, compiler_state & state)
{
    //Tokenize model line
    vector<string> model_tokens;
    tokenize(model_line, 0, model_tokens, state.current_file);
    //Check that tokens match between line and model line
    if(tokens.size() < model_tokens.size()) return false;
    unsigned int i = 0;
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
        else if(model_tokens[i] == "$natural") //$natural is an integer greater than 0
        {
            if(!is_natural(tokens[i])) return false;
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
        else if(model_tokens[i] == "$subprocedure") //$subprocedure is a SUB-PROCEDURE (?
        {
            if(!is_subprocedure(tokens[i], state)) return false;
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

bool is_natural(string number){
    if(!is_number(number)) return false;
    if(stod(number) <= 0) return false;
    for(char l : number)
        if(l == '.') return false;
    return true;
}

bool is_string(string & token){
    return (token.size() >= 2 && token[0] == '"' && token[token.size()-1] == '"');
}

bool is_num_vector(string & token, compiler_state & state)
{
    for(pair<string, unsigned int> & var : state.variables)
        if(var.first == token && var.second == 3) return true;
    return false;
}

bool is_txt_vector(string & token, compiler_state & state)
{
    for(pair<string, unsigned int> & var : state.variables)
        if(var.first == token && var.second == 4) return true;
    return false;
}

bool is_vector(string & token, compiler_state & state)
{
    return is_num_vector(token, state) || is_txt_vector(token, state);
}

bool is_vector_index(queue<string> & token, compiler_state & state)
{
    string current_token = token.front();
    token.pop();
    if(token.size() > 1){
        if(!is_vector(current_token, state)) return false;
        return is_vector_index(token, state);
    }
    else if(token.size() == 1){
        if(!is_vector(current_token, state)) return false;
        if(!is_num_var(token.front(), state) && !is_number(token.front()) && !is_string(token.front())) return false;
        return true;
    }
    return false;
}

bool is_num_var(string & token, compiler_state & state)
{
    //Veo si var
    for(pair<string, unsigned int> & var : state.variables)
        if(var.first == token && var.second == 1) return true;
    //Veo si num_vector index
    queue<string> vpart;
    split_vector(token, vpart);
    return is_num_vector(vpart.front(), state) && is_vector_index(vpart, state);
}

bool is_txt_var(string & token, compiler_state & state)
{
    for(pair<string, unsigned int> & var : state.variables)
        if(var.first == token && var.second == 2) return true;
    //Veo si num_vector index
    queue<string> vpart;
    split_vector(token, vpart);
    return is_txt_vector(vpart.front(), state) && is_vector_index(vpart, state);
}

bool is_variable(string & token, compiler_state & state)
{
    return is_num_var(token, state) || is_txt_var(token, state);
}

void split_vector(string & line, queue<string> & tokens)
{
    bool in_string = false;
    string current_token = "";
    //For each letter in the line
    for(unsigned int i = 0; i < line.size(); ++i)
    {
        char letter = line[i];
        if(letter == ':')
        {
            if(in_string) current_token += letter;
            else
            {
                if(current_token.size() > 0)
                    tokens.push(current_token);
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
                else error("unknown escape sequence on a VECTOR access.");
            }
            else error("\\ found alone on a VECTOR access.");
        }
        else if(letter == '#') //Comment character
        {
            if(in_string) current_token += letter;
            else
            {
                if(current_token.size() > 0)
                    tokens.push(current_token);
                return;
            }
        }
        else
        {
            current_token += letter;
        }
        if(i == line.size() - 1){
            if(letter != ':')
            {
                if(in_string) error("Unterminated string on a VECTOR access.");
                if(current_token.size() > 0)
                        tokens.push(current_token);
            }
            else error("Incomplete VECTOR access found (can't end on ':'!).");
        }
    }
}

/*La diferencia entre is_variable y variable_exists es que is_variable
 no toma los vectores como una variable, sino los índices de los vectores.
 Variable exists te dice si una variable está declarada como tal, por lo que
 un subíndice de vector no sería una variable.*/
bool variable_exists(string & token, compiler_state & state)
{
    for(pair<string, unsigned int> & var : state.variables)
        if(var.first == token) return true;
    return false;
}

bool is_subprocedure(string & token, compiler_state & state)
{
    for(string & var : state.subprocedures)
        if(var == token) return true;
    return false;
}

void get_var_value(compiler_state & state, string & variable)
{
    queue<string> vpart;
    split_vector(variable, vpart);
    if(vpart.size() == 1){
        state.add_code("AUX:"+variable);
        return;
    }
    stack<string> token;
    while(!vpart.empty())
    {
        token.push(vpart.front());
        vpart.pop();
    }
    //Last element of vector access:
    string t = token.top();
    token.pop();
    if(is_number(t) || is_string(t)) state.add_code(t);
    else state.add_code("AUX:" + t);
    
    while(!token.empty())
    {
        t = token.top();
        token.pop();
        state.add_code("\"" + t + ":\"");
        state.add_code("SWAP");
        state.add_code("TO-STR");
        state.add_code("JOIN");
        state.add_code("AUX-POP");
    }
}

//Setea el valor de una variable o vector
void set_var_value(compiler_state & state, string & variable)
{
    queue<string> vpart;
    split_vector(variable, vpart);
    if(vpart.size() == 1){
        state.add_code("TOAUX:" + variable);
        return;
    }
    stack<string> token;
    while(!vpart.empty())
    {
        token.push(vpart.front());
        vpart.pop();
    }
    //Last element of vector access:
    string t = token.top();
    token.pop();
    if(is_number(t) || is_string(t)) state.add_code(t);
    else state.add_code("AUX:" + t);
    
    while(!token.empty())
    {
        t = token.top();
        token.pop();
        state.add_code("\"" + t + ":\"");
        state.add_code("SWAP");
        state.add_code("TO-STR");
        state.add_code("JOIN");
        if(!token.empty())
            state.add_code("AUX-POP");
        else{
            state.add_code("SWAP");
            state.add_code("TOAUX-POP");
        }
    }
}

