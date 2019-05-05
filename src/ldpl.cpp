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
        cout << endl;
        cout << " This is \033[32;1mLDPL version " << VERSION << "\033[0m '\033[32;1m" << VERSIONNAME << "\033[0m'." << endl << endl;
        cout << " Copyright 2018-2019, \033[35;1mMartín del Río\033[0m (www.lartu.net)." << endl;
        cout << " Built with amazing contributions from \033[35;1mʇʞʌp\033[0m, \033[35;1mDamián Garro\033[0m and" << endl;
        cout << " a bunch of other contributors." << endl << endl;
        cout << " The LDPL Home Page can be found at \033[36;1mwww.ldpl-lang.org\033[0m." << endl;
        cout << " The LDPL source code is available at \033[36;1mwww.github.com/lartu/ldpl\033[0m." << endl << endl;
        cout << " Complete documentation for LDPL should be found on this system" << endl;
        cout << " using '\033[33;1mman ldpl\033[0m'. If you have access to the internet, the"<< endl;
        cout << " documentation can also be found online at \033[36;1mdocs.ldpl-lang.org\033[0m." << endl << endl;
        cout << " LDPL may be copied only under the terms of the GNU General" << endl;
        cout << " Public License 3.0, which may be found in the LDPL repository." << endl <<endl;
        cout << " Compiled on \033[31;1m" << COMPILEDATE << "\033[0m at \033[31;1m" << COMPILEHOUR << "\033[0m." << endl;
        cout << endl;
        return 0;
    }
    else if(contains_any(args, {"-h", "--help"})){
        cout << endl;
        cout << " \033[33;1mUsage:\033[0m" << endl;
        cout << "    ldpl [-i='<included file>']... <source file>|-c" << endl;
        cout << "         [-o='<output name>'|-r] [-f='<c++ flag>']... [-n]" << endl;
        cout << "    ldpl [-v|-h]" << endl;
        cout << endl;
        cout << " \033[33;1mOptions:\033[0m" << endl;
        cout << "   -v --version             Display LDPL version information" << endl;
        cout << "   -h --help                Display this information" << endl;
        cout << "   -r                       Display generated C++ code" << endl;
        cout << "   -o=<name>                Set output file for compiled binary" << endl;
        cout << "   -i=<file>                Include file in current compilation" << endl;
        cout << "   -f=<flag>                Pass a flag to the C++ compiler" << endl;
		#ifdef  STATIC_BUILDS
		cout << "   -n --non-static          Disables static binary building" << endl;
		#endif
        cout << "   -c                       Compile from standard input" << endl;
        cout << endl;
        cout << " Complete documentation for LDPL should be found on this system" << endl;
        cout << " using '\033[33;1mman ldpl\033[0m'. If you have access to the internet, the"<< endl;
        cout << " documentation can also be found online at \033[36;1mdocs.ldpl-lang.org\033[0m." << endl;
        cout << endl;
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
            else if(arg == "-n" || arg == "--non-static"){
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
            else if(arg == "-c"){
                files_to_compile.push_back(arg);
            }
            else{
                cout << "Unknown option: " << arg << endl;
                cout << "Try 'ldpl -h' for more information." << endl;
                return 0;
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
    state.variables["ERRORCODE"] = 1; //Declared in ldpl_lib.cpp
    state.variables["ERRORTEXT"] = 2; //Declared in ldpl_lib.cpp
    state.add_code("for(int i = 1; i < argc; ++i)");
    state.add_code(fix_identifier("ARGV", true) + "[i-1] = argv[i];");
    state.add_code(fix_identifier("ARGC", true) + " = argc - 1;");

    //Fail if file was not passed
    if(files_to_compile.size() == 0) error("Filename expected.");

    //For each file, compile each file into one big code
    for(string & filename : files_to_compile)
    {
        //Reset state section for this file
        state.section_state = 0;
        if(filename != "-c"){
            state.current_file = filename;
            load_and_compile(filename, state);
        }else{
            state.current_file = "standard input";
            accept_and_compile(state);
        }
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
    //Accept input from stdin
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

void accept_and_compile(compiler_state & state){
    //Get file contents
    vector<string> lines;
    string line = "";
    while(getline(cin, line))
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
        tokenize(line, line_num, tokens, state.current_file, true);
        for(string & token : tokens) if(token == "CRLF") token = "\"\\r\\n\"";
        if(tokens.size() == 0) continue;
        compile_line(tokens, line_num, state);
    }
    if(state.open_quote) error("your QUOTE block was not terminated.");
    if(state.closing_subprocedure()) error("your SUB-PROCEDURE block was not terminated.");
    if(state.closing_if()) error("your IF block was not terminated.");
    if(state.closing_while()) error("your WHILE block was not terminated.");
}

//Tokenizes a line with optional convertion of tokens to uppercase (except in string)
void tokenize(string & line, unsigned int line_num, vector<string> & tokens, string & current_file, bool uppercase)
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
        else if(letter == '\\' && in_string)
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
            else error("\\ found alone on a string (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
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
            current_token += (uppercase && !in_string)? toupper(letter) : letter;
        }
        if(i == line.size() - 1 && letter != ' ')
        {
            if(in_string) error("Unterminated string (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
            if(current_token.size() > 0)
                    tokens.push_back(current_token);
        }
    }
}

//Compiles line per line
void compile_line(vector<string> & tokens, unsigned int line_num, compiler_state & state)
{
    string & current_file = state.current_file;
    ++line_num;

    // Sections
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

    // Variable Declaration
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
    }
    if(line_like("$name IS EXTERNAL NUMBER", tokens, state))
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

    // SUB-PROCEDURE Declaration
    if(line_like("SUB-PROCEDURE $name", tokens, state))
    {
        if(state.section_state != 2)
            error("SUB-PROCEDURE declaration outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(!is_subprocedure(tokens[1], state))
            state.subprocedures.push_back(tokens[1]);
        else
            error("Duplicate declaration for subprocedure " + tokens[1] + " (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(state.closing_subprocedure())
            error("Subprocedure declaration inside subprocedure (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        else if(state.closing_if())
            error("Subprocedure declaration inside IF (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        else if(state.closing_while())
            error("Subprocedure declaration inside WHILE (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        else
            state.open_subprocedure();
        //C Code
        state.add_code("void "+fix_identifier(tokens[1], false)+"(){");
        return;
    }
    if(line_like("EXTERNAL SUB-PROCEDURE $external", tokens, state))
    {
        if(state.section_state != 2)
            error("EXTERNAL SUB-PROCEDURE declaration outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(state.closing_subprocedure())
            error("Subprocedure declaration inside subprocedure (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        else if(state.closing_if())
            error("Subprocedure declaration inside IF (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        else if(state.closing_while())
            error("Subprocedure declaration inside WHILE (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        else
            state.open_subprocedure();
        //C Code
        state.add_code("void "+fix_external_identifier(tokens[2], false)+"(){");
        return;
    }
    if(line_like("END SUB-PROCEDURE", tokens, state))
    {
        if(state.section_state != 2)
            error("END SUB-PROCEDURE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(!state.closing_subprocedure())
            error("END SUB-PROCEDURE without SUB-PROCEDURE (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("}");
        //Cierro la subrutina
        state.close_subprocedure();
        return;
    }

    // Control Flow Statements
    if(line_like("STORE $expression IN $var", tokens, state))
    {
        if(state.section_state != 2)
        error("STORE statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        string rhand;
        if (is_num_var(tokens[3], state))
        rhand = get_c_number(state, tokens[1]);
        else
        rhand = get_c_string(state, tokens[1]);
        state.add_code(get_c_variable(state, tokens[3]) + " = " + rhand + ";");
        return;
    }
    if(line_like("IF $condition THEN", tokens, state))
    {
        if(state.section_state != 2)
            error("IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.open_if();
        state.add_code("if (" + get_c_condition(state, vector<string>(tokens.begin()+1, tokens.end()-1)) + "){");
        return;
    }
    if(line_like("ELSE IF $condition THEN", tokens, state))
    {
        if(state.section_state != 2)
            error("ELSE IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(!state.closing_if())
            error("ELSE IF without IF (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("} else if (" + get_c_condition(state, vector<string>(tokens.begin()+2, tokens.end()-1)) + "){");
        return;
    }
    if(line_like("ELSE", tokens, state))
    {
        if(state.section_state != 2)
            error("ELSE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(!state.closing_if())
            error("ELSE without IF (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.open_else();
        state.add_code("}else{");
        return;
    }
    if(line_like("END IF", tokens, state) || line_like("END-IF", tokens, state))
    {
        if(state.section_state != 2)
            error("END IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(!state.closing_if() && !state.closing_else())
            error("END IF without IF (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.close_if();
        state.add_code("}");
        return;
    }
    if(line_like("WHILE $condition DO", tokens, state))
    {
        if(state.section_state != 2)
            error("WHILE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.open_while();
        state.add_code("while (" + get_c_condition(state, vector<string>(tokens.begin()+1, tokens.end()-1)) + "){");
        return;
    }
    if(line_like("REPEAT", tokens, state))
    {
        if(state.section_state != 2)
            error("REPEAT outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(!state.closing_while())
            error("REPEAT without WHILE (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.close_while();
        state.add_code("}");
        return;
    }
    if(line_like("BREAK", tokens, state))
    {
        if(state.section_state != 2)
            error("BREAK outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(state.open_whiles == 0)
            error("BREAK without WHILE (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("break;");
        return;
    }
    if(line_like("CONTINUE", tokens, state))
    {
        if(state.section_state != 2)
            error("CONTINUE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(state.open_whiles == 0)
            error("CONTINUE without WHILE (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("continue;");
        return;
    }
    if(line_like("CALL $subprocedure", tokens, state))
    {
        if(state.section_state != 2)
            error("CALL outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(fix_identifier(tokens[1], false) + "();");
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
    if(line_like("RETURN", tokens, state))
    {
        if(state.section_state != 2)
            error("RETURN outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(!state.in_subprocedure)
            error("RETURN found outside subprocedure (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("return;");
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
    if(line_like("WAIT $num-expr MILLISECONDS", tokens, state))
    {
        if(state.section_state != 2)
            error("WAIT statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("std::this_thread::sleep_for(std::chrono::milliseconds((long int)" + get_c_expression(state, tokens[1]) + "));");
        return;
    }
    if(line_like("GOTO $label", tokens, state))
    {
        if(state.section_state != 2)
            error("GOTO outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        state.add_code("goto label_"+fix_identifier(tokens[1])+";");
        return;
    }
    if(line_like("LABEL $label", tokens, state))
    {
        if(state.section_state != 2)
            error("LABEL outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        state.add_code("label_"+fix_identifier(tokens[1])+":");
        return;
    }

    // Arithmetic Statements
    if(line_like("ADD $num-expr AND $num-expr IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("ADD statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[5]) + " = " + get_c_expression(state, tokens[1]) + " + " + get_c_expression(state, tokens[3]) + ";");
        return;
    }
    if(line_like("SUBTRACT $num-expr FROM $num-expr IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("SUBTRACT statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[5]) + " = " + get_c_expression(state, tokens[3]) + " - " + get_c_expression(state, tokens[1]) + ";");
        return;
    }
    if(line_like("MULTIPLY $num-expr BY $num-expr IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("MULTIPLY statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[5]) + " = " + get_c_expression(state, tokens[1]) + " * " + get_c_expression(state, tokens[3]) + ";");
        return;
    }
    if(line_like("DIVIDE $num-expr BY $num-expr IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("DIVIDE statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[5]) + " = " + get_c_expression(state, tokens[1]) + " / " + get_c_expression(state, tokens[3]) + ";");
        return;
    }
    if(line_like("MODULO $num-expr BY $num-expr IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("MODULO statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[5]) + " = modulo(" + get_c_expression(state, tokens[1]) + ", " + get_c_expression(state, tokens[3]) + ");");
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
    if(line_like("IN $num-var SOLVE $math", tokens, state))
    {
        if(state.section_state != 2)
            error("IN-SOLVE statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");

        string code = "";
        for(unsigned int i = 3; i < tokens.size(); ++i){
            if(is_num_var(tokens[i], state))
                code += " " + get_c_variable(state, tokens[i]);
            else if(is_txt_expr(tokens[i], state))
                code += " " + get_c_number(state, tokens[i]);
            else
                code += " " + tokens[i];
        }
        state.add_code(get_c_variable(state, tokens[1]) + " =" + code + ";");
        return;
    }

    // Text Statements
    if(line_like("JOIN $expression AND $expression IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("JOIN statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("join(" + get_c_string(state, tokens[1]) + ", " + get_c_string(state, tokens[3]) + ", " + get_c_variable(state, tokens[5]) + ");");
        return;
    }
    if(line_like("GET CHARACTER AT $num-expr FROM $str-expr IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("GET CHARACTER statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[7]) + " = charat(" + get_c_expression(state, tokens[5]) + ", " + get_c_expression(state, tokens[3]) + ");");
        return;
    }
    if(line_like("STORE LENGTH OF $str-expr IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("EXECUTE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[5]) + " = str_len(" + get_c_expression(state, tokens[3]) + ");");
        return;
    }
    if(line_like("STORE CHARACTER $num-expr IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("STORE statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[4]) + " = (char)" + get_c_expression(state, tokens[2]) + ";");
        return;
    }
    if(line_like("STORE CHARACTER CODE OF $str-expr IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("STORE statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[6]) + " = get_char_num(" + get_c_expression(state, tokens[4]) + ");");
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
    if(line_like("IN $str-var JOIN $display", tokens, state))
    {
        if(state.section_state != 2)
            error("IN-JOIN statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(tokens.size() < 5)
            error("IN-JOIN expects at least two values to join (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("joinvar = \"\";");
        for(unsigned int i = 3; i < tokens.size(); ++i){
            state.add_code("join(joinvar, " + get_c_string(state, tokens[i]) + ", joinvar);");
        }
        state.add_code(get_c_variable(state, tokens[1]) + " = joinvar;");
        return;
    }

    // I/O Statements
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
    if(line_like("ACCEPT $var", tokens, state))
    {
        if(state.section_state != 2)
            error("ACCEPT statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        if(is_num_var(tokens[1], state))
            state.add_code(get_c_variable(state, tokens[1]) + " = input_number();");
        else
            state.add_code(get_c_variable(state, tokens[1]) + " = input_string();");
        return;
    }
    if(line_like("EXECUTE $str-expr", tokens, state))
    {
        if(state.section_state != 2)
            error("EXECUTE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("system(" + get_c_char_array(state, tokens[1]) + ");");
        return;
    }
    if(line_like("EXECUTE $str-expr AND STORE OUTPUT IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("EXECUTE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[6]) + " = exec(" + get_c_char_array(state, tokens[1]) + ");");
        return;
    }
    if(line_like("EXECUTE $str-expr AND STORE EXIT CODE IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("EXECUTE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[7]) + " = system(" + get_c_char_array(state, tokens[1]) + ");");
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
    if(line_like("LOAD FILE $str-expr IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("LOAD FILE statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("load_file(" + get_c_expression(state, tokens[2]) + ", " + get_c_variable(state, tokens[4]) +");");
        return;
    }
    if(line_like("WRITE $expression TO FILE $str-expr", tokens, state))
    {
        if(state.section_state != 2)
            error("WRITE statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("file_writing_stream.open(" + get_c_expression(state, tokens[4]) + ", ios_base::out);");
        state.add_code("file_writing_stream <<" + get_c_expression(state, tokens[1]) + ";");
        state.add_code("file_writing_stream.close();");
        return;
    }
    if(line_like("APPEND $expression TO FILE $str-expr", tokens, state))
    {
        if(state.section_state != 2)
            error("APPEND statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("file_writing_stream.open(" + get_c_expression(state, tokens[4]) + ", ios_base::app);");
        state.add_code("file_writing_stream <<" + get_c_expression(state, tokens[1]) + ";");
        state.add_code("file_writing_stream.close();");
        return;
    }

/*  TODO: Implement
    if(line_like("REPLACE $str-var FROM $str-expr WITH $str-expr IN $str-var", tokens, state))
    {
        if(state.section_state != 2)
            error("REPLACE statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        //TODO
        return;
    }
    if(line_like("GET INDEX OF $str-expr FROM $str-expr IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("GET INDEX OF statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        //TODO
        return;
    }
    if(line_like("COUNT $str-expr FROM $str-expr IN $num-var", tokens, state))
    {
        if(state.section_state != 2)
            error("COUNT statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        //TODO
        return;
    }
    if(line_like("SPLIT $str-expr BY $str-expr IN $str-vector", tokens, state))
    {
        if(state.section_state != 2)
            error("SPLIT statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        //TODO
        return;
    }
*/

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
    tokenize(model_line, 0, model_tokens, state.current_file, false);
    //Check that tokens match between line and model line
    if(tokens.size() < model_tokens.size()) return false;
    unsigned int i = 0;
    unsigned int j = 0;
    for(; i < model_tokens.size(); ++i)
    {
        if(model_tokens[i] == "$name") //$name is a valid identifier for a variable or a sub-procedure
        {
            for(char letter : tokens[j]) if(letter == ':') return false;
            for(char letter : tokens[j]) if(letter == '\"') return false;
            for(char letter : tokens[j]) if(letter == '(') return false;
            for(char letter : tokens[j]) if(letter == ')') return false;
            if(is_number(tokens[j])) return false;
            if(tokens[j] == "+") return false;
            if(tokens[j] == "-") return false;
            if(tokens[j] == "*") return false;
            if(tokens[j] == "/") return false;
        }
        else if(model_tokens[i] == "$num-var") //$num-var is NUMBER variable
        {
            if(!is_num_var(tokens[j], state)) return false;
        }
        else if(model_tokens[i] == "$str-var") //$str-var is TEXT variable
        {
            if(!is_txt_var(tokens[j], state)) return false;
        }
        else if(model_tokens[i] == "$var") //$var is either a NUMBER variable or a TEXT variable
        {
            if(!is_variable(tokens[j], state)) return false;
        }
        else if(model_tokens[i] == "$literal") //$literal is either a NUMBER or a TEXT
        {
            if(!is_string(tokens[j]) && !is_number(tokens[j])) return false;
        }
        else if(model_tokens[i] == "$string") //$string is a string (?
        {
            if(!is_string(tokens[j])) return false;
        }
        else if(model_tokens[i] == "$number") //$number is a number (?
        {
            if(!is_number(tokens[j])) return false;
        }
        else if(model_tokens[i] == "$expression") //$expression is NUMBER, TEXT, TEXT-VAR, NUMBER-VAR
        {
            if(!is_expression(tokens[j], state)) return false;
        }
        else if(model_tokens[i] == "$str-expr") //$str-expr is either a TEXT or a TEXT variable
        {
            if(!is_txt_expr(tokens[j], state)) return false;
        }
        else if(model_tokens[i] == "$num-expr") //$num-expr is either a NUMBER or a NUMBER variable
        {
            if(!is_num_expr(tokens[j], state)) return false;
        }
        else if(model_tokens[i] == "$natural") //$natural is an integer greater than 0
        {
            if(!is_natural(tokens[j])) return false;
        }
        else if(model_tokens[i] == "$display") //multiple NUMBER, TEXT, TEXT-VAR, NUMBER-VAR
        {
            for(; j < tokens.size(); ++j){
                if(!is_expression(tokens[j], state))
                    return false;
            }
        }
        else if(model_tokens[i] == "$subprocedure") //$subprocedure is a SUB-PROCEDURE (?
        {
            if(!is_subprocedure(tokens[j], state)) return false;
        }
        else if(model_tokens[i] == "$external") //$external is a C++ function defined elsewhere
        {
            return !is_subprocedure(tokens[j], state) && !is_expression(tokens[j], state);
        }
        else if(model_tokens[i] == "$label") //$label is a GOTO label
        {
            return is_label(tokens[j]);
        }
        else if(model_tokens[i] == "$math") //$math is a math expression
        {
            vector<string> maths; //further tokenize math expressions
            string math_token = "";
            for(; j < tokens.size(); ++j){
                for(unsigned int z = 0; z < tokens[j].size(); ++z){
                    if(tokens[j][z] == '(' || tokens[j][z] == ')'){
                        if(!math_token.empty()) maths.push_back(math_token);
                        math_token = tokens[j][z];
                        maths.push_back(math_token);
                        math_token = "";
                    }else{
                        math_token += tokens[j][z];
                    }
                }
                if(!math_token.empty()) maths.push_back(math_token);
                math_token = "";
            }
            //replace LDPL line tokens with new math tokens
            tokens.erase(tokens.begin()+i, tokens.end());
            tokens.insert(tokens.end(), maths.begin(), maths.end());

            //validate the new tokens
            for(unsigned int z = i; z < tokens.size(); ++z){
                if(!is_math_symbol(tokens[z]) && !is_expression(tokens[z], state))
                    return false;
            }
            return true;
        }
        else if(model_tokens[i] == "$condition") //$condition is a IF/WHILE condition
        {
            // Note: We assume that there is only one token after $condition,
            // which is always the case in IF and WHILE statements
            string first_value = tokens[j];
            string second_value = tokens.rbegin()[1];
            bool text_values;
            if(is_num_expr(first_value, state) && is_num_expr(second_value, state))
                text_values = false;
            else if(is_txt_expr(first_value, state) && is_txt_expr(second_value, state))
                text_values = true;
            else
                return false;

            if(tokens[++j] != "IS") return false;

            string rel_op;
            for(++j; j < tokens.size() - 2; ++j){
                rel_op += tokens[j] + " ";
            }
            if(rel_op != "EQUAL TO " && rel_op != "NOT EQUAL TO "){
                if(text_values) return false;
                if(rel_op != "GREATER THAN " && rel_op != "GREATER THAN OR EQUAL TO "
                && rel_op != "LESS THAN " && rel_op != "LESS THAN OR EQUAL TO ")
                    return false;
            }
        }
        else if(model_tokens[i] != tokens[j]) return false;
        ++j;
    }
    if(j < tokens.size()) return false;
    return true;
}

bool is_number(string & number){
    unsigned int firstchar = 0;
    if(number[0] == '-') firstchar = 1;
    if(number[firstchar] == '.') return false; //.12 is not a valid decimal in LDPL, 0.12 is.
    if(number[firstchar] == '+') return false; //+5 is not a valid decimal in LDPL, 5 is.
    istringstream iss(number);
    double f;
    iss >> f;
    bool isNumber = iss.eof() && !iss.fail();
    //If it is a number, it might be an octal literal (e.g. 025, 067, 003 etc)
    //so we proceed to fix the original number to make it decimal.
    if(isNumber){
        string f_number = "";
        unsigned int i;
        for(i = 1; i < number.length(); ++i){
            //If prev char not 0
            if(number[i - 1] != '0'){
                //if prev char is -, continue check
                if(number[i - 1] == '-') f_number += '-';
                //if prev char is number, break
                else break;
            }
            //If prev number is 0
            else if(number[i] == '.') {
                f_number += '0';
            }
        }
        f_number += number.substr(i - 1);
        number = f_number;
        return true;
    }else{
        return false;
    }
}

bool is_natural(string number){
    if(!is_number(number)) return false;
    if(stod(number) <= 0) return false;
    for(char l : number)
        if(l == '.') return false;
    return true;
}

bool is_label(string & token){
    //return !isdigit(token[0]) && token[0] != ':' && token[0] != '"';
    for(char letter : token) if(letter == '\"') return false;
    return true;
}

bool is_math_symbol(string & token){
    string syms = "+-*/()";
    return token.size() == 1 && syms.find(token[0]) != string::npos;
}

bool is_string(string & token){
    if(token.size() < 2 || token[0] != '"' || token[token.size()-1] != '"')
        return false;
    // Check for unescaped quotes
    for(unsigned int i = 1; i < token.size() - 1; ++i) {
        if (token[i] == '\"' && token[i-1] != '\\') return false;
    }
    return true;
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

bool is_num_var(string & token, compiler_state & state)
{
    //Check if var
    if(state.variables.count(token) > 0 && state.variables[token] == 1) return true;
    //Check if num_vector:index
    string vector, index;
    split_vector(token, vector, index);
    return is_num_vector(vector, state) && is_expression(index, state);
}

bool is_txt_var(string & token, compiler_state & state)
{
    //Check if var
    if(state.variables.count(token) > 0 && state.variables[token] == 2) return true;
    //Check if txt_vector:index
    string vector, index;
    split_vector(token, vector, index);
    return is_txt_vector(vector, state) && is_expression(index, state);
}

bool is_variable(string & token, compiler_state & state)
{
    return is_num_var(token, state) || is_txt_var(token, state);
}

bool is_num_expr(string & token, compiler_state & state)
{
    return is_num_var(token, state) || is_number(token);
}

bool is_txt_expr(string & token, compiler_state & state)
{
    return is_txt_var(token, state) || is_string(token);
}

bool is_expression(string & token, compiler_state & state)
{
    return is_num_expr(token, state) || is_txt_expr(token, state);
}

bool is_external(string & token, compiler_state & state)
{
    return state.externals[token];
}

void split_vector(string & token, string & vector, string & index)
{
    size_t pos = token.find(":");
    if (pos == string::npos) {
        vector = token;
        index = "";
        return;
    } else if (pos == token.size() - 1)
        error("Incomplete VECTOR access found (can't end on ':'!).");
    vector = token.substr(0, pos);
    index = token.substr(pos+1);
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
    string var_name, index;
    split_vector(variable, var_name, index);
    var_name = fix_identifier(var_name, true, state);
    //Single variable
    if(index.empty())
        return var_name;
    //Vector variable
    return var_name + '[' + get_c_expression(state, index) + ']';
}

string get_c_expression(compiler_state & state, string & expression)
{
    if(is_variable(expression, state))
        return get_c_variable(state, expression);
    is_number(expression); //We fix the expression should it be a number literal
    return expression;
}

// text must be a TEXT or a TEXT variable
string get_c_char_array(compiler_state & state, string & text)
{
    if(is_txt_var(text, state))
        return get_c_variable(state, text) + ".c_str()";
    return text;
}

string get_c_string(compiler_state & state, string & expression)
{
    string c_expression = get_c_expression(state, expression);
    if (is_num_expr(expression, state))
        return "to_ldpl_string(" + c_expression + ")";
    return c_expression;
}

string get_c_number(compiler_state & state, string & expression)
{
    string c_expression = get_c_expression(state, expression);
    if (is_txt_expr(expression, state))
        return "to_number(" + c_expression + ")";
    return c_expression;
}

string get_c_condition(compiler_state & state, vector<string> tokens) {
    string first_value = get_c_expression(state, tokens[0]);
    string second_value = get_c_expression(state, tokens.rbegin()[0]);

    string rel_op = "";
    for(unsigned int i = 2; i < tokens.size() - 1; ++i){
        rel_op += tokens[i] + " ";
    }
    if(is_txt_expr(tokens[0], state)) {
        if(rel_op == "EQUAL TO ")
            return first_value + " == " + second_value;
        else
            return first_value + " != " + second_value;
    } else {
        if(rel_op == "EQUAL TO ")
            return "num_equal(" + first_value + ", " + second_value + ")";
        else if(rel_op == "NOT EQUAL TO ")
            return "!num_equal(" + first_value + ", " + second_value + ")";
        else if(rel_op == "GREATER THAN ")
            return first_value + " > " + second_value;
        else if(rel_op == "LESS THAN ")
            return first_value + " < " + second_value;
        else if(rel_op == "GREATER THAN OR EQUAL TO ")
            return first_value + " > " + second_value
            + " || num_equal(" + first_value + ", " + second_value + ")";
        else
            return first_value + " < " + second_value
            + " || num_equal(" + first_value + ", " + second_value + ")";
    }
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
