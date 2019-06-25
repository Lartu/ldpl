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
                if(files_to_compile.size() > 0){
                    warning("passing multiple LDPL source files to the\ncompiler is deprecated and may be removed in the future.\nPlease use the IMPORT statement instead.");
                }
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
                    if(files_to_compile.size() > 0){
                        warning("passing multiple LDPL source files to the\ncompiler is deprecated and may be removed in the future.\nPlease use the IMPORT statement instead.");
                    }
                    files_to_compile.push_back(arg.substr(3));
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

    state.variables[""]["ARGC"] = 1;
    state.add_var_code("ldpl_number "+fix_identifier("ARGC", true)+";");
    state.variables[""]["ARGV"] = 4;
    state.add_var_code("ldpl_vector<string> "+fix_identifier("ARGV", true)+";");
    state.variables[""]["ERRORCODE"] = 1; //Declared in ldpl_lib.cpp
    state.variables[""]["ERRORTEXT"] = 2; //Declared in ldpl_lib.cpp
    state.add_code("for(int i = 1; i < argc; ++i)");
    state.add_code(fix_identifier("ARGV", true) + "[i-1] = argv[i];");
    state.add_code(fix_identifier("ARGC", true) + " = argc - 1;");

    //Fail if file was not passed
    if(files_to_compile.size() == 0) error("filename expected.");

    //For each file, compile each file into one big code
    for(string & filename : files_to_compile)
    {
        //Reset state section for this file
        state.section_state = 0;
        if(filename != "-c"){
            bullet_msg("Loading " + filename);
            load_and_compile(filename, state);
        }else{
            bullet_msg("Waiting for standard input...");
            state.current_file = "standard input";
            accept_and_compile(state);
        }
        if(state.section_state < 2) error("PROCEDURE section not found" + (filename == "-c" ? "." : " in file '" + filename + "'."));
    }
    state.add_code("return 0; \n}");

    //If an expected subprocedure was not declared, raise an error
    if(state.expected_subprocedures.size() > 0)
        error("the subprocedure " + state.expected_subprocedures[0].first + " is called but never declared.");

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
    bullet_msg("Building " + final_filename);
    //Compile the C++ code
    string compile_line = "c++ ldpl-temp.cpp -std=gnu++11 -w -o " + final_filename;
#ifdef STATIC_BUILDS
    if(!no_static) compile_line+=" -static-libgcc -static-libstdc++ ";
#endif
    if(!extensions.empty()){
        for(string & extension : extensions){
            bullet_msg("Including C++ extension " + extension);
            compile_line += " "+extension;
        }
    }
    int compiled = system(compile_line.c_str());
    #if defined(_WIN32)
        system("del ldpl-temp.cpp");
    #else
        system("rm ldpl-temp.cpp");
    #endif
    if(compiled == 0){
        bullet_msg("Saved as " + final_filename);
        bullet_msg("\033[32;1mFile(s) compiled successfully.\033[0m");
    }else{
        error("compilation failed.");
    }
}

void load_and_compile(string & filename, compiler_state & state)
{
    state.current_file = filename;
    //Accept input from stdin
    ifstream file(filename);
    //Fail if the file couldn't be loaded
    if(!file.is_open()) error("The file '" + filename + "' couldn't be loaded.");
    //Get file contents
    vector<string> lines;
    string line = "";
    while(getline(file, line))
    {
        replace_whitespace(line);
        lines.push_back(line);
    }
    bullet_msg("Compiling " + filename);
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
    bullet_msg("Compiling");
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

//Shows message preceded by a *
void bullet_msg(const string & msg)
{
    cerr << "\033[1;33m*\033[0m ";
    cerr << msg << endl;
}

//Shows warning message
void warning(const string & msg)
{
    cerr << "\033[1;35mLDPL Warning: ";
    cerr << msg;
    cerr << "\033[0m" << endl;
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
            if(line.size() >= 9/*&& (line[0] == 'E' || line[0] == 'e')*/){
                string upper = "";
                for(char c : line) upper += toupper(c);
                trim(upper);
                if(upper == "END QUOTE"){
                    state.open_quote = false;
                    //Kill final newline. Programs can add crlf if needed.
                    string & prev = state.current_subprocedure != "" ? state.subroutine_code.back() : state.output_code.back();
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
    if(state.open_quote) error("a QUOTE block was not terminated.");
    if(state.closing_subprocedure()) error("a SUB-PROCEDURE block was not terminated.");
    if(state.closing_if()) error("a IF block was not terminated.");
    if(state.closing_loop()) error("a WHILE or FOR block was not terminated.");
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
    string current_file = state.current_file;
    ++line_num;

    //import
    if(line_like("IMPORT $string", tokens, state)) {
        if(state.section_state != 0)
            error("can only import files declared at the start of the file (\033[0m" + current_file + ":" + to_string(line_num)+"\033[1;31m)");
        else {
            string file_to_compile = tokens[1].substr(1, tokens[1].size() - 2);
            string separators = "/";
            #if defined(_WIN32)
            separators += "\\";
            #endif
            size_t last_sep = current_file.find_last_of(separators);
            if (last_sep != string::npos)
                file_to_compile = current_file.substr(0, last_sep) + "/" + file_to_compile;
            load_and_compile(file_to_compile, state);
            state.section_state = 0;
            state.current_file = current_file;
        }
        return;
    }

    // Sections
    if(line_like("DATA:", tokens, state))
    {
        if(state.section_state == 1)
            error("Duplicate DATA section declaration (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(state.section_state >= 2)
            error("DATA section declaration within PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        state.section_state = 1;
        return;
    }
    if(line_like("PROCEDURE:", tokens, state))
    {
        if(state.section_state == 2)
            error("Duplicate PROCEDURE section declaration (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if (state.current_subprocedure != "" && state.section_state >= 3)
            open_subprocedure_code(state, line_num, current_file);
        state.section_state = 2;
        return;
    }
    if(line_like("PARAMETERS:", tokens, state))
    {
        if(state.current_subprocedure == "")
            error("PARAMETERS section outside subprocedure (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(state.section_state == 4)
            error("Duplicate PARAMETERS section declaration (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(state.section_state == 1)
            error("PARAMETERS section declaration within LOCAL DATA section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(state.section_state == 2)
            error("PARAMETERS section declaration within PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        state.section_state = 4;
        return;
    }
    if(line_like("LOCAL DATA:", tokens, state))
    {
        if(state.current_subprocedure == "")
            error("LOCAL DATA section outside subprocedure (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(state.section_state == 1)
            error("Duplicate LOCAL DATA section declaration (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(state.section_state == 2)
            error("LOCAL DATA section declaration within PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        state.section_state = 1;
        open_subprocedure_code(state, line_num, current_file);
        return;
    }

    // Variable Declaration
    if(line_like("$name IS $anything", tokens, state))
    {
        string extern_keyword = ""; // C++ extern keyword to prepend to the type (empty if not EXTERNAL)
        string type; // C++ variable type
        unsigned int type_number;
        string assign_default; // default value assignation to variable
        size_t i = 2;
        bool valid_type = true;
        if (tokens[i] == "EXTERNAL" && state.current_subprocedure == "") { // EXTERNAL is only valid in DATA section
            state.externals[tokens[0]] = true;
            extern_keyword = "extern ";
            ++i;
        }
        if (tokens[i] == "NUMBER") {
            type_number = 1;
            type = "ldpl_number";
            if (extern_keyword == "") assign_default = " = 0";
        } else if (tokens[i] == "TEXT") {
            type_number = 2;
            type = "string";
            if (extern_keyword == "") assign_default = " = \"\"";
        } else {
            valid_type = false;
        }
        if (valid_type && ++i < tokens.size()) {
            // Collections
            assign_default = ""; // Collections are initially empty
            if (tokens[i] == "MAP" || tokens[i] == "VECTOR") {
                type_number += 2; // 1 -> 3, 2 -> 4
                type = "ldpl_vector<" + type + ">";
            } else if (tokens[i] == "LIST") {
                type_number += 4; // 1 -> 5, 2 -> 6
                type = "ldpl_list<" + type + ">";
            } else {
                valid_type = false;
            }
        }
        if (valid_type && i >= tokens.size() - 1) {
            if(state.section_state != 1 && state.section_state != 4)
                error("Variable declaration outside DATA, PARAMETERS or LOCAL DATA section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
            if(state.variables[state.current_subprocedure].count(tokens[0]) > 0)
                error("Duplicate declaration for variable " + tokens[0] + " (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
            state.variables[state.current_subprocedure][tokens[0]] = type_number;
            string identifier = fix_identifier(tokens[0], true, state);
            if (state.section_state == 1) { // DATA or LOCAL DATA
                string code = extern_keyword + type + " " + identifier + assign_default + ";";
                if (state.current_subprocedure == "") // DATA
                    state.add_var_code(code);
                else
                    state.add_code(code); // LOCAL DATA
            } else // PARAMETERS
                state.subprocedures[state.current_subprocedure].emplace_back(type, identifier);
            return;
        }
    }

    // SUB-PROCEDURE Declaration
    if(line_like("SUB-PROCEDURE $name", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("SUB-PROCEDURE declaration outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(is_subprocedure(tokens[1], state))
            error("Duplicate declaration for subprocedure " + tokens[1] + " (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(state.closing_subprocedure())
            error("Subprocedure declaration inside subprocedure (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        else if(state.closing_if())
            error("Subprocedure declaration inside IF (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        else if(state.closing_loop())
            error("Subprocedure declaration inside WHILE or FOR (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        state.section_state = 3;
        state.open_subprocedure(tokens[1]);
        state.subprocedures.emplace(tokens[1], vector<pair<string, string>>());
        return;
    }
    if(line_like("EXTERNAL SUB-PROCEDURE $external", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("EXTERNAL SUB-PROCEDURE declaration outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(state.closing_subprocedure())
            error("Subprocedure declaration inside subprocedure (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        else if(state.closing_if())
            error("Subprocedure declaration inside IF (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        else if(state.closing_loop())
            error("Subprocedure declaration inside WHILE or FOR (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        else
            state.open_subprocedure(tokens[2]);
        //C Code
        state.add_code("void "+fix_external_identifier(tokens[2], false)+"(){");
        return;
    }
    if(line_like("END SUB-PROCEDURE", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
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
        if(!in_procedure_section(state, line_num, current_file))
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
        if(!in_procedure_section(state, line_num, current_file))
            error("IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.open_if();
        state.add_code("if (" + get_c_condition(state, vector<string>(tokens.begin()+1, tokens.end()-1)) + "){");
        return;
    }
    if(line_like("ELSE IF $condition THEN", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("ELSE IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(!state.closing_if())
            error("ELSE IF without IF (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("} else if (" + get_c_condition(state, vector<string>(tokens.begin()+2, tokens.end()-1)) + "){");
        return;
    }
    if(line_like("ELSE", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
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
        if(!in_procedure_section(state, line_num, current_file))
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
        if(!in_procedure_section(state, line_num, current_file))
            error("WHILE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.open_loop();
        state.add_code("while (" + get_c_condition(state, vector<string>(tokens.begin()+1, tokens.end()-1)) + "){");
        return;
    }
    if(line_like("REPEAT", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("REPEAT outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(!state.closing_loop())
            error("REPEAT without WHILE or FOR (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.close_loop();
        state.add_code("}");
        return;
    }
    if(line_like("FOR $num-var FROM $num-expr TO $num-expr STEP $num-expr DO", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("FOR outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        state.open_loop();
        string var = get_c_variable(state, tokens[1]);
        string from = get_c_expression(state, tokens[3]);
        string to = get_c_expression(state, tokens[5]);
        string step = get_c_expression(state, tokens[7]);
        string init = var + " = " + from;
        string condition = step + " >= 0 ? " +
                           var + " < " + to + " : " + var + " > " + to;
        string increment = var + " += " + step;
        //C Code
        state.add_code("for (" + init + "; " + condition + "; " + increment + ") {");
        return;
    }
    if(line_like("FOR EACH $var IN $collection DO", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("FOR EACH outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(is_num_var(tokens[2], state) != (is_num_vector(tokens[4], state) || is_num_list(tokens[4], state)))
            error("FOR EACH iteration variable type doesn't match collection type (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        state.open_loop();
        //C Code
        string range_var = state.new_range_var();
        string collection = get_c_variable(state, tokens[4]);
        string iteration_var = range_var;
        if (is_vector(tokens[4], state)) {
            collection += ".inner_map";
            iteration_var += ".second";
        }
        state.add_code("for (auto& " + range_var + " : " + collection + ") {");
        state.add_code(get_c_variable(state, tokens[2]) + " = " + iteration_var + ";");
        return;
    }
    if(line_like("BREAK", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("BREAK outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(state.open_loops == 0)
            error("BREAK without WHILE or FOR (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("break;");
        return;
    }
    if(line_like("CONTINUE", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("CONTINUE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(state.open_loops == 0)
            error("CONTINUE without WHILE or FOR (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("continue;");
        return;
    }
    if(line_like("CALL SUB-PROCEDURE $anything", tokens, state) || line_like("CALL $anything", tokens, state))
    {
        size_t i = 1;
        if(tokens[i] == "SUB-PROCEDURE") i++;
        string subprocedure = tokens[i];
        // Valid options: No WITH or WITH with at least one paramter
        if (i == tokens.size()-1 || (i < tokens.size()-2 && tokens[i+1] == "WITH")) {
            if(!in_procedure_section(state, line_num, current_file))
                error("CALL outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
            vector<string> parameters(i != tokens.size()-1 ? tokens.begin() + i + 2 : tokens.end(), tokens.end());
            vector<string> types;
            for (string & parameter : parameters) {
                if (is_num_expr(parameter, state))
                    types.push_back("ldpl_number");
                else if(is_txt_expr(parameter, state))
                    types.push_back("string");
                else if(is_num_vector(parameter, state))
                    types.push_back("ldpl_vector<ldpl_number>");
                else if(is_num_list(parameter, state))
                    types.push_back("ldpl_list<ldpl_number>");
                else if(is_txt_vector(parameter, state))
                    types.push_back("ldpl_vector<string>");
                else if(is_txt_list(parameter, state))
                    types.push_back("ldpl_list<string>");
                else
                    error("CALL with invalid parameter \"" + parameter + "\" (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
            }
            bool correct_types = state.correct_subprocedure_types(subprocedure, types);
            if(!is_subprocedure(subprocedure, state)) {
                if(!correct_types)
                    error("CALL parameter types doesn't match previous CALL (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
                state.add_expected_subprocedure(subprocedure, fix_identifier(subprocedure, false), types);
            } else {
                if(!correct_types)
                    error("CALL parameter types doesn't match SUB-PROCEDURE declaration (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
            }
            add_call_code(subprocedure, parameters, state, line_num);
            return;
        }
    }
    if(line_like("CALL EXTERNAL $external", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("CALL EXTERNAL outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        state.add_code(fix_external_identifier(tokens[2], false) + "();");
        //prototype of function defined in extension
        state.add_var_code("void "+fix_external_identifier(tokens[2], false)+"();");
        return;
    }
    if(line_like("RETURN", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("RETURN outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(state.current_subprocedure == "")
            error("RETURN found outside subprocedure (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("return;");
        return;
    }
    if(line_like("EXIT", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("EXIT outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("exit(0);");
        return;
    }
    if(line_like("WAIT $num-expr MILLISECONDS", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("WAIT statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        #if defined(_WIN32)
        state.add_code("_sleep((long int)" + get_c_expression(state, tokens[1]) + ");");
        #else
        state.add_code("std::this_thread::sleep_for(std::chrono::milliseconds((long int)" + get_c_expression(state, tokens[1]) + "));");
        #endif
        return;
    }
    if(line_like("GOTO $label", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("GOTO outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        state.add_code("goto label_"+fix_identifier(tokens[1])+";");
        return;
    }
    if(line_like("LABEL $label", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("LABEL outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        state.add_code("label_"+fix_identifier(tokens[1])+":");
        return;
    }

    // Arithmetic Statements
    if(line_like("ADD $num-expr AND $num-expr IN $num-var", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("ADD statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[5]) + " = " + get_c_expression(state, tokens[1]) + " + " + get_c_expression(state, tokens[3]) + ";");
        return;
    }
    if(line_like("SUBTRACT $num-expr FROM $num-expr IN $num-var", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("SUBTRACT statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[5]) + " = " + get_c_expression(state, tokens[3]) + " - " + get_c_expression(state, tokens[1]) + ";");
        return;
    }
    if(line_like("MULTIPLY $num-expr BY $num-expr IN $num-var", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("MULTIPLY statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[5]) + " = " + get_c_expression(state, tokens[1]) + " * " + get_c_expression(state, tokens[3]) + ";");
        return;
    }
    if(line_like("DIVIDE $num-expr BY $num-expr IN $num-var", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("DIVIDE statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[5]) + " = " + get_c_expression(state, tokens[1]) + " / " + get_c_expression(state, tokens[3]) + ";");
        return;
    }
    if(line_like("MODULO $num-expr BY $num-expr IN $num-var", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("MODULO statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[5]) + " = modulo(" + get_c_expression(state, tokens[1]) + ", " + get_c_expression(state, tokens[3]) + ");");
        return;
    }
    if(line_like("ABS $num-var", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("ABS statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[1]) + " = fabs("+get_c_variable(state, tokens[1])+");");
        return;
    }
    if(line_like("STORE RANDOM IN $num-var", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("RANDOM outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[3]) + " = get_random();");
        return;
    }
    if(line_like("FLOOR $num-var", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("FLOOR statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[1]) + " = floor(" + get_c_variable(state, tokens[1]) +");");
        return;
    }
    if(line_like("CEIL $num-var", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("CEIL statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[1]) + " = ceil(" + get_c_variable(state, tokens[1]) +");");
        return;
    }
    if(line_like("INCR $num-var", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("INCR statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("++" + get_c_variable(state, tokens[1]) + ";");
        return;
    }
    if(line_like("DECR $num-var", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("DECR statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("--" + get_c_variable(state, tokens[1]) + ";");
        return;
    }

    if(line_like("IN $num-var SOLVE $math", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
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
        if(!in_procedure_section(state, line_num, current_file))
            error("JOIN statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("join(" + get_c_string(state, tokens[1]) + ", " + get_c_string(state, tokens[3]) + ", " + get_c_variable(state, tokens[5]) + ");");
        return;
    }
    if(line_like("GET CHARACTER AT $num-expr FROM $str-expr IN $str-var", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("GET CHARACTER statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[7]) + " = charat(" + get_c_expression(state, tokens[5]) + ", " + get_c_expression(state, tokens[3]) + ");");
        return;
    }
    if(line_like("STORE LENGTH OF $str-expr IN $num-var", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("EXECUTE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[5]) + " = str_len(" + get_c_expression(state, tokens[3]) + ");");
        return;
    }
    if(line_like("STORE CHARACTER $num-expr IN $str-var", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("STORE statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[4]) + " = (char)" + get_c_expression(state, tokens[2]) + ";");
        return;
    }
    if(line_like("STORE CHARACTER CODE OF $str-expr IN $num-var", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("STORE statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[6]) + " = get_char_num(" + get_c_expression(state, tokens[4]) + ");");
        return;
    }
    if(line_like("STORE QUOTE IN $str-var", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
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
        if(!in_procedure_section(state, line_num, current_file))
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
        if(!in_procedure_section(state, line_num, current_file))
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
        if(!in_procedure_section(state, line_num, current_file))
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
        if(!in_procedure_section(state, line_num, current_file))
            error("EXECUTE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("system(" + get_c_char_array(state, tokens[1]) + ");");
        return;
    }
    if(line_like("EXECUTE $str-expr AND STORE OUTPUT IN $str-var", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("EXECUTE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[6]) + " = exec(" + get_c_char_array(state, tokens[1]) + ");");
        return;
    }
    if(line_like("EXECUTE $str-expr AND STORE EXIT CODE IN $num-var", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("EXECUTE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[7]) + " = (system(" + get_c_char_array(state, tokens[1]) + ") >> 8) & 0xff;"); //shift wait() val and get lowest 2
        return;
    }
    if(line_like("ACCEPT $str-var UNTIL EOF", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("ACCEPT statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[1])+" = input_until_eof();");
        return;
    }
    if(line_like("LOAD FILE $str-expr IN $str-var", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("LOAD FILE statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("load_file(" + get_c_expression(state, tokens[2]) + ", " + get_c_variable(state, tokens[4]) +");");
        return;
    }
    if(line_like("WRITE $expression TO FILE $str-expr", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("WRITE statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("file_writing_stream.open(" + get_c_expression(state, tokens[4]) + ", ios_base::out);");
        state.add_code("file_writing_stream <<" + get_c_expression(state, tokens[1]) + ";");
        state.add_code("file_writing_stream.close();");
        return;
    }
    if(line_like("APPEND $expression TO FILE $str-expr", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("APPEND statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("file_writing_stream.open(" + get_c_expression(state, tokens[4]) + ", ios_base::app);");
        state.add_code("file_writing_stream <<" + get_c_expression(state, tokens[1]) + ";");
        state.add_code("file_writing_stream.close();");
        return;
    }

    if(line_like("REPLACE $str-expr FROM $str-expr WITH $str-expr IN $str-var", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("REPLACE statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[7]) + " = str_replace(" + get_c_expression(state, tokens[3]) + ", " + get_c_expression(state, tokens[1]) + ", " + get_c_expression(state, tokens[5]) + ");");
        return;
    }
    if(line_like("SPLIT $str-expr BY $str-expr IN $str-vec", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("SPLIT statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[5]) + " = utf8_split(" + get_c_expression(state, tokens[1]) + ", " + get_c_expression(state, tokens[3]) + ");");
        return;
    }
    if(line_like("GET INDEX OF $str-expr FROM $str-expr IN $num-var", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("GET INDEX OF statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[7]) + " = utf8GetIndexOf(" + get_c_expression(state, tokens[5]) + ", " + get_c_expression(state, tokens[3]) + ");");
        return;
    }
    if(line_like("COUNT $str-expr FROM $str-expr IN $num-var", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("COUNT statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[5]) + " = utf8Count(" + get_c_expression(state, tokens[3]) + ", " + get_c_expression(state, tokens[1]) + ");");
        return;
    }
    if(line_like("SUBSTRING $str-expr FROM $num-expr LENGTH $num-expr IN $str-var", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("SUBSTRING statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("joinvar = " + get_c_expression(state, tokens[1]) + ";");
        state.add_code(get_c_variable(state, tokens[7]) + " = utf8_substr(joinvar, " + get_c_expression(state, tokens[3]) + ", " + get_c_expression(state, tokens[5]) + ");");
        return;
    }
    if(line_like("TRIM $str-expr IN $str-var", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("TRIM statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[3]) + " = trimCopy(" + get_c_expression(state, tokens[1]) +  ");");
        return;
    }
    if(line_like("CLEAR $vector", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("CLEAR statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[1]) + ".clear();");
        return;
    }
    if(line_like("COPY $str-vec TO $str-vec", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("COPY statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[3]) + " = " + get_c_variable(state, tokens[1]) + ";");
        return;
    }
    if(line_like("COPY $num-vec TO $num-vec", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("COPY statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[3]) + " = " + get_c_variable(state, tokens[1]) + ";");
        return;
    }
    if(line_like("STORE INDEX COUNT OF $vector IN $num-var", tokens, state)) //Deprecated
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("STORE INDEX COUNT statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[6]) + " = " + get_c_variable(state, tokens[4]) + ".count();");
        return;
    }
    if(line_like("STORE INDICES OF $vector IN $str-vec", tokens, state)) //Deprecated
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("STORE INDICES statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("get_indices(" + get_c_variable(state, tokens[5]) + ", " + get_c_variable(state, tokens[3]) + ");");
        return;
    }
    if(line_like("STORE KEY COUNT OF $vector IN $num-var", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("STORE KEY COUNT statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[6]) + " = " + get_c_variable(state, tokens[4]) + ".count();");
        return;
    }
    if(line_like("STORE KEYS OF $vector IN $str-vec", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("STORE KEYS statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("get_indices(" + get_c_variable(state, tokens[5]) + ", " + get_c_variable(state, tokens[3]) + ");");
        return;
    }
    if(line_like("PUSH $num-expr TO $num-list", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("PUSH statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[3]) + ".push_back(" + get_c_expression(state, tokens[1]) + ");");
        return;
    }
    if(line_like("PUSH $str-expr TO $str-list", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("PUSH statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[3]) + ".push_back(" + get_c_expression(state, tokens[1]) + ");");
        return;
    }
    if(line_like("CLEAR $list", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("CLEAR statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[1]) + ".clear();");
        return;
    }
    if(line_like("STORE LENGTH OF $list IN $num-var", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("STORE LENGTH OF (list) statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[5]) + " = " + get_c_variable(state, tokens[3]) + ".size();");
        return;
    }
    if(line_like("DELETE LAST ELEMENT OF $list", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("DELETE LAST ELEMENT OF statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code("if(" + get_c_variable(state, tokens[4]) + ".size() > 0)");
        state.add_code(get_c_variable(state, tokens[4]) + ".pop_back();");
        return;
    }
    if(line_like("COPY $str-list TO $str-list", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("COPY statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[3]) + " = " + get_c_variable(state, tokens[1]) + ";");
        return;
    }
    if(line_like("COPY $num-list TO $num-list", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("COPY statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[3]) + " = " + get_c_variable(state, tokens[1]) + ";");
        return;
    }
    if(line_like("SPLIT $str-expr BY $str-expr IN $str-list", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("SPLIT statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[5]) + " = utf8_split_list(" + get_c_expression(state, tokens[1]) + ", " + get_c_expression(state, tokens[3]) + ");");
        return;
    }
    if(line_like("LIST CONTENTS OF DIRECTORY $str-expr IN $str-list", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("LIST CONTENTS OF DIRECTORY statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C Code
        state.add_code(get_c_variable(state, tokens[6]) + " = GetDirectoryFiles(" + get_c_expression(state, tokens[4]) + ");");
        return;
    }

    // Custom Statements
    if(line_like("CREATE STATEMENT $string EXECUTING $subprocedure", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("CREATE STATEMENT statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(state.closing_subprocedure())
            error("CREATE STATEMENT statement inside SUB-PROCEDURE (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        else if(state.closing_if())
            error("CREATE STATEMENT statement inside IF (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        else if(state.closing_loop())
            error("CREATE STATEMENT statement inside WHILE or FOR (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        string model_line = tokens[2].substr(1, tokens[2].size() - 2);
        vector<string> model_tokens;
        vector<pair<string, string>> parameters = state.subprocedures[tokens[4]];
        trim(model_line);
        tokenize(model_line, 0, model_tokens, state.current_file, true);
        size_t param_count = 0;
        size_t keyword_count = 0;
        string valid_keyword_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        model_line = "";
        for (string & token : model_tokens) {
            if (token == "$") {
                ++param_count;
                if (param_count > parameters.size()) break;
                if (parameters[param_count-1].first == "ldpl_number")
                    model_line += "$num-expr ";
                else if (parameters[param_count-1].first == "string")
                    model_line += "$str-expr ";
                else if (parameters[param_count-1].first == "ldpl_vector<ldpl_number>")
                    model_line += "$num-vec ";
                else if (parameters[param_count-1].first == "ldpl_list<ldpl_number>")
                    model_line += "$num-list ";
                else if (parameters[param_count-1].first == "ldpl_vector<string>")
                    model_line += "$str-vec ";
                else if (parameters[param_count-1].first == "ldpl_list<string>")
                    model_line += "$str-list ";
            } else if (token.find_first_not_of(valid_keyword_chars) == string::npos) {
                ++keyword_count;
                model_line += token + " ";
            } else {
                error("CREATE STATEMENT with invalid token \"" + token + "\" (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
            }
        }
        if (param_count != parameters.size())
            error("CREATE STATEMENT parameters count doesn't match SUB-PROCEDURE (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if (keyword_count == 0)
            error("CREATE STATEMENT without keywords (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        state.custom_statements.emplace_back(model_line, tokens[4]);
        return;
    }
    for (pair<string, string> & statement : state.custom_statements) {
        if(line_like(statement.first, tokens, state))
        {
            string prefix = statement.first.substr(0, statement.first.find("$"));
            if(!in_procedure_section(state, line_num, current_file))
                error(prefix + "statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
            vector<string> model_tokens;
            vector<string> parameters;
            tokenize(statement.first, line_num, model_tokens, state.current_file, false);
            for (size_t i = 0; i < model_tokens.size(); i++) {
                if (model_tokens[i][0] == '$')
                    parameters.push_back(tokens[i]);
            }
            add_call_code(statement.second, parameters, state, line_num);
            return;
        }
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
        else if(model_tokens[i] == "$vector"){ //$vector is TEXT MAP, NUMBER MAP
            if(!is_vector(tokens[j], state)) return false;
        }
        else if(model_tokens[i] == "$num-vec") //$num-vec is NUMBER vector
        {
            if(!is_num_vector(tokens[j], state)) return false;
        }
        else if(model_tokens[i] == "$str-vec") //$str-vec is TEXT vector
        {
            if(!is_txt_vector(tokens[j], state)) return false;
        }
        else if(model_tokens[i] == "$list"){ //$list is a LIST
            if(!is_list(tokens[j], state)) return false;
        }
        else if(model_tokens[i] == "$num-list") //$num-vec is NUMBER list
        {
            if(!is_num_list(tokens[j], state)) return false;
        }
        else if(model_tokens[i] == "$str-list") //$str-vec is TEXT list
        {
            if(!is_txt_list(tokens[j], state)) return false;
        }
        else if(model_tokens[i] == "$collection") //$collection is either a MAP or a LIST
        {
            if(!is_vector(tokens[j], state) && !is_list(tokens[j], state)) return false;
        }
        else if(model_tokens[i] == "$literal") //$literal is either a NUMBER or a TEXT
        {
            if(!is_string(tokens[j]) && !is_number(tokens[j])) return false;
        }
        else if(model_tokens[i] == "$string") //$string is a TEXT literal
        {
            if(!is_string(tokens[j])) return false;
        }
        else if(model_tokens[i] == "$number") //$number is a NUMBER literal
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
        else if(model_tokens[i] == "$subprocedure") //$subprocedure is a SUB-PROCEDURE
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
            bool text_values = false;
            bool vector_values = false;
            bool list_values = false;
            if(is_num_expr(first_value, state) && is_num_expr(second_value, state))
                text_values = false;
            else if(is_txt_expr(first_value, state) && is_txt_expr(second_value, state))
                text_values = true;
            else if(is_num_vector(first_value, state) && is_num_vector(second_value, state))
                vector_values = true;
            else if(is_txt_vector(first_value, state) && is_txt_vector(second_value, state))
                vector_values = true;
            else if(is_num_list(first_value, state) && is_num_list(second_value, state))
                list_values = true;
            else if(is_txt_list(first_value, state) && is_txt_list(second_value, state))
                list_values = true;
            else
                return false;

            if(tokens[++j] != "IS") return false;

            string rel_op;
            for(++j; j < tokens.size() - 2; ++j){
                rel_op += tokens[j] + " ";
            }
            if(rel_op != "EQUAL TO " && rel_op != "NOT EQUAL TO "){
                if(text_values) return false;
                if(vector_values) return false;
                if(list_values) return false;
                if(rel_op != "GREATER THAN " && rel_op != "GREATER THAN OR EQUAL TO "
                && rel_op != "LESS THAN " && rel_op != "LESS THAN OR EQUAL TO ")
                    return false;
            }
        }
        else if(model_tokens[i] == "$anything") return true;
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
    if(variable_type(token, state) == 3) return true;
    return false;
}

bool is_txt_vector(string & token, compiler_state & state)
{
    if(variable_type(token, state) == 4) return true;
    return false;
}

bool is_num_list(string & token, compiler_state & state)
{
    if(variable_type(token, state) == 5) return true;
    return false;
}

bool is_txt_list(string & token, compiler_state & state)
{
    if(variable_type(token, state) == 6) return true;
    return false;
}

bool is_vector(string & token, compiler_state & state)
{
    return is_num_vector(token, state) || is_txt_vector(token, state);
}

bool is_list(string & token, compiler_state & state)
{
    return is_num_list(token, state) || is_txt_list(token, state);
}

bool is_num_var(string & token, compiler_state & state)
{
    //Check if var
    if(variable_type(token, state) == 1) return true;
    //Check if num_vector:index
    string vector, index;
    split_vector(token, vector, index);
    return (is_num_vector(vector, state) || is_num_list(vector, state)) && is_expression(index, state);
}

bool is_txt_var(string & token, compiler_state & state)
{
    //Check if var
    if(variable_type(token, state) == 2) return true;
    //Check if txt_vector:index
    string vector, index;
    split_vector(token, vector, index);
    return (is_txt_vector(vector, state) || is_txt_list(vector, state)) && index != "" && is_expression(index, state);
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
        //Bear in mind that if we are storing a value in vector:"",
        //this means that index will contain "\"\"", and not "".
    } else if (pos == token.size() - 1)
        error("Incomplete MAP or LIST access found (can't end on ':'!).");
    else{
        vector = token.substr(0, pos);
        index = token.substr(pos+1);
    }
}

/*La diferencia entre is_variable y variable_exists es que is_variable
 no toma los vectores como una variable, sino los índices de los vectores.
 Variable exists te dice si una variable está declarada como tal, por lo que
 un subíndice de vector no sería una variable.*/
bool variable_exists(string & token, compiler_state & state)
{
    return variable_type(token, state) != 0;
}

bool is_subprocedure(string & token, compiler_state & state)
{
    for(auto & subprocedure : state.subprocedures)
        if(subprocedure.first == token) return true;
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
    if(is_variable(expression, state) || variable_exists(expression, state))
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
    // Text expressions
    if(is_txt_expr(tokens[0], state)) {
        if(rel_op == "EQUAL TO ")
            return first_value + " == " + second_value;
        else
            return first_value + " != " + second_value;
    }
    // Vectors
    else if(is_vector(tokens[0], state)){
        if(rel_op == "EQUAL TO ")
            return get_c_variable(state, first_value) + " == " + get_c_variable(state, second_value);
        else
            return "!(" + get_c_variable(state, first_value) + " == " + get_c_variable(state, second_value) + ")";
    }
    // Numeric expressions
    else {
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

bool in_procedure_section(compiler_state & state, unsigned int line_num, string & current_file) {
    if (state.section_state == 3) {
        //We're inside a SUB-PROCEDURE procedure with no sections
        state.section_state = 2;
        open_subprocedure_code(state, line_num, current_file);
    }
    return state.section_state == 2;
}

// Return the number of the type or 0 if the variable doesn't exist
unsigned int variable_type(string & token, compiler_state & state) {
    if (state.variables[""].count(token) > 0)
        return state.variables[""][token];
    if (state.variables[state.current_subprocedure].count(token) > 0)
        return state.variables[state.current_subprocedure][token];
    return 0;
}

// This is called when we know all parameters of a subprocedure
void open_subprocedure_code(compiler_state & state, unsigned int line_num, string & current_file) {
    string name = state.current_subprocedure;
    vector<pair<string, string>> & parameters = state.subprocedures[name];
    vector<string> types;
    string code = "void " +fix_identifier(name, false)+ "(";
    for (size_t i = 0; i < parameters.size(); ++i) {
        code += parameters[i].first + " & " + parameters[i].second;
        if (i < parameters.size() - 1) code += ", ";
        types.push_back(parameters[i].first);
    }
    if (!state.correct_subprocedure_types(name, types))
        error("SUB-PROCEDURE declaration parameter types doesn't match previous CALL (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
    code += "){";
    state.add_code(code);
    state.remove_expected_subprocedure(name);
}

void add_call_code(string & subprocedure, vector<string> & parameters, compiler_state & state, unsigned int line_num) {
    string current_file = state.current_file;
    string code = fix_identifier(subprocedure, false) + "(";
    for (size_t i = 0; i < parameters.size(); ++i) {
        if (is_number(parameters[i]) || is_string(parameters[i])) {
            // C++ doen't allow passing literals in  reference parameters, we create vars for them
            string literal_paramater_var = state.new_literal_parameter_var();
            state.add_code(is_number(parameters[i]) ? "ldpl_number " : "string "
                           + literal_paramater_var + " = " + parameters[i] + ";");
            code += literal_paramater_var;
        } else {
            code += get_c_variable(state, parameters[i]);
        }
        if (i < parameters.size() - 1) code += ", ";
    }
    code += ");";
    state.add_code(code);
}
