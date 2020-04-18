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

vector<string> extensions; //C++ extensions to add to the file
vector<string> extension_flags; //Flags to pass to the C++ Compiler

int main(int argc, const char* argv[])
{
    //Get command line arguments as string vector
    vector<string> args(argv + 1, argv + argc);

    if(contains_any(args, {"-v", "--version"})){
        cout << endl;
        cout << " This is \033[32;1mLDPL version " << VERSION << "\033[0m '\033[32;1m" << VERSIONNAME << "\033[0m'." << endl << endl;
        cout << " Copyright 2018-2020, \033[35;1mMartín del Río\033[0m (www.lartu.net)." << endl;
        cout << " Built with amazing contributions from \033[35;1mChris West\033[0m, \033[35;1mDamián Garro\033[0m," << endl;
        cout << " \033[35;1mIgnacio Losiggio\033[0m, \033[35;1mFireasembler\033[0m and other wonderful contributors." << endl << endl;
        cout << " The LDPL Home Page can be found at \033[36;1mwww.ldpl-lang.org\033[0m." << endl;
        cout << " The LDPL source code is available at \033[36;1mwww.github.com/lartu/ldpl\033[0m." << endl << endl;
        cout << " Complete documentation for LDPL should be found on this system" << endl;
        cout << " using '\033[33;1mman ldpl\033[0m'. If you have access to the internet, the"<< endl;
        cout << " documentation can also be found online at \033[36;1mdocs.ldpl-lang.org\033[0m." << endl << endl;
        cout << " LDPL may be copied only under the terms of the Apache License" << endl;
        cout << " Version 2.0, which may be found in the LDPL repository." << endl <<endl;
        cout << " This binary was compiled on \033[31;1m" << COMPILEDATE << "\033[0m at \033[31;1m" << COMPILEHOUR << "\033[0m." << endl;
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
                extension_flags.push_back(arg.substr(3));
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

    state.variables[""]["ARGV"] = {2, 3}; //List of text
    state.add_var_code("ldpl_list<chText> "+fix_identifier("ARGV", true)+";");
    state.variables[""]["ERRORCODE"] = {1}; //Declared in ldpl_lib.cpp
    state.variables[""]["ERRORTEXT"] = {2}; //Declared in ldpl_lib.cpp
    state.add_code("for(int i = 1; i < argc; ++i)");
    state.add_code(fix_identifier("ARGV", true) + ".inner_collection.push_back(argv[i]);");

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
    
    //Add include for extensions
    if(!extensions.empty()){
        for(string & extension : extensions){
            bullet_msg("Including C++ extension " + extension);
            state.add_code("#include \"" + extension + "\"");
        }
    }

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
    //Compile the C++ code
    string compile_line = "c++ ldpl-temp.cpp -std=gnu++11 -lpthread -w -o " + final_filename;
#ifdef STATIC_BUILDS
    if(!no_static) compile_line+=" -static-libgcc -static-libstdc++ ";
#endif
    if(!extension_flags.empty()){
        for(string & flag : extension_flags){
            bullet_msg("using C++ switch " + flag);
            compile_line += " " + flag;
        }
    }
    bullet_msg("Building " + final_filename);
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

#include <array>
string exec(const char* cmd) {
    array<char, 128> buffer;
    string result;
    unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

string expandHomeDirectory(string & filename){
    #if defined(_WIN32)
        return filename;	
    #else
        string homeDir = exec("echo $HOME");
        trim(homeDir);
        string newPath = "";
        for(size_t i = 0; i < filename.length(); ++i){
            if(filename[i] == '~'){
                newPath += homeDir;
            }else{
                newPath += filename[i];
            }
        }
        return newPath;
    #endif
}

void load_and_compile(string & filename, compiler_state & state)
{
    filename = expandHomeDirectory(filename);
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
            state.add_code("\"" + escape_c_quotes(line) + "\\n\"", line_num);
            continue;
        }

        trim(line);
        //Split line in various tokens
        vector<string> tokens;
        tokenize(line, line_num, tokens, state.current_file, true, ' ');
        for(string & token : tokens) if(token == "CRLF" || token == "LF") token = "\"\\n\"";
        if(tokens.size() == 0) continue;
        compile_line(tokens, line_num, state);
    }
    if(state.open_quote) error("a QUOTE block was not terminated.");
    if(state.closing_subprocedure()) error("a SUB-PROCEDURE block was not terminated.");
    if(state.closing_if()) error("a IF block was not terminated.");
    if(state.closing_loop()) error("a WHILE or FOR block was not terminated.");
}

//Tokenizes a line splitting by 'splitChar' with optional convertion of tokens to uppercase (except in strings)
void tokenize(string & line, unsigned int line_num, vector<string> & tokens, string & current_file, bool uppercase, char splitChar)
{
    ++line_num;
    bool in_string = false;
    string current_token = "";
    //For each letter in the line
    for(unsigned int i = 0; i < line.size(); ++i)
    {
        char letter = line[i];
        if(letter == splitChar)
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
        if(i == line.size() - 1 && letter != splitChar)
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

    //include
    if(line_like("INCLUDE $string", tokens, state)) {
        if(state.section_state != 0)
            error("you can only use the INCLUDE statement before the DATA and PROCEDURE sections (\033[0m" + current_file + ":" + to_string(line_num)+"\033[1;31m)");
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

    //std include
    if(line_like("USING PACKAGE $name", tokens, state)) {
        if(state.section_state != 0)
            error("you can only use the USING PACKAGE statement before the DATA and PROCEDURE sections (\033[0m" + current_file + ":" + to_string(line_num)+"\033[1;31m)");
        else {
            #if defined(_WIN32)	
                //TODO!
                error("LPM packages are not yet supported on Windows.");
            #else
                system(((string)"mkdir -p " + LPMLOCATION).c_str());
            #endif
            string libFilename = tokens[2];
            std::for_each(libFilename.begin(), libFilename.end(), [](char & c){
                c = ::tolower(c);
            });
            string file_to_compile = LPMLOCATION + libFilename + "/" + libFilename + ".ldpl";
            load_and_compile(file_to_compile, state);
            state.section_state = 0;
            state.current_file = current_file;
        }
        return;
    }

    //extension (INCLUDE but for c++ extensions)
    if(line_like("EXTENSION $string", tokens, state)) {
        if(state.section_state != 0)
            error("you can only use the EXTENSION statement before the DATA and PROCEDURE sections (\033[0m" + current_file + ":" + to_string(line_num)+"\033[1;31m)");
        else {
            string file_to_add = tokens[1].substr(1, tokens[1].size() - 2);
            string separators = "/";
            #if defined(_WIN32)	
                separators += "\\";	
            #endif
            size_t last_sep = current_file.find_last_of(separators);
            if (last_sep != string::npos)
                file_to_add = current_file.substr(0, last_sep) + "/" + file_to_add;
            extensions.push_back(file_to_add);
        }
        return;
    }

    //extension flags (for the C++ compiler)
    if(line_like("FLAG $string", tokens, state)) {
        if(state.section_state != 0)
            error("you can only use the FLAG statement before the DATA and PROCEDURE sections (\033[0m" + current_file + ":" + to_string(line_num)+"\033[1;31m)");
        else {
            string flag = tokens[1].substr(1, tokens[1].size() - 2);
            extension_flags.push_back(flag);
        }
        return;
    }
    //os-specific extension flags
    if(line_like("FLAG $name $string", tokens, state)) {
        if(state.section_state != 0)
            error("you can only use the FLAG statement before the DATA and PROCEDURE sections (\033[0m" + current_file + ":" + to_string(line_num)+"\033[1;31m)");
        else {
            if (tokens[1] == current_os()) {
              string flag = tokens[2].substr(1, tokens[2].size() - 2);
              extension_flags.push_back(flag);
            }
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
        if(state.declaring_parallel)
            error("PARALLEL SUB-PROCEDURES cannot receive parameters (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
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
    if(line_like("$name IS $anything", tokens, state))                      // If it's a variable declaration
    {
        string extern_keyword = "";                                         // C++ extern keyword to prepend to the type (empty if not EXTERNAL)
        vector<unsigned int> type_number;                                   // All data types in LDPL have a list of numbers that represent its type
        string assign_default;                                              // Default Value to asign to the variable
        size_t i = 2;                                                       // i is used to check all the words after 'IS' and thus starts in 2.
        bool valid_type = true;                                             // Used to check if it's a valid tipe
        if (tokens[i] == "EXTERNAL" && state.current_subprocedure == "") {  // EXTERNAL is only valid in DATA section (not in LOCAL DATA)
            state.externals[tokens[0]] = true;                              // Add it to the list of external variables
            extern_keyword = "extern ";                                     // Set the prepended keyword to 'extern'
            ++i;                                                            // Check the next word.
        }
        if (tokens[i] == "NUMBER") {                                        // If it's a number...
            type_number.push_back(1);                                       // Then the type number is 1
            if (extern_keyword == "") assign_default = " = 0";              // if its not an external variable, set a default value for it.
        } else if (tokens[i] == "TEXT") {                                   // If we are dealing with a text variable...
            type_number.push_back(2);                                       // The type number is 2
            if (extern_keyword == "") assign_default = " = \"\"";           // And if it's not external, we set it to a default value.
        } else if ((tokens[i]=="MAP"||tokens[i]=="LIST") && tokens.size() > i && tokens[i+1] == "OF") {
            // nested 'of' syntax, ex: map of list of number
            while (valid_type && i < tokens.size()) {
                if (i % 2 == 1) {
                    if (tokens[i] != "OF")
                        valid_type = false;
                } else if (i % 2 == 0) {
                    if (tokens[i] == "MAP"||(tokens[i] == "MAPS" && i>0)) {
                        type_number.push_back(4);
                    } else if (tokens[i] == "LIST"||(tokens[i] == "LISTS" && i>0)) {
                        type_number.push_back(3);
                    } else if (tokens.size()-1 > i) {
                        // text and number must be the final type listed
                        valid_type = false;
                    } else if (tokens[i] == "TEXT") {
                        type_number.push_back(2);
                    } else if (tokens[i]=="NUMBER"||tokens[i]=="NUMBERS") {
                        type_number.push_back(1);
                    } else {
                        valid_type = false;
                    }
                } else {
                    valid_type = false;
                }
                ++i;
            }
            reverse(begin(type_number), end(type_number));
        } else {
            valid_type = false;                                             // If its not a NUMBER, a TEXT or a collection of these data types
        }                                                                   // then it's not a valid LDPL data type.
        ++i;                                                                // Move to the next keyword.
        while (valid_type && i < tokens.size()) {                           // If up to this point we got a valid data type, we check for containers.
            assign_default = "";                                            // Collections are not initialized with any default values.
            if (tokens[i] == "MAP" || tokens[i] == "VECTOR") {              // If its a MAP (aka VECTOR)
                type_number.push_back(4);                                   // We add the MAP type (4) to its type list
            } else if (tokens[i] == "LIST") {                               // If its a LIST
                type_number.push_back(3);                                   // We add the LIST type (3) to its type list
            } else {                                                        // If the container is not a VECTOR nor a MAP
                valid_type = false;                                         // then it's not a valid data type.
            }
            ++i;                                                            // Move to the next keyword.
        }
        if (valid_type && i >= tokens.size() - 1) {
            if(state.section_state != 1 && state.section_state != 4)
                error("Variable declaration outside DATA, PARAMETERS or LOCAL DATA section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
            if(state.variables[state.current_subprocedure].count(tokens[0]) > 0)
                error("Duplicate declaration for variable " + tokens[0] + " (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
            state.variables[state.current_subprocedure][tokens[0]] = type_number;
            if (state.section_state == 1) { // DATA or LOCAL DATA
                string identifier = fix_identifier(tokens[0], true, state);
                string type = state.get_c_type(type_number);
                string code = extern_keyword + type + " " + identifier + assign_default + ";";
                if (state.current_subprocedure == "") // DATA
                    state.add_var_code(code);
                else
                    state.add_code(code, line_num); // LOCAL DATA
            } else // PARAMETERS
                state.subprocedures[state.current_subprocedure].emplace_back(tokens[0]);
            return;
        }
    }

    // SUB-PROCEDURE Declaration
    if(line_like("SUB-PROCEDURE $name", tokens, state) || line_like("SUB $name", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("SUB-PROCEDURE declaration outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(is_subprocedure(tokens[1], state))
            error("Duplicate declaration for SUB-PROCEDURE " + tokens[1] + " (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(state.closing_subprocedure())
            error("SUB-PROCEDURE declaration inside SUB-PROCEDURE (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        else if(state.closing_if())
            error("SUB-PROCEDURE declaration inside IF (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        else if(state.closing_loop())
            error("SUB-PROCEDURE declaration inside WHILE or FOR (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        state.section_state = 3;
        state.open_subprocedure(tokens[1]);
        state.subprocedures.emplace(tokens[1], vector<string>());
        state.parallels.emplace(tokens[1], false);
        return;
    }
    if(line_like("EXTERNAL SUB-PROCEDURE $external", tokens, state) || line_like("EXTERNAL SUB $external", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("EXTERNAL SUB-PROCEDURE declaration outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(state.closing_subprocedure())
            error("SUB-PROCEDURE declaration inside SUB-PROCEDURE (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        else if(state.closing_if())
            error("SUB-PROCEDURE declaration inside IF (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        else if(state.closing_loop())
            error("SUB-PROCEDURE declaration inside WHILE or FOR (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        else
            state.open_subprocedure(tokens[2]);
        //C++ Code
        state.add_code("void "+fix_external_identifier(tokens[2], false)+"(){", line_num);
        return;
    }
    // PARALLEL Declaration
    if(line_like("PARALLEL SUB-PROCEDURE $name", tokens, state) || line_like("PARALLEL SUB $name", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("SUB-PROCEDURE declaration outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(is_subprocedure(tokens[2], state))
            error("Duplicate declaration for SUB-PROCEDURE " + tokens[2] + " (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(state.closing_subprocedure())
            error("SUB-PROCEDURE declaration inside SUB-PROCEDURE (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        else if(state.closing_if())
            error("SUB-PROCEDURE declaration inside IF (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        else if(state.closing_loop())
            error("SUB-PROCEDURE declaration inside WHILE or FOR (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        state.section_state = 3;
        state.open_parallel(tokens[2]);
        state.subprocedures.emplace(tokens[2], vector<string>());
        string id_var_name = "PARALLEL_ID";
        state.subprocedures[tokens[2]].emplace_back(id_var_name);
        state.variables[state.current_subprocedure][id_var_name] = {1};
        state.parallels.emplace(tokens[2], true);
        return;
    }
    if(line_like("END SUB-PROCEDURE", tokens, state) || line_like("END SUB", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("END SUB-PROCEDURE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(!state.closing_subprocedure())
            error("END SUB-PROCEDURE without SUB-PROCEDURE (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C++ Code
        if(state.declaring_parallel)
            state.add_code("return NULL;}", line_num);
        else
            state.add_code("return;}", line_num);
        //Cierro la subrutina
        state.close_subprocedure();
        return;
    }

    // Control Flow Statements
    if(line_like("STORE $expression IN $var", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
        error("STORE statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C++ Code
        string rhand;
        if (is_num_var(tokens[3], state))
        rhand = get_c_number(state, tokens[1]);
        else
        rhand = get_c_string(state, tokens[1]);
        state.add_code(get_c_variable(state, tokens[3]) + " = " + rhand + ";", line_num);
        return;
    }
    if(line_like("IF $condition THEN", tokens, state))
    {
        string condition = get_c_condition(state, vector<string>(tokens.begin()+1, tokens.end()-1));
        if (condition != "[ERROR]") {
            if(!in_procedure_section(state, line_num, current_file))
                error("IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
            //C++ Code
            state.open_if();
            state.add_code("if (" + condition + "){", line_num);
            return;
        }
    }
    if(line_like("ELSE IF $condition THEN", tokens, state))
    {
        string condition = get_c_condition(state, vector<string>(tokens.begin()+2, tokens.end()-1));
        if (condition != "[ERROR]") {
            if(!in_procedure_section(state, line_num, current_file))
                error("ELSE IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
            if(!state.closing_if())
                error("ELSE IF without IF (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
            //C++ Code
            state.add_code("} else if (" + condition + "){", line_num);
            return;
        }
    }
    if(line_like("ELSE", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("ELSE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(!state.closing_if())
            error("ELSE without IF (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C++ Code
        state.open_else();
        state.add_code("}else{", line_num);
        return;
    }
    if(line_like("END IF", tokens, state) || line_like("END-IF", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("END IF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(!state.closing_if() && !state.closing_else())
            error("END IF without IF (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C++ Code
        state.close_if();
        state.add_code("}", line_num);
        return;
    }
    if(line_like("WHILE $condition DO", tokens, state))
    {
        string condition = get_c_condition(state, vector<string>(tokens.begin()+1, tokens.end()-1));
        if (condition != "[ERROR]") {
            if(!in_procedure_section(state, line_num, current_file))
                error("WHILE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
            //C++ Code
            state.open_loop();
            state.add_code("while (" + condition + "){", line_num);
            return;
        }
    }
    if(line_like("REPEAT", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("REPEAT outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(!state.closing_loop())
            error("REPEAT without WHILE or FOR (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C++ Code
        state.close_loop();
        state.add_code("}", line_num);
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
        //C++ Code
        state.add_code("for (" + init + "; " + condition + "; " + increment + ") {", line_num);
        return;
    }
    if(line_like("FOR EACH $anyVar IN $collection DO", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("FOR EACH outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        vector<unsigned int> iteration_type = variable_type(tokens[2], state);
        vector<unsigned int> collected_type = variable_type(tokens[4], state);
        unsigned int collection_type = collected_type.back(); // LIST or MAP
        collected_type.pop_back();
        if(iteration_type != collected_type)
            error("FOR EACH iteration variable type doesn't match collection type (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        state.open_loop();
        //C Code
        string range_var = state.new_range_var();
        string collection = get_c_variable(state, tokens[4]) + ".inner_collection";
        string iteration_var = range_var;
        if (collection_type == 4) {
            iteration_var += ".second";
        }
        state.add_code("for (auto& " + range_var + " : " + collection + ") {", line_num);
        state.add_code(get_c_variable(state, tokens[2]) + " = " + iteration_var + ";", line_num);
        return;
    }
    if(line_like("BREAK", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("BREAK outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(state.open_loops == 0)
            error("BREAK without WHILE or FOR (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C++ Code
        state.add_code("break;", line_num);
        return;
    }
    if(line_like("CONTINUE", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("CONTINUE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(state.open_loops == 0)
            error("CONTINUE without WHILE or FOR (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C++ Code
        state.add_code("continue;", line_num);
        return;
    }
    if(line_like("CALL EXTERNAL $external", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("CALL EXTERNAL outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        state.add_code(fix_external_identifier(tokens[2], false) + "();", line_num);
        //prototype of function defined in extension
        state.add_var_code("void "+fix_external_identifier(tokens[2], false)+"();");
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
            vector<vector<unsigned int>> types;
            for (string & parameter : parameters) {
                if (is_number(parameter))
                    types.push_back({1});
                else if(is_string(parameter))
                    types.push_back({2});
                else if(variable_exists(parameter, state))
                    types.push_back(variable_type(parameter, state));
                else
                    error("CALL with invalid parameter \"" + parameter + "\" (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
            }
            bool correct_types = state.correct_subprocedure_types(subprocedure, types);
            if(!is_subprocedure(subprocedure, state)) {
                if(!correct_types)
                    error("CALL parameter types don't match previous CALL (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
                state.add_expected_subprocedure(subprocedure, fix_identifier(subprocedure, false), types);
            } else {
                if(!correct_types)
                    error("CALL parameter types don't match SUB-PROCEDURE declaration (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
                if(is_parallel(subprocedure, state))
                    error("A PARALLEL SUB-PROCEDURE cannot be invoked using CALL (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
            }
            add_call_code(subprocedure, parameters, state, line_num);
            return;
        }
    }
    if(line_like("IN $num-var CALL PARALLEL $name", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("CALL PARALLEL outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        string subprocedure = tokens[4];
        // By precondition, parameters.size is always 0
        vector<string> parameters;
        vector<vector<unsigned int>> types;
        if(!is_subprocedure(subprocedure, state)) {
            state.add_expected_subprocedure(subprocedure, fix_identifier(subprocedure, false), types);
        } else {
            if(!is_parallel(subprocedure, state))
                error("CALL PARALLEL cannot invoke non-PARALLEL SUB-PROCEDUREs (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        }
        add_call_parallel_code(subprocedure, tokens[1], parameters, state, line_num);
        return;
    }
    if(line_like("STOP PARALLEL $num-var", tokens, state)){
        add_stop_parallel_code(tokens[2], state, line_num);
        return;
    }
    if(line_like("WAIT FOR PARALLEL $num-var", tokens, state)){
        add_wait_parallel_code(tokens[3], state, line_num);
        return;
    }
    if(line_like("RETURN", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("RETURN outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        if(state.current_subprocedure == "")
            error("RETURN found outside subprocedure (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C++ Code
        if(state.declaring_parallel)
            state.add_code("return NULL;", line_num);
        else
            state.add_code("return;", line_num);
        return;
    }
    if(line_like("EXIT", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("EXIT outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C++ Code
        state.add_code("exit(0);", line_num);
        return;
    }
    if(line_like("WAIT $num-expr MILLISECONDS", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("WAIT statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C++ Code
        #if defined(_WIN32)
        state.add_code("_sleep((long int)" + get_c_expression(state, tokens[1]) + ");", line_num);
        #else
        state.add_code("std::this_thread::sleep_for(std::chrono::milliseconds((long int)" + get_c_expression(state, tokens[1]) + "));", line_num);
        #endif
        return;
    }
    if(line_like("GOTO $label", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("GOTO outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        state.add_code("goto label_"+fix_identifier(tokens[1])+";", line_num);
        return;
    }
    if(line_like("LABEL $label", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("LABEL outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        state.add_code("label_"+fix_identifier(tokens[1])+":", line_num);
        return;
    }

    // Arithmetic Statements
    if(line_like("MODULO $num-expr BY $num-expr IN $num-var", tokens, state)) //TODO move this into the standard library
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("MODULO statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C++ Code
        state.add_code(get_c_variable(state, tokens[5]) + " = modulo(" + get_c_expression(state, tokens[1]) + ", " + get_c_expression(state, tokens[3]) + ");", line_num);
        return;
    }
    if(line_like("GET RANDOM IN $num-var", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("RANDOM outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C++ Code
        state.add_code(get_c_variable(state, tokens[3]) + " = get_random();", line_num);
        return;
    }
    if(line_like("FLOOR $num-var", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("FLOOR statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C++ Code
        state.add_code(get_c_variable(state, tokens[1]) + " = floor(" + get_c_variable(state, tokens[1]) +");", line_num);
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
        state.add_code(get_c_variable(state, tokens[1]) + " =" + code + ";", line_num);
        return;
    }

    // Text Statements
    if(line_like("JOIN $expression AND $expression IN $str-var", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("JOIN statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C++ Code
        state.add_code("join(" + get_c_string(state, tokens[1]) + ", " + get_c_string(state, tokens[3]) + ", " + get_c_variable(state, tokens[5]) + ");", line_num);
        return;
    }
    if(line_like("GET CHARACTER AT $num-expr FROM $str-expr IN $str-var", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("GET CHARACTER statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C++ Code
        state.add_code(get_c_variable(state, tokens[7]) + " = charat(" + get_c_expression(state, tokens[5]) + ", " + get_c_expression(state, tokens[3]) + ");", line_num);
        return;
    }
    if(line_like("GET LENGTH OF $str-expr IN $num-var", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("GET LENGTH OF outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C++ Code
        state.add_code(get_c_variable(state, tokens[5]) + " = ((chText)" + get_c_expression(state, tokens[3]) + ").size();", line_num);
        return;
    }
    if(line_like("GET ASCII CHARACTER $num-expr IN $str-var", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("GET ASCII CHARACTER statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C++ Code
        state.add_code(get_c_variable(state, tokens[5]) + " = (char)(" + get_c_expression(state, tokens[3]) + ");", line_num);
        return;
    }
    if(line_like("GET CHARACTER CODE OF $str-expr IN $num-var", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("GET CHARACTER CODE OF statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C++ Code
        state.add_code(get_c_variable(state, tokens[6]) + " = get_char_num(" + get_c_expression(state, tokens[4]) + ");", line_num);
        return;
    }
    if(line_like("STORE QUOTE IN $str-var", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("STORE QUOTE IN statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        state.open_quote = true;
        //C++ Code. More strings will get emitted
        state.add_code(get_c_variable(state, tokens[3]) + " = \"\"", line_num);
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
        //C++ Code
        state.add_code("joinvar = \"\";", line_num);
        for(unsigned int i = 3; i < tokens.size(); ++i){
            state.add_code("join(joinvar, " + get_c_string(state, tokens[i]) + ", joinvar);", line_num);
        }
        state.add_code(get_c_variable(state, tokens[1]) + " = joinvar;", line_num);
        return;
    }

    // I/O Statements
    if(line_like("DISPLAY $display", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("DISPLAY statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C++ Code
        for(unsigned int i = 1; i < tokens.size(); ++i){
            //TODO ADD COLORS HERE
            if(is_scalar_variable(tokens[i], state)){
                state.add_code("cout << " + get_c_variable(state, tokens[i]) + " << flush;", line_num);
            }
            else{
                state.add_code("cout << " + tokens[i] + " << flush;", line_num);
            }
        }
        return;
    }
    if(line_like("ACCEPT $var", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("ACCEPT statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C++ Code
        if(is_num_var(tokens[1], state))
            state.add_code(get_c_variable(state, tokens[1]) + " = input_number();", line_num);
        else
            state.add_code(get_c_variable(state, tokens[1]) + " = input_string();", line_num);
        return;
    }
    if(line_like("EXECUTE $str-expr", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("EXECUTE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C++ Code
        state.add_code("system(" + get_c_char_array(state, tokens[1]) + ");", line_num);
        return;
    }
    if(line_like("EXECUTE $str-expr AND STORE OUTPUT IN $str-var", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("EXECUTE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C++ Code
        state.add_code(get_c_variable(state, tokens[6]) + " = exec(" + get_c_char_array(state, tokens[1]) + ");", line_num);
        return;
    }
    if(line_like("EXECUTE $str-expr AND STORE EXIT CODE IN $num-var", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("EXECUTE outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C++ Code
        state.add_code(get_c_variable(state, tokens[7]) + " = (system(" + get_c_char_array(state, tokens[1]) + ") >> 8) & 0xff;", line_num); //shift wait() val and get lowest 2
        return;
    }
    if(line_like("ACCEPT $str-var UNTIL EOF", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("ACCEPT statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C++ Code
        state.add_code(get_c_variable(state, tokens[1])+" = input_until_eof();", line_num);
        return;
    }
    if(line_like("LOAD FILE $str-expr IN $str-var", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("LOAD FILE statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C++ Code
        state.add_code("load_file(" + get_c_expression(state, tokens[2]) + ", " + get_c_variable(state, tokens[4]) +");", line_num);
        return;
    }
    if(line_like("WRITE $expression TO FILE $str-expr", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("WRITE statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C++ Code
        state.add_code("write_file(" + get_c_expression(state, tokens[4]) + ", " + get_c_expression(state, tokens[1]) + ");", line_num);
        return;
    }
    if(line_like("APPEND $expression TO FILE $str-expr", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("APPEND statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C++ Code
        state.add_code("append_to_file(" + get_c_expression(state, tokens[4]) + ", " + get_c_expression(state, tokens[1]) + ");", line_num);
        return;
    }

    if(line_like("REPLACE $str-expr FROM $str-expr WITH $str-expr IN $str-var", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("REPLACE statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C++ Code
        state.add_code(get_c_variable(state, tokens[7]) + " = str_replace(((chText)" + get_c_expression(state, tokens[3]) + ").str_rep(), ((chText)" + get_c_expression(state, tokens[1]) + ").str_rep(), ((chText)" + get_c_expression(state, tokens[5]) + ").str_rep());", line_num);
        return;
    }
    if(line_like("GET INDEX OF $str-expr FROM $str-expr IN $num-var", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("GET INDEX OF statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C++ Code
        state.add_code(get_c_variable(state, tokens[7]) + " = utf8GetIndexOf(" + get_c_expression(state, tokens[5]) + ", " + get_c_expression(state, tokens[3]) + ");", line_num);
        return;
    }
    if(line_like("COUNT $str-expr FROM $str-expr IN $num-var", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("COUNT statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C++ Code
        state.add_code(get_c_variable(state, tokens[5]) + " = utf8Count(" + get_c_expression(state, tokens[3]) + ", " + get_c_expression(state, tokens[1]) + ");", line_num);
        return;
    }
    if(line_like("SUBSTRING $str-expr FROM $num-expr LENGTH $num-expr IN $str-var", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("SUBSTRING statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C++ Code
        state.add_code("joinvar = " + get_c_expression(state, tokens[1]) + ";", line_num);
        state.add_code(get_c_variable(state, tokens[7]) + " = joinvar.substr(" + get_c_expression(state, tokens[3]) + ", " + get_c_expression(state, tokens[5]) + ");", line_num);
        return;
    }
    if(line_like("TRIM $str-expr IN $str-var", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("TRIM statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C++ Code
        state.add_code(get_c_variable(state, tokens[3]) + " = trimCopy(" + get_c_expression(state, tokens[1]) +  ");", line_num);
        return;
    }
    if(line_like("CLEAR $collection", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("CLEAR statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C++ Code
        state.add_code(get_c_variable(state, tokens[1]) + ".inner_collection.clear();", line_num);
        return;
    }
    if(line_like("COPY $collection TO $collection", tokens, state))
    {
        if(variable_type(tokens[1], state) == variable_type(tokens[3], state)) {
            if(!in_procedure_section(state, line_num, current_file))
            error("COPY statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
            //C++ Code
            state.add_code(get_c_variable(state, tokens[3]) + ".inner_collection = " + get_c_variable(state, tokens[1]) + ".inner_collection;", line_num);
            return;
        }
    }
    if(line_like("GET KEY COUNT OF $map IN $num-var", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("GET KEY COUNT statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C++ Code
        state.add_code(get_c_variable(state, tokens[6]) + " = " + get_c_variable(state, tokens[4]) + ".inner_collection.size();", line_num);
        return;
    }
    if(line_like("GET KEYS OF $map IN $str-list", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("GET KEYS statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C++ Code
        state.add_code("get_indices(" + get_c_variable(state, tokens[5]) + ", " + get_c_variable(state, tokens[3]) + ");", line_num);
        return;
    }
    if(line_like("PUSH MAP TO $map-list", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("PUSH statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C++ Code
        state.add_code(get_c_variable(state, tokens[3]) + ".inner_collection.emplace_back();", line_num);
        return;
    }
    if(line_like("PUSH LIST TO $list-list", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("PUSH statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C++ Code
        state.add_code(get_c_variable(state, tokens[3]) + ".inner_collection.emplace_back();", line_num);
        return;
    }
    if(line_like("PUSH $expression TO $scalar-list", tokens, state))
    {
        // The type of the pushed element must match the collection type
        if(is_num_expr(tokens[1], state) == is_num_list(tokens[3], state)) {
            if(!in_procedure_section(state, line_num, current_file))
            error("PUSH statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
            //C++ Code
            state.add_code(get_c_variable(state, tokens[3]) + ".inner_collection.push_back(" + get_c_expression(state, tokens[1]) + ");", line_num);
            return;
        }else{
            error("List - Value type mismatch (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        }
    }
    if(line_like("GET LENGTH OF $list IN $num-var", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("GET LENGTH OF (list) statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C++ Code
        state.add_code(get_c_variable(state, tokens[5]) + " = " + get_c_variable(state, tokens[3]) + ".inner_collection.size();", line_num);
        return;
    }
    if(line_like("DELETE LAST ELEMENT OF $list", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("DELETE LAST ELEMENT OF statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C++ Code
        state.add_code("if(" + get_c_variable(state, tokens[4]) + ".inner_collection.size() > 0)", line_num);
        state.add_code(get_c_variable(state, tokens[4]) + ".inner_collection.pop_back();", line_num);
        return;
    }
    if(line_like("SPLIT $str-expr BY $str-expr IN $str-list", tokens, state))
    {
        if(!in_procedure_section(state, line_num, current_file))
            error("SPLIT statement outside PROCEDURE section (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        //C++ Code
        state.add_code(get_c_variable(state, tokens[5]) + " = utf8_split_list(" + get_c_expression(state, tokens[1]) + ", " + get_c_expression(state, tokens[3]) + ");", line_num);
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
        vector<string> parameters = state.subprocedures[tokens[4]];
        trim(model_line);
        tokenize(model_line, 0, model_tokens, state.current_file, true, ' ');
        size_t param_count = 0;
        size_t keyword_count = 0;
        string valid_keyword_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        model_line = "";
        for (string & token : model_tokens) {
            if (token == "$") {
                ++param_count;
                if (param_count > parameters.size()) break;
                vector<unsigned int> type = state.variables[tokens[4]][parameters[param_count-1]];
                if (type == vector<unsigned int>{1})
                    model_line += "$num-expr ";
                else if (type == vector<unsigned int>{2})
                    model_line += "$str-expr ";
                else {
                    model_line += "$var-type-";
                    for (size_t i = 0; i < type.size(); ++i) {
                        model_line += to_string(type[i]);
                    }
                    model_line += " ";

                }
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
            tokenize(statement.first, line_num, model_tokens, state.current_file, false, ' ');
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
    tokenize(model_line, 0, model_tokens, state.current_file, false, ' ');
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
            if(!is_scalar_variable(tokens[j], state)) return false;
        }
        else if(model_tokens[i] == "$anyVar") //$anyVar is any variable
        {
            if(!variable_exists(tokens[j], state)) return false;
        }
        else if(model_tokens[i] == "$scalar-map"){ //$scalar-map is TEXT MAP, NUMBER MAP
            if(!is_scalar_map(tokens[j], state)) return false;
        }
        else if(model_tokens[i] == "$num-vec") //$num-vec is NUMBER vector
        {
            if(!is_num_map(tokens[j], state)) return false;
        }
        else if(model_tokens[i] == "$str-vec") //$str-vec is TEXT vector
        {
            if(!is_txt_map(tokens[j], state)) return false;
        }
        else if(model_tokens[i] == "$list"){ //$list is a LIST
            if(variable_type(tokens[j], state).back() != 3) return false;
        }
        else if(model_tokens[i] == "$map"){ //$map is a MAP
            if(variable_type(tokens[j], state).back() != 4) return false;
        }
        else if(model_tokens[i] == "$scalar-list"){ //$scalar-list is a LIST of scalar values
            if(!is_scalar_list(tokens[j], state)) return false;
        }
        else if(model_tokens[i] == "$num-list") //$num-vec is NUMBER list
        {
            if(!is_num_list(tokens[j], state)) return false;
        }
        else if(model_tokens[i] == "$str-list") //$str-vec is TEXT list
        {
            if(!is_txt_list(tokens[j], state)) return false;
        }
        else if(model_tokens[i] == "$list-list") //$str-vec is a LIST of LISTs
        {
            if(!is_list_list(tokens[j], state)) return false;
        }
        else if(model_tokens[i] == "$map-list") //$str-vec is LIST of MAPs
        {
            if(!is_map_list(tokens[j], state)) return false;
        }
        else if(model_tokens[i] == "$collection") //$collection is either a MAP or a LIST
        {
            //if(!is_scalar_map(tokens[j], state) && !is_scalar_list(tokens[j], state) && (variable_type(tokens[j], state).size() < 2)) return false;
            if(variable_type(tokens[j], state).size() < 2) return false;
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
        else if(model_tokens[i].find("$var-type-") == 0) //variable with a given type number
        {
            vector<unsigned int> actual_type = variable_type(tokens[j], state);
            string expected_type = model_tokens[i].substr(10);
            if (actual_type.size() != expected_type.length()) return false;
            for (size_t t = 0; t < actual_type.size(); ++t) {
                if ((int)actual_type[t] != expected_type[t] - '0') return false;
            }
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
            // Skip to the last token (THEN/DO),
            // the condition is validated in get_c_condition
            j = tokens.size() - 1;
            continue;
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

// Returns if the variable is a NUMBER MAP or an access to a multicontainer that results in a NUMBER MAP
bool is_num_map(string & token, compiler_state & state)
{
    vector<unsigned int> type = variable_type(token, state);
    if(type.size() == 2 && type[0] == 1 && type[1] == 4) return true;
    return false;
}

// Returns if the variable is a TEXT MAP or an access to a multicontainer that results in a TEXT MAP
bool is_txt_map(string & token, compiler_state & state)
{
    vector<unsigned int> type = variable_type(token, state);
    if(type.size() == 2 && type[0] == 2 && type[1] == 4) return true;
    return false;
}

// Returns if the variable is a NUMBER LIST or an access to a multicontainer that results in a NUMBER LIST
bool is_num_list(string & token, compiler_state & state)
{
    vector<unsigned int> type = variable_type(token, state);
    if(type.size() == 2 && type[0] == 1 && type[1] == 3) return true;
    return false;
}

// Returns if the variable is a TEXT MAP or an access to a multicontainer that results in a TEXT MAP
bool is_txt_list(string & token, compiler_state & state)
{
    vector<unsigned int> type = variable_type(token, state);
    if(type.size() == 2 && type[0] == 2 && type[1] == 3) return true;
    return false;
}

// Returns if the variable is a NUMBER/TEXT LIST LIST multicontainer or a multicontainer access that results in a LIST of LISTs
bool is_list_list(string & token, compiler_state & state)
{
    vector<unsigned int> type = variable_type(token, state);
    if(type.size() >= 2 && type[type.size()-2] == 3 && type.back() == 3) return true;
    return false;
}

// Returns if the variable is a multicontainer NUMBER/TEXT LIST MAP or a multicontainer access that results in a LIST of MAPs
bool is_map_list(string & token, compiler_state & state)
{
    vector<unsigned int> type = variable_type(token, state);
    if(type.size() >= 2 && type[type.size()-2] == 4 && type.back() == 3) return true;
    return false;
}

// Returns if the variable is a NUMBER MAP or an access to a multicontainer that results in a NUMBER MAP
// or if the variable is a TEXT MAP or an access to a multicontainer that results in a TEXT MAP
bool is_scalar_map(string & token, compiler_state & state)
{
    return is_num_map(token, state) || is_txt_map(token, state);
}

// Returns if the variable is a NUMBER/TEXT MAP MAP multicontainer or a multicontainer access that results in a MAP of MAPs
bool is_map_map(string & token, compiler_state & state)
{
    vector<unsigned int> type = variable_type(token, state);
    if(type.size() >= 2 && type[type.size()-2] == 4 && type.back() == 4) return true;
    return false;
}

// Returns if the variable is a NUMBER LIST or an access to a multicontainer that results in a NUMBER LIST
// or if the variable is a TEXT LIST or an access to a multicontainer that results in a TEXT LIST
bool is_scalar_list(string & token, compiler_state & state)
{
    return is_num_list(token, state) || is_txt_list(token, state);
}

// Checks if token is a NUMBER variable (or an access to a container that results in a NUMBER variable)
bool is_num_var(string & token, compiler_state & state)
{
    return (variable_type(token, state) == vector<unsigned int>{1});
}

// Checks if token is a TEXT variable (or an access to a container that results in a TEXT variable)
bool is_txt_var(string & token, compiler_state & state)
{
    return (variable_type(token, state) == vector<unsigned int>{2});
}

bool is_scalar_variable(string & token, compiler_state & state)
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

// Given a 'full variable' (for example 'bar' or 'foo:0:"hi there":4'), a place to store a variable
// name and a vector of strings to store all indexes, we get the variable name and all its subindexes (if any)
// into the corresponding variables.
void split_vector(string & token, string & var_name, vector<string> & indexes, compiler_state & state)
{
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
    else{
        // If none of the above happened, then our container is formatted correctly. Now
        // it's time to split it. As we can have something like 
        // foo IS NUMBER LIST LIST
        // bar IS NUMBER LIST
        // and try to access foo using a value stored in bar (for example foo:bar:0:1,
        // meaning the foo[bar[0]][1]), we have to know how many indexes our container
        // expects. We tokenize our container using the tokenize function.
        vector<string> tokens;
        tokenize(token, 0, tokens, state.current_file, true, ':');
        // We take the first token as the variable name and then discard it.
        var_name = tokens[0];
        tokens.erase (tokens.begin(), tokens.begin()+1);
        // For each of the remaining, we have to check if its a variable or not. If
        // its a variable name, it may be another container. For each container we
        // find, we append as many indexes that container requires to the previous
        // token we found (the index name). So, if we had foo and bar as stated above,
        // the parsing of foo:bar:0:1 would go like this:
        // - foo is taken as the variable name and thus skipped.
        // - bar is a container that takes one index. We'll then 'skip' one index.
        //   We then push bar to a list of tokens we've already checked.
        // - 1 is an index, and we said we were going to 'skip' one index. What we do
        //   is, instead of pushing 1 to the list of tokens we've already checked, we
        //   append it to the last token we found as an index access. So we take the
        //   'bar' that's already in our checked list and we turn it into 'bar:1'.
        // - 0 is an index. We are not skipping anymore indexes as we already skipped
        //   '1', so we push it into our checked tokens list.
        // Our checked tokens list will end up like {'bar:1', 0}, that are the indexes
        // of foo we are trying to access.
        vector<string> checked_tokens;
        size_t tokens_to_skip = 0;
        for(size_t i = 0; i < tokens.size(); ++i){
            if(is_number(tokens[i]) || is_string(tokens[i])){
                if(tokens_to_skip > 0){
                    checked_tokens.back() += ":" + tokens[i];
                    tokens_to_skip--;
                }else{
                    checked_tokens.push_back(tokens[i]);
                }
            }
            // If the current token is not a number nor a string, it must be a variable
            // name.
            else{
                // We get the types of the current variable.
                vector<unsigned int> cvar_types = variable_type(tokens[i], state);
                // If the variable doesn't exist, we raise an error.
                if(cvar_types == vector<unsigned int>{0}){
                    error("The variable " + tokens[i] + " doesn't exist in " + token + ".");
                }
                // We remove an element of the current variable types as the main type (NUMBER
                // or TEXT) never names a container type and thus cannot be indexed.
                cvar_types.pop_back();
                // If we had to skip this token, we append it to the last token we have.
                if(tokens_to_skip > 0){
                    checked_tokens.back() += ":" + tokens[i];
                    tokens_to_skip--;
                }else{
                    checked_tokens.push_back(tokens[i]);
                }
                // We tell the splitter to skip this many tokens.
                tokens_to_skip += cvar_types.size();
            }
        }
        // After we've got all indexes we needed, we store our checked tokens list into the
        // indexes vector, that is what we are going to return with the variable name.
        indexes = checked_tokens;
    }
}

/*La diferencia entre is_scalar_variable y variable_exists es que is_scalar_variable
 no toma los vectores como una variable, sino los índices de los vectores.
 Variable exists te dice si una variable está declarada como tal, por lo que
 un subíndice de vector no sería una variable.*/
bool variable_exists(string & token, compiler_state & state)
{
    return variable_type(token, state) != vector<unsigned int>{0};
}

bool is_subprocedure(string & token, compiler_state & state)
{
    for(auto & subprocedure : state.subprocedures)
        if(subprocedure.first == token) return true;
    return false;
}

bool is_parallel(string & token, compiler_state & state)
{
    //Precondition: the token passed is the name of an existing sub-procedure
    //              so that is_subprocedure(token, state) == true
    for(auto & subprocedure : state.parallels)
        if(subprocedure.first == token) return subprocedure.second;
    return false;
}

// Given a full variable (with accesses and everything, like foo:0:'hi there' or bar) returns
// the C++ representation of said variable in order to be accessed.
string get_c_variable(compiler_state & state, string & variable)
{
    // We want to get two things in order to create the correct C++ representation
    // of a 'full variable': the variable name and all the indexess we are trying
    // to access that correspond to this particular variable (because we could have
    // foo number list list and bar number list and try to access foo:bar:0:1. In this
    // case, we would be interested in getting 'foo', 'bar:0' and '1'.
    string var_name;
    vector<string> indexes;
    // We use the split_vector function to get these values into our variables.
    split_vector(variable, var_name, indexes, state);
    // We 'fix' the variable name, turning all characters not accepted by C++ into
    // LDPL codes that C++ does accept.
    var_name = fix_identifier(var_name, true, state);
    // If split_vector didn't return any indexes, then we are dealing with a scalar
    // variable. We just return the C++ variable name and we are done.
    if (indexes.empty())
        return var_name;
    // If our indexes vector is not empty, however, we recreate the correct C++
    // container access, with one dimension for each value in our indexes vector.
    for (size_t i = 0; i < indexes.size(); ++i)
        var_name += '[' + get_c_expression(state, indexes[i]) + ']';
    // Once we are done, we return the variable name.
    return var_name;
}

string get_c_expression(compiler_state & state, string & expression)
{
    if(is_scalar_variable(expression, state) || variable_exists(expression, state))
        return get_c_variable(state, expression);
    is_number(expression); //We fix the expression should it be a number literal
    return expression;
}

// text must be a TEXT or a TEXT variable
string get_c_char_array(compiler_state & state, string & text)
{
    if(is_txt_var(text, state))
        return get_c_variable(state, text) + ".str_rep().c_str()";
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

// Returns [ERROR] if invalid condtion, otherwise returns C++ condition
string get_c_condition(compiler_state & state, vector<string> tokens) {
    unsigned int ct = 0;
    string condition = get_c_condition(state, tokens, ct);
    if (ct < tokens.size()) return "[ERROR]";
    return condition;
}

#define MATCH(x) \
    if (ct < tokens.size() && tokens[ct] == x) ct++; \
    else return "[ERROR]";
string get_c_condition(compiler_state & state, vector<string> tokens, unsigned int & ct) {
    if (ct >= tokens.size()) return "[ERROR]";
    string condition;
    if (tokens[ct] == "(") {
        MATCH("(");
        condition = get_c_condition(state, tokens, ct);
        if (condition == "[ERROR]") return condition;
        MATCH(")");
        condition =  "(" + condition + ")";
    } else {
        string first_value = tokens[ct];
        ct++; // We validate the token after we get the second value
        MATCH("IS");

        string rel_op;
        if (tokens[ct] == "EQUAL") {
            MATCH("EQUAL"); MATCH("TO");
            rel_op = "EQUAL TO";
        } else if (tokens[ct] == "NOT") {
            MATCH("NOT"); MATCH("EQUAL"); MATCH("TO");
            rel_op = "NOT EQUAL TO";
        } else if (tokens[ct] == "GREATER") {
            MATCH("GREATER"); MATCH("THAN");
            if (ct+1 < tokens.size() && tokens[ct+1] == "EQUAL") {
                // We check the next token instead of the curent one
                // because "OR" could be a variable after "GREATER THAN"
                MATCH("OR"); MATCH("EQUAL"); MATCH("TO");
                rel_op = "GREATER THAN OR EQUAL TO";
            } else {
                rel_op = "GREATER THAN";
            }
        } else if (tokens[ct] == "LESS") {
            MATCH("LESS"); MATCH("THAN");
            if (ct+1 < tokens.size() && tokens[ct+1] == "EQUAL") {
                // We check the next token instead of the curent one
                // because "OR" could be a variable after "LESS THAN"
                MATCH("OR"); MATCH("EQUAL"); MATCH("TO");
                rel_op = "LESS THAN OR EQUAL TO";
            } else {
                rel_op = "LESS THAN";
            }
        } else {
            return "[ERROR]";
        }

        string second_value = tokens[ct];
        ++ct;

        string type;
        if(is_num_expr(first_value, state) && is_num_expr(second_value, state))
            type = "NUMBER";
        else if(is_txt_expr(first_value, state) && is_txt_expr(second_value, state))
            type = "TEXT";
        else if(is_num_map(first_value, state) && is_num_map(second_value, state))
            type = "NUMBER MAP";
        else if(is_txt_map(first_value, state) && is_txt_map(second_value, state))
            type = "TEXT MAP";
        else if(is_num_list(first_value, state) && is_num_list(second_value, state))
            type = "NUMBER LIST";
        else if(is_txt_list(first_value, state) && is_txt_list(second_value, state))
            type = "TEXT LIST";
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
            else if(rel_op == "LESS THAN")
                condition = first_value + " < " + second_value;
            else if (rel_op == "GREATER THAN OR EQUAL TO")
                condition = "(" + first_value + " > " + second_value
                + " || num_equal(" + first_value + ", " + second_value + "))";
            else
                condition = "(" + first_value + " < " + second_value
                + " || num_equal(" + first_value + ", " + second_value + "))";
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
            if( rel_op == "EQUAL TO")
                condition =  first_value + " == " + second_value;
            else if (rel_op == "NOT EQUAL TO")
                condition = first_value + " != " + second_value;
            else // >, >, <= and >= are only valid in NUMBER and TEXT
                return "[ERROR]";
        }

        if (ct < tokens.size() && (tokens[ct] == "AND" || tokens[ct] == "OR")) {
            if (tokens[ct] == "AND")
                condition += " && ";
            else if(tokens[ct] == "OR")
                condition += " || ";
            ++ct;
            string next_condition = get_c_condition(state, tokens, ct);
            if (next_condition == "[ERROR]") return next_condition;
            condition += next_condition;
        }
    }
    return condition;
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

// Return the number of the type or {0} if the variable doesn't exist. This function can take full variables (foo:0:"hi"). Returns all the types of the variable.
// If foo is number map list {1, 4, 3} and we just pass it foo:0, it will return {1, 4}, that is the type foo:0 has.
vector<unsigned int> variable_type(string & token, compiler_state & state) {
    // Variables can have mixed types. For example, a LIST of MAPS of NUMBERS called foo
    // is a NUMBER when you access both containers (foo:0:"hi")
    // a NUMBER MAP when you access just the list (foo:0)
    // or a NUMBER MAP LIST when you access nothing (foo)
    // So we first split the full variable with accesses and everything into tokens by :
    vector<string> tokens;
    string varName = "";
    tokenize(token, 0, tokens, state.current_file, true, ':');
    // So, for example, foo:0:"hi" will be split into {foo, 0, "hi"}
    // We take the first element as the variable name
    varName = tokens[0];
    // Then we check if the variable exists. If it does, we store its types in a variable.
    vector<unsigned int> types;
    if (state.variables[state.current_subprocedure].count(varName) > 0)
        types = state.variables[state.current_subprocedure][varName];
    else if (state.variables[""].count(varName) > 0)
        types = state.variables[""][varName];
    // If the variable wasn't found, we return {0}
    else return {0};
    // If it was found, though, we want to get its current type.
    // If, in the example above, we had foo:0, then our current type would be {1, 4}
    // meaning, a NUMBER (1) MAP (4). But the type the variable has stored is {1, 4, 3},
    // because it is a NUMBER MAP LIST (LIST is 3). We have to remove all accessed elements
    // from the type vector ({1, 4, 3} would turn into {1, 4} because we accessed the list
    // when we did :0). The number of elements to pop from the vector is equal to the number
    // of : found within the full variable (foo:0). As we've already splitted the variable
    // into tokens, the number of elements to pop from the vector is equal to the number of
    // tokens we have minus one.
    // If the container access should contain other container accesses (for example foo:bar:0),
    // the thing changes, and we must make sure to discard those indexes that access the other
    // containers and not the one we are trying to get the types of.
    size_t tokensToSkip = 0;
    for(size_t i = 1; i < tokens.size(); ++i){
        // If the current token is a scalar literal, we can skip it safely.
        if(is_number(tokens[i]) || is_string(tokens[i])){
            if(tokensToSkip > 0) tokensToSkip--;
        }
        // If it's not, then it must be a variable name.
        else{
            // If the variable doesn't exist in the current context, we rise an error.
            if(state.variables[state.current_subprocedure].count(tokens[i]) == 0 && state.variables[""].count(tokens[i]) == 0){
                error("The variable " + tokens[i] + " used in " + token + " doesn't exist.");
            }
            vector<unsigned int> cvar_types = variable_type(tokens[i], state);
            if (cvar_types.size() > 1) {
                // If the variable exists and is a container, then we skip as many tokens as that variable takes.
                tokensToSkip += cvar_types.size() - 1;
            } else {
                // If the variable exists and is a scalar, we can skip it
                if(tokensToSkip > 0) tokensToSkip--;
            }
        }
        if (tokensToSkip == 0) types.pop_back();
    }
    // We return {0} if there is an incomplete container access
    // that must be complete to resolve as a scalar index
    if (tokensToSkip > 0) return {0};
    // Now we have the types and can return them.
    return types;
}

// This is called when we know all parameters of a subprocedure
void open_subprocedure_code(compiler_state & state, unsigned int line_num, string & current_file) {
    string name = state.current_subprocedure;
    vector<string> & parameters = state.subprocedures[name];
    vector<vector<unsigned int>> types;
    string code;
    if(is_parallel(name, state)){
        code = "void * " +fix_identifier(name, false)+ "(void * thread_id){";
        state.add_code(code, line_num);
        string id_var_name = "PARALLEL_ID";
        id_var_name = fix_identifier(id_var_name, true, state);
        code = "ldpl_number " + id_var_name + "= (long long int) thread_id;";
        state.add_code(code, line_num);
    }else{
        code = "void " +fix_identifier(name, false)+ "(";
        for (size_t i = 0; i < parameters.size(); ++i) {
            string identifier = fix_identifier(parameters[i], true, state);
            string type = state.get_c_type(state.variables[name][parameters[i]]);
            code += type + " & " + identifier;
            if (i < parameters.size() - 1) code += ", ";
            types.push_back(state.variables[name][parameters[i]]);
        }
        if (!state.correct_subprocedure_types(name, types))
            error("SUB-PROCEDURE declaration parameter types doesn't match previous CALL (\033[0m" + current_file + ":"+ to_string(line_num)+"\033[1;31m)");
        code += "){";
        state.add_code(code, line_num);
    }
    state.remove_expected_subprocedure(name);
}

void add_call_code(string & subprocedure, vector<string> & parameters, compiler_state & state, unsigned int line_num) {
    string current_file = state.current_file;
    string code = fix_identifier(subprocedure, false) + "(";
    for (size_t i = 0; i < parameters.size(); ++i) {
        if (is_number(parameters[i]) || is_string(parameters[i])) {
            // C++ doen't allow passing literals in  reference parameters, we create vars for them
            string literal_paramater_var = state.new_literal_parameter_var();
            state.add_code((is_number(parameters[i]) ? "ldpl_number " : "chText ")
                           + literal_paramater_var + " = " + parameters[i] + ";", line_num);
            code += literal_paramater_var;
        } else {
            code += get_c_variable(state, parameters[i]);
        }
        if (i < parameters.size() - 1) code += ", ";
    }
    code += ");";
    state.add_code(code, line_num);
}

void add_call_parallel_code(string & subprocedure, string & var_name, vector<string> & parameters, compiler_state & state, unsigned int line_num)
{
    string current_file = state.current_file;
    string code;
    code = "pthread_create(&ldpl_thread_num, NULL, ";
    code += fix_identifier(subprocedure, false) + ", ";
    code += "(void *) ++ldpl_pthread_count);\n";
    state.add_code(code, line_num);
    code = get_c_variable(state, var_name) + " = ldpl_pthread_count;\n";
    code += "ldpl_thread_numbers[ldpl_pthread_count] = ldpl_thread_num;\n";
    state.add_code(code, line_num);
}

void add_stop_parallel_code(string & var_name, compiler_state & state, unsigned int line_num)
{
    string code = "if(ldpl_thread_numbers.find(" + get_c_variable(state, var_name) + ") != ldpl_thread_numbers.end()){\n";
    code += "pthread_cancel(ldpl_thread_numbers[" + get_c_variable(state, var_name) + "]);\n";
    code += "ldpl_thread_numbers.erase(" + get_c_variable(state, var_name) + ");\n";
    code += "}";
    state.add_code(code, line_num);
}

void add_wait_parallel_code(string & var_name, compiler_state & state, unsigned int line_num)
{
    string code = "if(ldpl_thread_numbers.find(" + get_c_variable(state, var_name) + ") != ldpl_thread_numbers.end()){\n";
    code += "pthread_join(ldpl_thread_numbers[" + get_c_variable(state, var_name) + "], NULL);\n";
    code += "ldpl_thread_numbers.erase(" + get_c_variable(state, var_name) + ");\n";
    code += "}";
    state.add_code(code, line_num);
}

// https://sourceforge.net/p/predef/wiki/OperatingSystems/
string current_os() {
#if defined(__linux__)
    return "LINUX";
#elif defined(__APPLE__)
    return "MACOS";
#elif defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__) || defined(__DragonFly__)
    return "BSD";
#elif defined(__ANDROID__)
    return "ANDROID";
#elif defined(__EMSCRIPTEN__)
    return "EMSCRIPTEN";
#else
    return "UNKNOWN";
#endif
}
