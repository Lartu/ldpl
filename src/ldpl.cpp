#include "ldpl.h"
#include "ldpl_included_lib.cpp"

//Show internal representation
bool show_ir = false;

template <typename T>
static bool contains(vector<T> const& vec, T const& val)
{
    return vec.end() != find(vec.begin(), vec.end(), val);
}

template <typename T>
static bool contains_any(vector<T> const& vec, vector<T> vals)
{
    for (auto const& v : vals)
    {
        if (contains(vec, v))
            return true;
    }
    return false;
}

int main(int argc, const char* argv[])
{
    //Get command line arguments as string vector
    vector<string> args(argv + 1, argv + argc);

    if(contains_any(args, {"-v", "--version"})){
        cout << "This is \033[32;1mLDPL version " << VERSION << "\033[0m." << endl << endl;
        cout << "Copyright 2018-2019, \033[35;1mMartín del Río\033[0m (www.lartu.net)." << endl;
	cout << "Built with amazing contributions from \033[35;1mʇʞʌp\033[0m and a bunch of other contributors." << endl << endl;
        cout << "Website and documentation can be found on \033[36;1mwww.ldpl-lang.org\033[0m." << endl;
        cout << "Source code can be found at \033[36;1mhttps://github.com/lartu/ldpl\033[0m." << endl << endl;
        cout << "LDPL may be copied only under the terms of the GNU General Public License 3.0." << endl <<endl;
        cout << "Compiled on " << COMPILEDATE << endl;
        return 0;
    }
    else if(contains_any(args, {"-h", "--help"})){
        cout << "Usage: ldpl [options] file [arguments]" << endl;
        cout << "Options:" << endl;
        cout << "  -h --help                Display this information" << endl;
        cout << "  -r                       Display generated C++ code" << endl;
        cout << "  -o=<name>                Set output file for compiled binary" << endl;
        cout << "  -i=<file>                Include file in current compilation" << endl;
        cout << "  -f=<flag>                Pass a flag to the C++ compiler" << endl;
        cout << "  -v --version             Display LDPL version information" << endl;
		#ifdef  STATIC_BUILDS
		cout << "  -ns             			Disables static binary building" << endl;
		#endif
        return 0;
    }

    compiler_state state; //Compiler state (holds variables, sections, functions, etc)
    vector<string> files_to_compile;
    vector<string> extensions;
    add_ldpllib(state);

#ifdef STATIC_BUILDS
	bool no_static = false;
#endif

    string output_filename = "";
    string final_filename = "";

    //Check arguments
    if(args.size() >= 1){
        for(string & arg : args){
            if(arg.size() > 1 && arg[0] != '-'){
                if(output_filename == "") output_filename = arg;
                files_to_compile.push_back(arg);
            }
            else if(arg == "-r"){
                show_ir = true;
            }
			#ifdef STATIC_BUILDS
			else if(arg == "-ns"){
                no_static = true;
            }
			#endif
            else if(arg.substr(0, 3) == "-o="){
                final_filename = arg.substr(3);
            }
            else if(arg.substr(0, 3) == "-i="){
                if(0 == arg.compare(arg.length()-5, 5, ".ldpl")||0 == arg.compare(arg.length()-4, 4, ".lsc")){
                    files_to_compile.insert(files_to_compile.begin(), arg.substr(3));
                }else{
                    //pass everything but .ldpl and .lsc files to the c++ compiler
                    extensions.push_back(arg.substr(3)); // kill -i= prefix
                }
            }
            else if(arg.substr(0, 3) == "-f="){
                //pass flags to the c++ compiler (for example -f=-lSDL)
                extensions.push_back(arg.substr(3));
            }
        }
    }

    state.add_code("int main(int argc, char *argv[]){");
    state.add_code("cout.precision(numeric_limits<ldpl_number>::digits10);");
    state.add_code("srand (time(NULL));");    

    state.variables["ARGC"] = 1;
    state.add_var_code("ldpl_number "+fix_identifier("ARGC", true)+";");
    state.variables["ARGV"] = 4;
    state.add_var_code("ldpl_vector<string> "+fix_identifier("ARGV", true)+";");
    state.add_code("for(int i = 1; i < argc; ++i)");
    state.add_code(fix_identifier("ARGV", true) + "[i-1] = argv[i];");
    state.add_code(fix_identifier("ARGC", true)+" = argc - 1;");

    //Fail if file was not passed
    if(files_to_compile.size() == 0) error("Filename expected.");

    //For each file, compile each file into one big code
    for(string & filename : files_to_compile)
    {
        //If it's an argument
        if(filename[0] == '-') continue;
        //Reset state section for this file
        state.section_state = 0;
        state.current_file = filename;
        load_and_compile(filename, state);
    }
    state.add_code("return 0; \n}");

    //If only IR was required
    if(show_ir){
        for(string line : state.variable_code) cout << line << endl;
        for(string line : state.subroutine_code) cout << line << endl;
        for(string line : state.output_code) cout << line << endl;
        exit(0);
    }
    //Run the compiled code
    ofstream myfile;
    myfile.open ("ldpl-temp.cpp");
    for(string line : state.variable_code) myfile << line << endl;
    for(string line : state.subroutine_code) myfile << line << endl;
    for(string line : state.output_code) myfile << line << endl;
    myfile.close();

    //Generate output filename if not set by -o=
    if(final_filename.empty()) {
        for(unsigned int i = 0; i < output_filename.size(); ++i){
            if(output_filename[i] != '.')
                final_filename += output_filename[i];
            else
                break;
        }
        if(final_filename.size() == 0) final_filename = "ldpl-output";
        final_filename += "-bin";
    }
    cout << "LDPL: Compiling..." << endl;
    string compile_line = "c++ ldpl-temp.cpp -std=gnu++11 -o " + final_filename;
#ifdef STATIC_BUILDS
    if(!no_static) compile_line+=" -static-libgcc -static-libstdc++ ";
#endif
    if(!extensions.empty()){
        for(string & extension : extensions) compile_line += " "+extension;
    }
    int compiled = system(compile_line.c_str());
    system("rm ldpl-temp.cpp");
    if(compiled == 0){
        cout << "*\033[32;1m File(s) compiled successfully.\033[0m" << endl;
        cout << "* Saved as " << final_filename << endl;
    }else{
        error("compilation failed.");
    }
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

        if(state.open_quote){
            //Check for END QUOTE first
            if(line.size() >= 9 && (line[0] == 'E' || line[0] == 'e')){
                string upper = "";
                for(char c : line) upper += toupper(c);
                trim(upper);
                if(upper == "END QUOTE"){
                    state.open_quote = false;
                    //Kill final newline. Programs can add crlf if needed.
                    string & prev = state.output_code.back();
                    size_t pos = prev.rfind("\\n");
                    if(pos != string::npos) prev.erase(pos, 2);
                    prev += ";"; 
                    continue;
                }
            }

            //No END QUOTE, emit the line as C++
            state.add_code("\"" + escape_c_quotes(line) + "\\n\"");
            continue;
        }

        trim(line);
        //Split line in various tokens
        vector<string> tokens;
        tokenize(line, line_num, tokens, state.current_file);
        capitalize_tokens(tokens);
        if(tokens.size() == 0) continue;
        compile_line(tokens, line_num, state);
    }
    if(state.open_ifs > 0){
        error("there may be open IF blocks in your code.");
        exit(1);
    }
    if(state.while_stack.size() > 0){
        error("there may be open WHILE blocks in your code.");
        exit(1);
    }
    if(state.open_quote) error("your QUOTE block was not terminated.");
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
                switch(next_letter)
                {
                    case '\\': case '"': case '0':
                    case 'a': case 'b': case 't': case 'n':
                    case 'v': case 'f': case 'r': case 'e':
                        current_token += "\\" + string(1, next_letter);
                    break;
                    default:
                        error("unknown escape sequence (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
                }
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
            if(is_vector_index(token))
            {
                for(char & l : token){
                    if (l == ':') break;
                    l = toupper(l);
                }
            }
            else if(!is_number(token) && !is_string(token))
            {
                for(char & l : token){
                    l = toupper(l);
                }
                if (token == "CRLF") token = "\"\\n\"";
            }
    }
}

//Compiles line per line
void compile_line(vector<string> & tokens, unsigned int line_num, compiler_state & state)
{
    string & current_file = state.current_file;
    ++line_num;

    //increment open IF count if this is an if statement
    if(tokens[0] == "IF")
        ++state.open_ifs;

    //handle ELSE and ELSE IF 
    if(tokens[0] == "ELSE"){
        if(state.section_state != 2)
            error("ELSE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(state.open_ifs == 0)
            error("ELSE without IF (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");

        if(tokens.size() == 1){ // ELSE
            state.add_code("}else{");
            return;
        } else if(tokens[1] == "IF"){ // ELSE IF
            state.add_code("}else ");
            //remove the ELSE input token so it becomes a regular IF,
            //then continue compiling this line.
            tokens.erase(tokens.begin()+0);
        }
    }
    
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
            state.variables[tokens[0]] = 1;
        else
            error("Duplicate declaration for variable " + tokens[0] + " (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_var_code("ldpl_number " + fix_identifier(tokens[0], true) + " = 0;");
        return;
    }
    if(line_like("$name IS TEXT", tokens, state))
    {
        if(state.section_state != 1)
            error("Variable declaration outside DATA section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(!variable_exists(tokens[0], state))
            state.variables[tokens[0]] = 2;
        else
            error("Duplicate declaration for variable " + tokens[0] + " (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_var_code("string " + fix_identifier(tokens[0], true) + " = \"\";");
        return;
    }
    if(line_like("$name IS NUMBER VECTOR", tokens, state))
    {
        if(state.section_state != 1)
            error("Variable declaration outside DATA section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(!variable_exists(tokens[0], state)){
            state.variables[tokens[0]] = 3;
        }
        else
            error("Duplicate declaration for variable " + tokens[0] + " (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_var_code("ldpl_vector<ldpl_number> " + fix_identifier(tokens[0], true) + ";");
        return;
    }
    if(line_like("$name IS TEXT VECTOR", tokens, state))
    {
        if(state.section_state != 1)
            error("Variable declaration outside DATA section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(!variable_exists(tokens[0], state)){
            state.variables[tokens[0]] = 4;
        }
        else
            error("Duplicate declaration for variable " + tokens[0] + " (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_var_code("ldpl_vector<string> " + fix_identifier(tokens[0], true) + ";");
        return;
    }if(line_like("$name IS EXTERNAL NUMBER", tokens, state))
    {
        if(state.section_state != 1)
            error("Variable declaration outside DATA section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(!variable_exists(tokens[0], state))
            state.variables[tokens[0]] = 1;
        else
            error("Duplicate declaration for variable " + tokens[0] + " (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        state.add_var_code("extern ldpl_number " + fix_external_identifier(tokens[0], true) + ";");
        state.externals[tokens[0]] = true;
        return;
    }
    if(line_like("$name IS EXTERNAL TEXT", tokens, state))
    {
        if(state.section_state != 1)
            error("Variable declaration outside DATA section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(!variable_exists(tokens[0], state))
            state.variables[tokens[0]] = 2;
        else
            error("Duplicate declaration for variable " + tokens[0] + " (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        state.add_var_code("extern string " + fix_external_identifier(tokens[0], true) + ";");
        state.externals[tokens[0]] = true;
        return;
    }
    if(line_like("$name IS EXTERNAL NUMBER VECTOR", tokens, state))
    {
        if(state.section_state != 1)
            error("Variable declaration outside DATA section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(!variable_exists(tokens[0], state)){
            state.variables[tokens[0]] = 3;
        }
        else
            error("Duplicate declaration for variable " + tokens[0] + " (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        state.add_var_code("extern ldpl_vector<ldpl_number> " + fix_external_identifier(tokens[0], true) + ";");
        state.externals[tokens[0]] = true;
        return;
    }
    if(line_like("$name IS EXTERNAL TEXT VECTOR", tokens, state))
    {
        if(state.section_state != 1)
            error("Variable declaration outside DATA section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(!variable_exists(tokens[0], state)){
            state.variables[tokens[0]] = 4;
        }
        else
            error("Duplicate declaration for variable " + tokens[0] + " (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        state.add_var_code("extern ldpl_vector<string> " + fix_external_identifier(tokens[0], true) + ";");
        state.externals[tokens[0]] = true;
        return;
    }
    if(line_like("DISPLAY $display", tokens, state))
    {
        if(state.section_state != 2)
            error("DISPLAY statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        for(unsigned int i = 1; i < tokens.size(); ++i){
            //TODO ADD COLORS HERE
            if(is_variable(tokens[i], state)){
                state.add_code("cout << " + get_c_variable(state, tokens[i]) + " << flush;");
            }
            else{
                state.add_code("cout << " + tokens[i] + " << flush;");
            }
        }
        return;
    }
    if(line_like("ACCEPT $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("ACCEPT statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[1]) + " = input_number();");
        return;
    }
    if(line_like("ACCEPT $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("ACCEPT statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[1]) + " = input_string();");
        return;
    }
    if(line_like("ACCEPT $str-var UNTIL EOF", tokens, state))
    {
        if(state.section_state != 2)
            error("ACCEPT statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[1])+" = input_until_eof();");
        return;
    }
    if(line_like("STORE $num-var IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("STORE statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[3]) + " = " + get_c_variable(state, tokens[1]) + ";");
        return;
    }
    if(line_like("STORE $number IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("STORE statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[3]) + " = " + tokens[1] + ";");
        return;
    }
    if(line_like("STORE $str-var IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("STORE statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[3]) + " = " + get_c_variable(state, tokens[1]) + ";");
        return;
    }
    if(line_like("STORE $string IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("STORE statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[3]) + " = " + tokens[1] + ";");
        return;
    }
    if(line_like("ADD $number AND $number IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("ADD statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[5]) + " = " + tokens[1] + " + " + tokens[3] + ";");
        return;
    }
    if(line_like("ADD $number AND $num-var IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("ADD statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[5]) + " = " + tokens[1] + " + " + get_c_variable(state, tokens[3]) + ";");
        return;
    }
    if(line_like("ADD $num-var AND $number IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("ADD statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[5]) + " = " + get_c_variable(state, tokens[1]) + " + " + tokens[3] + ";");
        return;
    }
    if(line_like("ADD $num-var AND $num-var IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("ADD statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[5]) + " = " + get_c_variable(state, tokens[1]) + " + " + get_c_variable(state, tokens[3]) + ";");
        return;
    }
    if(line_like("SUBTRACT $number FROM $number IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("SUBTRACT statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[5]) + " = " + tokens[3] + " - " + tokens[1] + ";");
        return;
    }
    if(line_like("SUBTRACT $number FROM $num-var IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("SUBTRACT statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[5]) + " = " + get_c_variable(state, tokens[3]) + " - " + tokens[1] + ";");
        return;
    }
    if(line_like("SUBTRACT $num-var FROM $number IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("SUBTRACT statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[5]) + " = " + tokens[3] + " - " + get_c_variable(state, tokens[1]) + ";");
        return;
    }
    if(line_like("SUBTRACT $num-var FROM $num-var IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("SUBTRACT statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[5]) + " = " + get_c_variable(state, tokens[3]) + " - " + get_c_variable(state, tokens[1]) + ";");
        return;
    }
    if(line_like("DIVIDE $number BY $number IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("DIVIDE statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[5]) + " = " + tokens[1] + " / " + tokens[3] + ";");
        return;
    }
    if(line_like("DIVIDE $number BY $num-var IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("DIVIDE statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[5]) + " = " + tokens[1] + " / " + get_c_variable(state, tokens[3]) + ";");
        return;
    }
    if(line_like("DIVIDE $num-var BY $number IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("DIVIDE statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[5]) + " = " + get_c_variable(state, tokens[1]) + " / " + tokens[3] + ";");
        return;
    }
    if(line_like("DIVIDE $num-var BY $num-var IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("DIVIDE statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[5]) + " = " + get_c_variable(state, tokens[1]) + " / " + get_c_variable(state, tokens[3]) + ";");
        return;
    }
    if(line_like("MULTIPLY $number BY $number IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("MULTIPLY statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[5]) + " = " + tokens[1] + " * " + tokens[3] + ";");
        return;
    }
    if(line_like("MULTIPLY $number BY $num-var IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("MULTIPLY statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[5]) + " = " + get_c_variable(state, tokens[1]) + " * " + tokens[3] + ";");
        return;
    }
    if(line_like("MULTIPLY $num-var BY $number IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("MULTIPLY statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[5]) + " = " + get_c_variable(state, tokens[1]) + " * " + tokens[3] + ";");
        return;
    }
    if(line_like("MULTIPLY $num-var BY $num-var IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("MULTIPLY statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[5]) + " = " + get_c_variable(state, tokens[1]) + " * " + get_c_variable(state, tokens[3]) + ";");
        return;
    }
    if(line_like("MODULO $number BY $number IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("MODULO statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[5]) + " = modulo(" + tokens[1] + ", " + tokens[3] + ");");
        return;
    }
    if(line_like("MODULO $number BY $num-var IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("MODULO statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[5]) + " = modulo(" + tokens[1] + ", " + get_c_variable(state, tokens[3]) + ");");
        return;
    }
    if(line_like("MODULO $num-var BY $number IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("MODULO statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[5]) + " = modulo(" + get_c_variable(state, tokens[1]) + ", " + tokens[3] + ");");
        return;
    }
    if(line_like("MODULO $num-var BY $num-var IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("MODULO statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[5]) + " = modulo(" + get_c_variable(state, tokens[1]) + ", " + get_c_variable(state, tokens[3]) + ");");
        return;
    }
    if(line_like("ABS $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("ABS statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[1]) + " = fabs("+get_c_variable(state, tokens[1])+");");
        return;
    }
    if(line_like("JOIN $string AND $string IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("JOIN statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("join(" + tokens[1] + ", " + tokens[3] + ", " + get_c_variable(state, tokens[5]) + ");");
        return;
    }
    if(line_like("JOIN $string AND $number IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("JOIN statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("join(" + tokens[1] + ", to_ldpl_string(" + tokens[3] + "), " + get_c_variable(state, tokens[5]) + ");");
        return;
    }
    if(line_like("JOIN $string AND $num-var IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("JOIN statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("join(" + tokens[1] + ", to_ldpl_string(" + get_c_variable(state, tokens[3]) + "), " + get_c_variable(state, tokens[5]) + ");");
        return;
    }
    if(line_like("JOIN $string AND $str-var IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("JOIN statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("join(" + tokens[1] + ", " + get_c_variable(state, tokens[3]) + ", " + get_c_variable(state, tokens[5]) + ");");
        return;
    }

    if(line_like("JOIN $number AND $string IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("JOIN statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("join(to_ldpl_string(" + tokens[1] + "), " + tokens[3] + ", " + get_c_variable(state, tokens[5]) + ");");
        return;
    }
    if(line_like("JOIN $number AND $number IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("JOIN statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("join(to_ldpl_string(" + tokens[1] + "), to_ldpl_string(" + tokens[3] + "), " + get_c_variable(state, tokens[5]) + ");");
        return;
    }
    if(line_like("JOIN $number AND $num-var IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("JOIN statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("join(to_ldpl_string(" + tokens[1] + "), to_ldpl_string(" + get_c_variable(state, tokens[3]) + "), " + get_c_variable(state, tokens[5]) + ");");
        return;
    }
    if(line_like("JOIN $number AND $str-var IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("JOIN statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("join(to_ldpl_string(" + tokens[1] + "), " + get_c_variable(state, tokens[3]) + ", " + get_c_variable(state, tokens[5]) + ");");
        return;
    }

    if(line_like("JOIN $str-var AND $string IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("JOIN statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("join(" +get_c_variable(state, tokens[1]) + ", " + tokens[3] + ", " + get_c_variable(state, tokens[5]) + ");");
        return;
    }
    if(line_like("JOIN $str-var AND $number IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("JOIN statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("join(" +get_c_variable(state, tokens[1]) + ", to_ldpl_string(" + tokens[3] + "), " + get_c_variable(state, tokens[5]) + ");");
        return;
    }
    if(line_like("JOIN $str-var AND $num-var IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("JOIN statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("join(" +get_c_variable(state, tokens[1]) + ", to_ldpl_string(" + get_c_variable(state, tokens[3]) + "), " + get_c_variable(state, tokens[5]) + ");");
        return;
    }
    if(line_like("JOIN $str-var AND $str-var IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("JOIN statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("join(" +get_c_variable(state, tokens[1]) + ", " + get_c_variable(state, tokens[3]) + ", " + get_c_variable(state, tokens[5]) + ");");
        return;
    }

    if(line_like("JOIN $num-var AND $string IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("JOIN statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("join(to_ldpl_string(" +get_c_variable(state, tokens[1]) + "), " + tokens[3] + ", " + get_c_variable(state, tokens[5]) + ");");
        return;
    }
    if(line_like("JOIN $num-var AND $number IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("JOIN statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("join(to_ldpl_string(" +get_c_variable(state, tokens[1]) + "), to_ldpl_string(" + tokens[3] + "), " + get_c_variable(state, tokens[5]) + ");");
        return;
    }
    if(line_like("JOIN $num-var AND $num-var IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("JOIN statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("join(to_ldpl_string(" +get_c_variable(state, tokens[1]) + "), to_ldpl_string(" + get_c_variable(state, tokens[3]) + "), " + get_c_variable(state, tokens[5]) + ");");
        return;
    }
    if(line_like("JOIN $num-var AND $str-var IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("JOIN statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("join(to_ldpl_string(" +get_c_variable(state, tokens[1]) + "), " + get_c_variable(state, tokens[3]) + ", " + get_c_variable(state, tokens[5]) + ");");
        return;
    }
    if(line_like("GET CHARACTER AT $num-var FROM $str-var IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("GET CHARACTER statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[7]) + " = charat(" + get_c_variable(state, tokens[5]) + ", " + get_c_variable(state, tokens[3]) + ");");
        return;
    }
    if(line_like("GET CHARACTER AT $number FROM $str-var IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("GET CHARACTER statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[7]) + " = charat(" + get_c_variable(state, tokens[5]) + ", " + tokens[3] + ");");
        return;
    }
    if(line_like("GET CHARACTER AT $num-var FROM $string IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("GET CHARACTER statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[7]) + " = charat(" + tokens[5] + ", " + get_c_variable(state, tokens[3]) + ");");
        return;
    }
    if(line_like("GET CHARACTER AT $number FROM $string IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("GET CHARACTER statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[7]) + " = charat(" + tokens[5] + ", " + tokens[3] + ");");
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
        //C Code
        state.add_code("void "+fix_identifier(tokens[1], false)+"(){");
        return;
    }
    if(line_like("RETURN", tokens, state))
    {
        if(state.section_state != 2)
            error("RETURN outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(state.open_subprocedure == "")
            error("RETURN found outside subprocedure (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("return;");
        return;
    }
    if(line_like("END SUB-PROCEDURE", tokens, state))
    {
        if(state.section_state != 2)
            error("END SUB-PROCEDURE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(state.open_subprocedure == "")
            error("END SUB-PROCEDURE found outside subprocedure (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("}");
        //Cierro la subrutina
        state.open_subprocedure = "";
        return;
    }

    if(line_like("IF $number IS EQUAL TO $number THEN", tokens, state))
    {
        if(state.section_state != 2)
            error("IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("if (num_equal(" + tokens[1] + ", " + tokens[5] + ")){");
        return;
    }
    if(line_like("IF $number IS EQUAL TO $num-var THEN", tokens, state))
    {
        if(state.section_state != 2)
            error("IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("if (num_equal(" + tokens[1] + ", " + get_c_variable(state, tokens[5]) + ")){");
        return;
    }
    if(line_like("IF $num-var IS EQUAL TO $number THEN", tokens, state))
    {
        if(state.section_state != 2)
            error("IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("if (num_equal(" + get_c_variable(state, tokens[1]) + ", " + tokens[5] + ")){");
        return;
    }
    if(line_like("IF $num-var IS EQUAL TO $num-var THEN", tokens, state))
    {
        if(state.section_state != 2)
            error("IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("if (num_equal(" + get_c_variable(state, tokens[1]) + ", " + get_c_variable(state, tokens[5]) + ")){");
        return;
    }

    if(line_like("IF $number IS NOT EQUAL TO $number THEN", tokens, state))
    {
        if(state.section_state != 2)
            error("IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("if (!num_equal(" + tokens[1] + ", " + tokens[6] + ")){");
        return;
    }
    if(line_like("IF $number IS NOT EQUAL TO $num-var THEN", tokens, state))
    {
        if(state.section_state != 2)
            error("IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("if (!num_equal(" + tokens[1] + ", " + get_c_variable(state, tokens[6]) + ")){");
        return;
    }
    if(line_like("IF $num-var IS NOT EQUAL TO $number THEN", tokens, state))
    {
        if(state.section_state != 2)
            error("IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("if (!num_equal(" + get_c_variable(state, tokens[1]) + ", " + tokens[6] + ")){");
        return;
    }
    if(line_like("IF $num-var IS NOT EQUAL TO $num-var THEN", tokens, state))
    {
        if(state.section_state != 2)
            error("IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("if (!num_equal(" + get_c_variable(state, tokens[1]) + ", " + get_c_variable(state, tokens[6]) + ")){");
        return;
    }

    if(line_like("IF $number IS GREATER THAN $number THEN", tokens, state))
    {
        if(state.section_state != 2)
            error("IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("if (" + tokens[1] + " > " + tokens[5] + "){");
        return;
    }
    if(line_like("IF $number IS GREATER THAN $num-var THEN", tokens, state))
    {
        if(state.section_state != 2)
            error("IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("if (" + tokens[1] + " > " + get_c_variable(state, tokens[5]) + "){");
        return;
    }
    if(line_like("IF $num-var IS GREATER THAN $number THEN", tokens, state))
    {
        if(state.section_state != 2)
            error("IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("if (" + get_c_variable(state, tokens[1]) + " > " + tokens[5] + "){");
        return;
    }
    if(line_like("IF $num-var IS GREATER THAN $num-var THEN", tokens, state))
    {
        if(state.section_state != 2)
            error("IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("if (" + get_c_variable(state, tokens[1]) + " > " + get_c_variable(state, tokens[5]) + "){");
        return;
    }

    if(line_like("IF $number IS LESS THAN $number THEN", tokens, state))
    {
        if(state.section_state != 2)
            error("IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("if (" + tokens[1] + " < " + tokens[5] + "){");
        return;
    }
    if(line_like("IF $number IS LESS THAN $num-var THEN", tokens, state))
    {
        if(state.section_state != 2)
            error("IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("if (" + tokens[1] + " < " + get_c_variable(state, tokens[5]) + "){");
    }
    if(line_like("IF $num-var IS LESS THAN $number THEN", tokens, state))
    {
        if(state.section_state != 2)
            error("IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("if (" + get_c_variable(state, tokens[1]) + " < " + tokens[5] + "){");
        return;
    }
    if(line_like("IF $num-var IS LESS THAN $num-var THEN", tokens, state))
    {
        if(state.section_state != 2)
            error("IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("if (" + get_c_variable(state, tokens[1]) + " < " + get_c_variable(state, tokens[5]) + "){");
        return;
    }

    if(line_like("IF $number IS GREATER THAN OR EQUAL TO $number THEN", tokens, state))
    {
        if(state.section_state != 2)
            error("IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("if (" + tokens[1] + " > " + tokens[8] + " || num_equal(" + tokens[1] + ", " + tokens[8] + ")){");
        return;
    }
    if(line_like("IF $number IS GREATER THAN OR EQUAL TO $num-var THEN", tokens, state))
    {
        if(state.section_state != 2)
            error("IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("if (" + tokens[1] + " > " + get_c_variable(state, tokens[8]) + " || num_equal(" + tokens[1] + ", " + get_c_variable(state, tokens[8]) + ")){");
        return;
    }
    if(line_like("IF $num-var IS GREATER THAN OR EQUAL TO $number THEN", tokens, state))
    {
        if(state.section_state != 2)
            error("IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("if (" + get_c_variable(state, tokens[1]) + " > " + tokens[8] + " || num_equal(" + get_c_variable(state, tokens[1]) + ", " + tokens[8] + ")){");
        return;
    }
    if(line_like("IF $num-var IS GREATER THAN OR EQUAL TO $num-var THEN", tokens, state))
    {
        if(state.section_state != 2)
            error("IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("if (" + get_c_variable(state, tokens[1]) + " > " + get_c_variable(state, tokens[8]) + " || num_equal(" + get_c_variable(state, tokens[1]) + ", " + get_c_variable(state, tokens[8]) + ")){");
        return;
    }

    if(line_like("IF $number IS LESS THAN OR EQUAL TO $number THEN", tokens, state))
    {
        if(state.section_state != 2)
            error("IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("if (" + tokens[1] + " < " + tokens[8] + " || num_equal(" + tokens[1] + ", " + tokens[8] + ")){");
        return;
    }
    if(line_like("IF $number IS LESS THAN OR EQUAL TO $num-var THEN", tokens, state))
    {
        if(state.section_state != 2)
            error("IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("if (" + tokens[1] + " < " + get_c_variable(state, tokens[8]) + " || num_equal(" + tokens[1] + ", " + get_c_variable(state, tokens[8]) + ")){");
        return;
    }
    if(line_like("IF $num-var IS LESS THAN OR EQUAL TO $number THEN", tokens, state))
    {
        if(state.section_state != 2)
            error("IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("if (" + get_c_variable(state, tokens[1]) + " < " + tokens[8] + " || num_equal(" + get_c_variable(state, tokens[1]) + ", " + tokens[8] + ")){");
        return;
    }
    if(line_like("IF $num-var IS LESS THAN OR EQUAL TO $num-var THEN", tokens, state))
    {
        if(state.section_state != 2)
            error("IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("if (" + get_c_variable(state, tokens[1]) + " < " + get_c_variable(state, tokens[8]) + " || num_equal(" + get_c_variable(state, tokens[1]) + ", " + get_c_variable(state, tokens[8]) + ")){");
        return;
    }

    if(line_like("IF $string IS EQUAL TO $string THEN", tokens, state))
    {
        if(state.section_state != 2)
            error("IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("if (" + tokens[1] + " == " + tokens[5] + "){");
        return;
    }
    if(line_like("IF $string IS EQUAL TO $str-var THEN", tokens, state))
    {
        //C Code
        state.add_code("if (" + tokens[1] + " == " + get_c_variable(state, tokens[5]) + "){");
        return;
    }
    if(line_like("IF $str-var IS EQUAL TO $string THEN", tokens, state))
    {
        if(state.section_state != 2)
            error("IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("if (" + get_c_variable(state, tokens[1]) + " == " + tokens[5] + "){");
        return;
    }
    if(line_like("IF $str-var IS EQUAL TO $str-var THEN", tokens, state))
    {
        if(state.section_state != 2)
            error("IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("if (" + get_c_variable(state, tokens[1]) + " == " + get_c_variable(state, tokens[5]) + "){");
        return;
    }

    if(line_like("IF $string IS NOT EQUAL TO $string THEN", tokens, state))
    {
        if(state.section_state != 2)
            error("IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("if (" + tokens[1] + " != " + tokens[6] + "){");
        return;
    }
    if(line_like("IF $string IS NOT EQUAL TO $str-var THEN", tokens, state))
    {
        if(state.section_state != 2)
            error("IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("if (" + tokens[1] + " != " + get_c_variable(state, tokens[6]) + "){");
        return;
    }
    if(line_like("IF $str-var IS NOT EQUAL TO $string THEN", tokens, state))
    {
        if(state.section_state != 2)
            error("IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("if (" + get_c_variable(state, tokens[1]) + " != " + tokens[6] + "){");
        return;
    }
    if(line_like("IF $str-var IS NOT EQUAL TO $str-var THEN", tokens, state))
    {
        if(state.section_state != 2)
            error("IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("if (" + get_c_variable(state, tokens[1]) + " != " + get_c_variable(state, tokens[6]) + "){");
        return;
    }

    if(line_like("END IF", tokens, state) || line_like("END-IF", tokens, state))
    {
        if(state.section_state != 2)
            error("END IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(state.open_ifs == 0)
            error("END IF without IF (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        --state.open_ifs;
        state.add_code("}");
        return;
    }

    if(line_like("WHILE $number IS EQUAL TO $number DO", tokens, state))
    {
        if(state.section_state != 2)
            error("WHILE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_while();
        state.add_code("while (num_equal(" + tokens[1] + ", " + tokens[5] + ")){");
        return;
    }
    if(line_like("WHILE $number IS EQUAL TO $num-var DO", tokens, state))
    {
        if(state.section_state != 2)
            error("WHILE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_while();
        state.add_code("while (num_equal(" + tokens[1] + ", " + get_c_variable(state, tokens[5]) + ")){");
        return;
    }
    if(line_like("WHILE $num-var IS EQUAL TO $number DO", tokens, state))
    {
        if(state.section_state != 2)
            error("WHILE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_while();
        state.add_code("while (num_equal(" + get_c_variable(state, tokens[1]) + ", " + tokens[5] + ")){");
        return;
    }
    if(line_like("WHILE $num-var IS EQUAL TO $num-var DO", tokens, state))
    {
        if(state.section_state != 2)
            error("WHILE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_while();
        state.add_code("while (num_equal(" + get_c_variable(state, tokens[1]) + ", " + get_c_variable(state, tokens[5]) + ")){");
        return;
    }

    if(line_like("WHILE $number IS NOT EQUAL TO $number DO", tokens, state))
    {
        if(state.section_state != 2)
            error("WHILE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_while();
        state.add_code("while (!num_equal(" + tokens[1] + ", " + tokens[6] + ")){");
        return;
    }
    if(line_like("WHILE $number IS NOT EQUAL TO $num-var DO", tokens, state))
    {
        if(state.section_state != 2)
            error("WHILE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_while();
        state.add_code("while (!num_equal(" + tokens[1] + ", " + get_c_variable(state, tokens[6]) + ")){");
        return;
    }
    if(line_like("WHILE $num-var IS NOT EQUAL TO $number DO", tokens, state))
    {
        if(state.section_state != 2)
            error("WHILE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_while();
        state.add_code("while (!num_equal(" + get_c_variable(state, tokens[1]) + ", " + tokens[6] + ")){");
        return;
    }
    if(line_like("WHILE $num-var IS NOT EQUAL TO $num-var DO", tokens, state))
    {
        if(state.section_state != 2)
            error("WHILE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_while();
        state.add_code("while (!num_equal(" + get_c_variable(state, tokens[1]) + ", " + get_c_variable(state, tokens[6]) + ")){");
        return;
    }

    if(line_like("WHILE $number IS GREATER THAN $number DO", tokens, state))
    {
        if(state.section_state != 2)
            error("WHILE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_while();
        state.add_code("while (" + tokens[1] + " > " + tokens[5] + "){");
        return;
    }
    if(line_like("WHILE $number IS GREATER THAN $num-var DO", tokens, state))
    {
        if(state.section_state != 2)
            error("WHILE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_while();
        state.add_code("while (" + tokens[1] + " > " + get_c_variable(state, tokens[5]) + "){");
        return;
    }
    if(line_like("WHILE $num-var IS GREATER THAN $number DO", tokens, state))
    {
        if(state.section_state != 2)
            error("WHILE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_while();
        state.add_code("while (" + get_c_variable(state, tokens[1]) + " > " + tokens[5] + "){");
        return;
    }
    if(line_like("WHILE $num-var IS GREATER THAN $num-var DO", tokens, state))
    {
        if(state.section_state != 2)
            error("WHILE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_while();
        state.add_code("while (" + get_c_variable(state, tokens[1]) + " > " + get_c_variable(state, tokens[5]) + "){");
        return;
    }

    if(line_like("WHILE $number IS LESS THAN $number DO", tokens, state))
    {
        if(state.section_state != 2)
            error("WHILE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_while();
        state.add_code("while (" + tokens[1] + " < " + tokens[5] + "){");
        return;
    }
    if(line_like("WHILE $number IS LESS THAN $num-var DO", tokens, state))
    {
        if(state.section_state != 2)
            error("WHILE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_while();
        state.add_code("while (" + tokens[1] + " < " + get_c_variable(state, tokens[5]) + "){");
        return;
    }
    if(line_like("WHILE $num-var IS LESS THAN $number DO", tokens, state))
    {
        if(state.section_state != 2)
            error("WHILE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_while();
        state.add_code("while (" + get_c_variable(state, tokens[1]) + " < " + tokens[5] + "){");
        return;
    }
    if(line_like("WHILE $num-var IS LESS THAN $num-var DO", tokens, state))
    {
        if(state.section_state != 2)
            error("WHILE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_while();
        state.add_code("while (" + get_c_variable(state, tokens[1]) + " < " + get_c_variable(state, tokens[5]) + "){");
        return;
    }

    if(line_like("WHILE $number IS GREATER THAN OR EQUAL TO $number DO", tokens, state))
    {
        if(state.section_state != 2)
            error("WHILE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_while();
        state.add_code("while (" + tokens[1] + " > " + tokens[8] + " || num_equal(" + tokens[1] + ", " + tokens[8] + ")){");
        return;
    }
    if(line_like("WHILE $number IS GREATER THAN OR EQUAL TO $num-var DO", tokens, state))
    {
        if(state.section_state != 2)
            error("WHILE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_while();
        state.add_code("while (" + tokens[1] + " > " + get_c_variable(state, tokens[8]) + " || num_equal(" + tokens[1] + ", " + get_c_variable(state, tokens[8]) + ")){");
        return;
    }
    if(line_like("WHILE $num-var IS GREATER THAN OR EQUAL TO $number DO", tokens, state))
    {
        if(state.section_state != 2)
            error("WHILE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_while();
        state.add_code("while (" + get_c_variable(state, tokens[1]) + " > " + tokens[8] + " || num_equal(" + get_c_variable(state, tokens[1]) + ", " + tokens[8] + ")){");
        return;
    }
    if(line_like("WHILE $num-var IS GREATER THAN OR EQUAL TO $num-var DO", tokens, state))
    {
        if(state.section_state != 2)
            error("WHILE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_while();
        state.add_code("while (" + get_c_variable(state, tokens[1]) + " > " + get_c_variable(state, tokens[8]) + " || num_equal(" + get_c_variable(state, tokens[1]) + ", " + get_c_variable(state, tokens[8]) + ")){");
        return;
    }

    if(line_like("WHILE $number IS LESS THAN OR EQUAL TO $number DO", tokens, state))
    {
        if(state.section_state != 2)
            error("WHILE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_while();
        state.add_code("while (" + tokens[1] + " < " + tokens[8] + " || num_equal(" + tokens[1] + ", " + tokens[8] + ")){");
        return;
    }
    if(line_like("WHILE $number IS LESS THAN OR EQUAL TO $num-var DO", tokens, state))
    {
        if(state.section_state != 2)
            error("WHILE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_while();
        state.add_code("while (" + tokens[1] + " < " + get_c_variable(state, tokens[8]) + " || num_equal(" + tokens[1] + ", " + get_c_variable(state, tokens[8]) + ")){");
        return;
    }
    if(line_like("WHILE $num-var IS LESS THAN OR EQUAL TO $number DO", tokens, state))
    {
        if(state.section_state != 2)
            error("WHILE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_while();
        state.add_code("while (" + get_c_variable(state, tokens[1]) + " < " + tokens[8] + " || num_equal(" + get_c_variable(state, tokens[1]) + ", " + tokens[8] + ")){");
        return;
    }
    if(line_like("WHILE $num-var IS LESS THAN OR EQUAL TO $num-var DO", tokens, state))
    {
        if(state.section_state != 2)
            error("WHILE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_while();
        state.add_code("while (" + get_c_variable(state, tokens[1]) + " < " + get_c_variable(state, tokens[8]) + " || num_equal(" + get_c_variable(state, tokens[1]) + ", " + get_c_variable(state, tokens[8]) + ")){");
        return;
    }

    if(line_like("WHILE $string IS EQUAL TO $string DO", tokens, state))
    {
        if(state.section_state != 2)
            error("WHILE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_while();
        state.add_code("while (" + tokens[1] + " == " + tokens[5] + "){");
        return;
    }
    if(line_like("WHILE $string IS EQUAL TO $str-var DO", tokens, state))
    {
        //C Code
        state.add_while();
        state.add_code("while (" + tokens[1] + " == " + get_c_variable(state, tokens[5]) + "){");
        return;
    }
    if(line_like("WHILE $str-var IS EQUAL TO $string DO", tokens, state))
    {
        if(state.section_state != 2)
            error("WHILE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_while();
        state.add_code("while (" + get_c_variable(state, tokens[1]) + " == " + tokens[5] + "){");
        return;
    }
    if(line_like("WHILE $str-var IS EQUAL TO $str-var DO", tokens, state))
    {
        if(state.section_state != 2)
            error("WHILE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_while();
        state.add_code("while (" + get_c_variable(state, tokens[1]) + " == " + get_c_variable(state, tokens[5]) + "){");
        return;
    }

    if(line_like("WHILE $string IS NOT EQUAL TO $string DO", tokens, state))
    {
        if(state.section_state != 2)
            error("WHILE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_while();
        state.add_code("while (" + tokens[1] + " != " + tokens[6] + "){");
        return;
    }
    if(line_like("WHILE $string IS NOT EQUAL TO $str-var DO", tokens, state))
    {
        if(state.section_state != 2)
            error("WHILE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_while();
        state.add_code("while (" + tokens[1] + " != " + get_c_variable(state, tokens[6]) + "){");
        return;
    }
    if(line_like("WHILE $str-var IS NOT EQUAL TO $string DO", tokens, state))
    {
        if(state.section_state != 2)
            error("WHILE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_while();
        state.add_code("while (" + get_c_variable(state, tokens[1]) + " != " + tokens[6] + "){");
        return;
    }
    if(line_like("WHILE $str-var IS NOT EQUAL TO $str-var DO", tokens, state))
    {
        if(state.section_state != 2)
            error("WHILE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_while();
        state.add_code("while (" + get_c_variable(state, tokens[1]) + " != " + get_c_variable(state, tokens[6]) + "){");
        return;
    }

    if(line_like("REPEAT", tokens, state))
    {
        if(state.section_state != 2)
            error("REPEAT outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(state.while_stack.size() == 0)
            error("REPEAT without WHILE (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.while_stack.pop();
        state.add_code("}");
        return;
    }

    if(line_like("BREAK", tokens, state))
    {
        if(state.section_state != 2)
            error("BREAK outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(state.while_stack.size() == 0)
            error("BREAK without WHILE (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("break;");
        return;
    }

    if(line_like("CONTINUE", tokens, state))
    {
        if(state.section_state != 2)
            error("CONTINUE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(state.while_stack.size() == 0)
            error("CONTINUE without WHILE (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("continue;");
        return;
    }

    if(line_like("CALL SUB-PROCEDURE $subprocedure", tokens, state))
    {
        if(state.section_state != 2)
            error("CALL SUB-PROCEDURE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(fix_identifier(tokens[2], false) + "();");
        return;
    }

    if(line_like("CALL EXTERNAL $external", tokens, state))
    {
        if(state.section_state != 2)
            error("CALL EXTERNAL outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        state.add_code(fix_external_identifier(tokens[2], false) + "();");
        //prototype of function defined in extension
        state.add_var_code("void "+fix_external_identifier(tokens[2], false)+"();");
        return;
    }

    if(line_like("EXECUTE $string", tokens, state))
    {
        if(state.section_state != 2)
            error("EXECUTE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("system(" + tokens[1] + ");");
        return;
    }
    if(line_like("EXECUTE $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("EXECUTE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("system(" + get_c_variable(state, tokens[1]) + ".c_str());");
        return;
    }
    if(line_like("EXECUTE $string AND STORE OUTPUT IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("EXECUTE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[6]) + " = exec(" + tokens[1] + ");");
        return;
    }
    if(line_like("EXECUTE $str-var AND STORE OUTPUT IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("EXECUTE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[6]) + " = exec(" + get_c_variable(state, tokens[1]) + ".c_str());");
        return;
    }
    if(line_like("EXECUTE $string AND STORE EXIT CODE IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("EXECUTE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[7]) + " = system(" + tokens[1] + ");");
        return;
    }
    if(line_like("EXECUTE $str-var AND STORE EXIT CODE IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("EXECUTE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[7]) + " = system(" + get_c_variable(state, tokens[1]) + ".c_str());");
        return;
    }
    if(line_like("EXIT", tokens, state))
    {
        if(state.section_state != 2)
            error("EXIT outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("exit(0);");
        return;
    }
    if(line_like("STORE LENGTH OF $str-var IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("EXECUTE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[5]) + " = str_len(" + get_c_variable(state, tokens[3]) + ");");
        return;
    }
    if(line_like("STORE LENGTH OF $string IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("EXECUTE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[5]) + " = str_len(" + tokens[3] + ");");
        return;
    }
    //Desde acá faltan en el standard
    if(line_like("STORE RANDOM IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("RANDOM outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[3]) + " = get_random();");
        return;
    }
    if(line_like("FLOOR $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("FLOOR statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[1]) + " = floor(" + get_c_variable(state, tokens[1]) +");");
        return;
    }
    if(line_like("CEIL $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("CEIL statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[1]) + " = ceil(" + get_c_variable(state, tokens[1]) +");");
        return;
    }
    if(line_like("STORE $number IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("STORE statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[3]) + " = to_ldpl_string(" + tokens[1] +");");
        return;
    }
    if(line_like("STORE $num-var IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("STORE statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[3]) + " = to_ldpl_string(" + get_c_variable(state, tokens[1]) +");");
        return;
    }
    if(line_like("STORE $string IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("STORE statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[3]) + " = to_number(" + tokens[1] +");");
        return;
    }
    if(line_like("STORE $str-var IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("STORE statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[3]) + " = to_number(" + get_c_variable(state, tokens[1]) +");");
        return;
    }

    if(line_like("STORE CHARACTER $number IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("STORE statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[4]) + " = (char)" + tokens[2] + ";");
        return;
    }
    if(line_like("STORE CHARACTER $num-var IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("STORE statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[4]) + " = (char)" + get_c_variable(state, tokens[2]) + ";");
        return;
    }

    if(line_like("STORE CHARACTER CODE OF $string IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("STORE statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[6]) + " = get_char_num(" + tokens[4] + ");");
        return;
    }
    if(line_like("STORE CHARACTER CODE OF $str-var IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("STORE statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[6]) + " = get_char_num(" + get_c_variable(state, tokens[4]) + ");");
        return;
    }

    if(line_like("STORE QUOTE IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("STORE statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        state.open_quote = true;
        //C code. More strings will get emitted 
        state.add_code(get_c_variable(state, tokens[3]) + " = \"\"");
        return;
    }

    if(line_like("END QUOTE", tokens, state))
        error("END QUOTE statement without preceding STORE QUOTE statement (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");

    //Undocumented
    if(line_like("IN $str-var JOIN $display", tokens, state))
    {
        if(state.section_state != 2)
            error("IN-JOIN statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(tokens.size() < 5)
            error("IN-JOIN expects at least two values to join (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("joinvar = \"\";");
        for(unsigned int i = 3; i < tokens.size(); ++i){
            if(is_num_var(tokens[i], state)){
                state.add_code("join(joinvar, to_ldpl_string(" + get_c_variable(state, tokens[i]) + "), joinvar);");
            }
            else if(is_txt_var(tokens[i], state)){
                state.add_code("join(joinvar, " + get_c_variable(state, tokens[i]) + ", joinvar);");
            }
            else if(is_number(tokens[i])){
                state.add_code("join(joinvar, to_ldpl_string(" + tokens[i] + "), joinvar);");
            }
            else{
                state.add_code("join(joinvar, " + tokens[i] + ", joinvar);");
            }
        }
        state.add_code(get_c_variable(state, tokens[1]) + " = joinvar;");
        return;
    }
    //REPLACE x FROM y WITH z IN w
    if(line_like("REPLACE $str-var FROM $str-var WITH $str-var IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("REPLACE statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        //TODO
        return;
    }
    if(line_like("REPLACE $string FROM $str-var WITH $str-var IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("REPLACE statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        //TODO
        return;
    }
    if(line_like("REPLACE $str-var FROM $string WITH $str-var IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("REPLACE statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        //TODO
        return;
    }
    if(line_like("REPLACE $string FROM $string WITH $str-var IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("REPLACE statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        //TODO
        return;
    }
    if(line_like("REPLACE $str-var FROM $str-var WITH $string IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("REPLACE statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        //TODO
        return;
    }
    if(line_like("REPLACE $string FROM $str-var WITH $string IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("REPLACE statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        //TODO
        return;
    }
    if(line_like("REPLACE $str-var FROM $string WITH $string IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("REPLACE statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        //TODO
        return;
    }
    if(line_like("REPLACE $string FROM $string WITH $string IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("REPLACE statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        //TODO
        return;
    }
    //GET INDEX OF x FROM y IN z
    if(line_like("GET INDEX OF $str-var FROM $str-var IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("GET INDEX OF statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        //TODO
        return;
    }
    if(line_like("GET INDEX OF $string FROM $str-var IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("GET INDEX OF statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        //TODO
        return;
    }
    if(line_like("GET INDEX OF $str-var FROM $string IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("GET INDEX OF statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        //TODO
        return;
    }
    if(line_like("GET INDEX OF $string FROM $string IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("GET INDEX OF statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        //TODO
        return;
    }
    //COUNT x FROM y IN z
    if(line_like("COUNT $str-var FROM $str-var IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("COUNT statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        //TODO
        return;
    }
    if(line_like("COUNT $string FROM $str-var IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("COUNT statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        //TODO
        return;
    }
    if(line_like("COUNT $str-var FROM $string IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("COUNT statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        //TODO
        return;
    }
    if(line_like("COUNT $string FROM $string IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("COUNT statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        //TODO
        return;
    }
    //CALL x
    if(line_like("CALL $subprocedure", tokens, state))
    {
        if(state.section_state != 2)
            error("CALL outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(fix_identifier(tokens[1], false) + "();");
        return;
    }
    //WRITE x TO FILE y
    if(line_like("WRITE $literal TO FILE $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("WRITE statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("file_writing_stream.open(" + get_c_variable(state, tokens[4]) + ", ios_base::out);");
        state.add_code("file_writing_stream <<" + tokens[1] + ";");
        state.add_code("file_writing_stream.close();");
        return;
    }
    if(line_like("WRITE $literal TO FILE $string", tokens, state))
    {
        if(state.section_state != 2)
            error("WRITE statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("file_writing_stream.open(" + tokens[4] + ", ios_base::out);");
        state.add_code("file_writing_stream <<" + tokens[1] + ";");
        state.add_code("file_writing_stream.close();");
        return;
    }
    if(line_like("WRITE $var TO FILE $string", tokens, state))
    {
        if(state.section_state != 2)
            error("WRITE statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("file_writing_stream.open(" + tokens[4] + ", ios_base::out);");
        state.add_code("file_writing_stream <<" + get_c_variable(state, tokens[1]) + ";");
        state.add_code("file_writing_stream.close();");
        return;
    }
    if(line_like("WRITE $var TO FILE $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("WRITE statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("file_writing_stream.open(" + get_c_variable(state, tokens[4]) + ", ios_base::out);");
        state.add_code("file_writing_stream <<" + get_c_variable(state, tokens[1]) + ";");
        state.add_code("file_writing_stream.close();");
        return;
    }
    //APPEND x TO FILE y
    if(line_like("APPEND $literal TO FILE $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("APPEND statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("file_writing_stream.open(" + get_c_variable(state, tokens[4]) + ", ios_base::app);");
        state.add_code("file_writing_stream <<" + tokens[1] + ";");
        state.add_code("file_writing_stream.close();");
        return;
    }
    if(line_like("APPEND $literal TO FILE $string", tokens, state))
    {
        if(state.section_state != 2)
            error("APPEND statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("file_writing_stream.open(" + tokens[4] + ", ios_base::app);");
        state.add_code("file_writing_stream <<" + tokens[1] + ";");
        state.add_code("file_writing_stream.close();");
        return;
    }
    if(line_like("APPEND $var TO FILE $string", tokens, state))
    {
        if(state.section_state != 2)
            error("APPEND statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("file_writing_stream.open(" + tokens[4] + ", ios_base::app);");
        state.add_code("file_writing_stream <<" + get_c_variable(state, tokens[1]) + ";");
        state.add_code("file_writing_stream.close();");
        return;
    }
    if(line_like("APPEND $var TO FILE $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("APPEND statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("file_writing_stream.open(" + get_c_variable(state, tokens[4]) + ", ios_base::app);");
        state.add_code("file_writing_stream <<" + get_c_variable(state, tokens[1]) + ";");
        state.add_code("file_writing_stream.close();");
        return;
    }
    //LOAD FILE x IN y
    if(line_like("LOAD FILE $string IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("LOAD FILE statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("load_file(" + tokens[2] + ", " + get_c_variable(state, tokens[4]) +");");
        return;
    }
    if(line_like("LOAD FILE $str-var IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("LOAD FILE statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("load_file(" + get_c_variable(state, tokens[2]) + ", " + get_c_variable(state, tokens[4]) +");");
        return;
    }
    //WAIT x MILLISECONDS
    if(line_like("WAIT $number MILLISECONDS", tokens, state))
    {
        if(state.section_state != 2)
            error("SLEEP statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("std::this_thread::sleep_for(std::chrono::milliseconds(" + tokens[1] + "));");
        return;
    }
    if(line_like("WAIT $num-var MILLISECONDS", tokens, state))
    {
        if(state.section_state != 2)
            error("SLEEP statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("std::this_thread::sleep_for(std::chrono::milliseconds((long int)" + get_c_variable(state, tokens[1]) + "));");
        return;
    }

    if(line_like("LABEL $label", tokens, state))
    {
        if(state.section_state != 2)
            error("LABEL outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        state.add_code(fix_identifier(tokens[1])+":");
        return;
    }
    if(line_like("GOTO $label", tokens, state))
    {
        if(state.section_state != 2)
            error("GOTO outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        state.add_code("goto "+fix_identifier(tokens[1])+";");
        return;
    }

    error("Malformed statement (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
}

//External identifiers are used by C++ extensions and thus have a simpler but
//more restrictive name mangling algorithm: The only characters allowed are 
//`A-Z`, `0-9`, and `_`. All other characters are converted to `_`. 
string fix_external_identifier(string identifier, bool isVariable){
    const string validChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890:";
    string new_id;
    for(unsigned int i = 0; i < identifier.size(); ++i){ 
        if(validChars.find(identifier[i]) != string::npos){
            new_id += identifier[i];
        }else{
            new_id += "_";
        }
    }
    return new_id;
}

//when state is provided to fix_identifier it can fix external variables too.
string fix_identifier(string ident, bool isVar, compiler_state & state){
    if(is_external(ident, state)){
        return fix_external_identifier(ident, isVar);
    }else{
        return fix_identifier(ident, isVar);
    }
}

string fix_identifier(string identifier, bool isVariable){
    return string(isVariable ? "VAR_" : "SUBPR_") + fix_identifier(identifier);
}

string fix_identifier(string identifier){
    const string validChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890:";
    ostringstream new_id;
    for(unsigned int i = 0; i < identifier.size(); ++i){ 
        if(validChars.find(identifier[i]) != string::npos){
            new_id << identifier[i];
        }else{
            new_id << "c" << (unsigned int)identifier[i] << "_";
        }
    }
    return new_id.str();
}

//Check if the tokens of a line passed are like the ones of a model line
bool line_like(string model_line, vector<string> & tokens, compiler_state & state)
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
            for(char letter : tokens[i]) if(letter == ':') return false;
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
        else if(model_tokens[i] == "$var") //$var is either a NUMBER variable or a TEXT variable
        {
            if(!is_num_var(tokens[i], state) && !is_txt_var(tokens[i], state)) return false;
        }
        else if(model_tokens[i] == "$literal") //$literal is either a NUMBER or a TEXT
        {
            if(!is_string(tokens[i]) && !is_number(tokens[i])) return false;
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
        else if(model_tokens[i] == "$display") //multiple NUMBER, TEXT, TEXT-VAR, NUMBER-VAR
        {
            for(; i < tokens.size(); ++i){
                if(!is_string(tokens[i])
                && !is_number(tokens[i])
                && !is_variable(tokens[i], state))
                    return false;
            }
        }
        else if(model_tokens[i] == "$subprocedure") //$subprocedure is a SUB-PROCEDURE (?
        {
            if(!is_subprocedure(tokens[i], state)) return false;
        }
        else if(model_tokens[i] == "$external") //$external is a C++ function defined elsewhere
        {
            return !is_subprocedure(tokens[i], state) && !is_variable(tokens[i], state) && 
                   !is_string(tokens[i]) && !is_number(tokens[i]);
        }
        else if(model_tokens[i] == "$label") //$label is a GOTO label
        {
            return is_label(tokens[i]);
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

bool is_label(string & token){
    return !isdigit(token[0]) && token[0] != ':' && token[0] != '"';
}

bool is_string(string & token){
    return (token.size() >= 2 && token[0] == '"' && token[token.size()-1] == '"');
}

bool is_vector_index(string & token)
{
    return token.size() >= 2 && token[0] != '"' && token[token.size()-1] == '"';
}

bool is_num_vector(string & token, compiler_state & state)
{
    if(state.variables.count(token) > 0 && state.variables[token] == 3) return true;
    return false;
}

bool is_txt_vector(string & token, compiler_state & state)
{
    if(state.variables.count(token) > 0 && state.variables[token] == 4) return true;
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
        if(is_vector(token.front(), state)) return false;
        return true;
    }
    return false;
}

bool is_num_var(string & token, compiler_state & state)
{
    //Veo si var
    if(state.variables.count(token) > 0 && state.variables[token] == 1) return true;
    //Veo si num_vector index
    queue<string> vpart;
    split_vector(token, vpart);
    return is_num_vector(vpart.front(), state) && is_vector_index(vpart, state);
}

bool is_txt_var(string & token, compiler_state & state)
{
    if(state.variables.count(token) > 0 && state.variables[token] == 2) return true;
    //Veo si num_vector index
    queue<string> vpart;
    split_vector(token, vpart);
    return is_txt_vector(vpart.front(), state) && is_vector_index(vpart, state);
}

bool is_variable(string & token, compiler_state & state)
{
    return is_num_var(token, state) || is_txt_var(token, state);
}

bool is_external(string & token, compiler_state & state)
{
    return state.externals[token];
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
                switch(next_letter)
                {
                    case '\\': case '"': case '0':
                    case 'a': case 'b': case 't': case 'n':
                    case 'v': case 'f': case 'r': case 'e':
                        current_token += "\\" + string(1, next_letter);
                    break;
                    default:
                        error("unknown escape sequence on a VECTOR access `" + current_token + "` in: " + line);
                }
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
    return state.variables.count(token) > 0;
}

bool is_subprocedure(string & token, compiler_state & state)
{
    for(string & var : state.subprocedures)
        if(var == token) return true;
    return false;
}

string get_c_variable(compiler_state & state, string & variable)
{

    //Single variable
    queue<string> vpart;
    split_vector(variable, vpart);
    if(vpart.size() == 1){
        return fix_identifier(variable, true, state);
    }

    //Vector variable
    vector<string> token;
    while(!vpart.empty())
    {
        token.push_back(vpart.front());
        vpart.pop();
    }

    //Last element of vector access:
    string newvar = fix_identifier(token[0], true, state);

    for(unsigned int i = 1; i < token.size(); ++i){
        newvar += "[";
        if(is_variable(token[i], state) || is_num_vector(token[i], state) || is_txt_vector(token[i], state))
        //Pongo esto porque el is_variable requiere que tenga subindices y acá le paso solo el nombre del vector
            newvar += fix_identifier(token[i], true, state);
        else
            newvar += token[i];
    }
    for(unsigned int i = 1; i < token.size(); ++i){
        newvar += "]";
    }

    return newvar;
}

//Escapes " char in string so it can be emitted as c++
string & escape_c_quotes(string & str)
{
    for(unsigned int i = 0; i < str.size(); ++i){
        if(str[i] == '"'){
            str.erase(i, 1);
            str.insert(i, "\\\"");
            ++i;
        }
    }
    return str;
}
