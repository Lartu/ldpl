// +=============================================+
// |                                             |
// |          The __    ____  ____  __           |
// |             / /   / __ \/ __ \/ /           |
// |            / /   / / / / /_/ / /            |
// |           / /___/ /_/ / ____/ /___          |
// |          /_____/_____/_/   /_____/          |
// |               Programming Language          |
// |          http://www.ldpl-lang.org/          |
// |                                             |
// +=============================================+

// +------------------+
// | --- Includes --- |
// +------------------+
#include "ldpl.h"

#include <memory>

#include "aux/aux_c_format.cpp"     // Contains auxiliary functions that turn LDPL identifiers / expressions into C / C++ identifiers / expressions
#include "aux/aux_code.cpp"         // Contains auxiliary functions that add C++ code to the generated C++ file
#include "aux/aux_compile_line.cpp" // Contains the behemoth compile_line function that pattern-matches lines and compiles them
#include "aux/aux_container.cpp"    // Contains auxiliary functions to split and access [multi]containers
#include "aux/aux_format.cpp"       // Contains auxiliary functions that do text or number formatting
#include "aux/aux_info.cpp"         // Contains auxiliary functions that display information about the compiler and the system
#include "aux/aux_line_like.cpp"    // Contains the line_like function used for pattern matching lines
#include "aux/aux_state.cpp"        // Contains auxiliary functions that check the current compilation state
#include "aux/aux_tokenizer.cpp"    // Contains functions used to tokenize an LDPL source file
#include "aux/aux_typecheck.cpp"    // Contains auxiliary functions to check data type compliance

// +----------------------------+
// | --- Main Aux Functions --- |
// +----------------------------+

string exec(const char *cmd)
{
    // -- Executes a command passed as a string --
    array<char, 128> buffer;
    string result;
    unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe)
    {
        throw runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
    {
        result += buffer.data();
    }
    return result;
}

void load_and_compile(string &filename, compiler_state &state)
{
    // -- Loads a file and sends it to compilation --
    filename = expandHomeDirectory(filename);
    state.where.current_file = filename;
    // Accept input from stdin
    ifstream file(filename);
    // Fail if the file couldn't be loaded
    if (!file.is_open())
        error("The file '" + filename + "' couldn't be loaded.");
    // Get file contents
    vector<string> lines;
    string line = "";
    while (getline(file, line))
    {
        replace_whitespace(line);
        lines.push_back(line);
    }
    bullet_msg("Compiling " + filename);
    compile(lines, state);
}

void accept_and_compile(compiler_state &state)
{
    // -- Accepts LDPL source code via standard input and sends it to compilation
    // -- Get file contents
    vector<string> lines;
    state.where.current_file = "standard input";
    string line = "";
    while (getline(cin, line))
    {
        replace_whitespace(line);
        lines.push_back(line);
    }
    bullet_msg("Compiling");
    compile(lines, state);
}

void compile(vector<string> &lines, compiler_state &state)
{
    // -- Takes a vector of LDPL source lines and compiles them to C++ --
    // For each line in the source code
    for (size_t line_num = 1; line_num <= lines.size(); ++line_num)
    {
        string &line = lines[line_num - 1];
        state.where.current_line = line_num;

        if (state.open_quote)
        {
            // Check for END QUOTE first
            if (line.size() >= 9 /*&& (line[0] == 'E' || line[0] == 'e')*/)
            {
                string upper = "";
                for (char c : line)
                    upper += toupper(c);
                trim(upper);
                if (upper == "END QUOTE")
                {
                    state.open_quote = false;
                    state.trim_quote_lines = false;
                    // Kill final newline. Programs can add crlf if needed.
                    string &prev = state.current_subprocedure != ""
                                       ? state.subroutine_code.back()
                                       : state.output_code.back();
                    size_t pos = prev.rfind("\\n");
                    if (pos != string::npos)
                        prev.erase(pos, 2);
                    prev += ";";
                    continue;
                }
            }

            // No END QUOTE, emit the line as C++
            if (!state.trim_quote_lines)
            {
                state.add_code("\"\" \"" + escape_c_quotes(escape_c_backslashes(line)) + "\\n\"", state.where);
            }
            else
            {
                trim(line);
                state.add_code("\"\" \"" + escape_c_quotes(escape_c_backslashes(line)) + "\\n\"", state.where);
            }
            continue;
        }

        trim(line);
        // Split line in various tokens
        vector<string> tokens;
        tokenize(line, tokens, state.where, true, ' ');
        for (string &token : tokens)
            if (token == "CRLF" || token == "LF")
                token = "\"\\n\"";
        if (tokens.size() == 0)
            continue;
        compile_line(tokens, state);
    }
    if (state.open_quote)
        error("a QUOTE block was not terminated.");
    if (state.closing_subprocedure())
        error("a SUB-PROCEDURE block was not terminated.");
    if (state.closing_if())
        error("a IF block was not terminated.");
    if (state.closing_loop())
        error("a WHILE or FOR block was not terminated.");
}

// +-----------------------+
// | --- Main Function --- |
// +-----------------------+

int main(int argc, const char *argv[])
{
    // Get command line arguments as string vector
    vector<string> args(argv + 1, argv + argc);

    // Check if the user requested version or help information
    if (contains_any(args, {"-v", "--version"}))
    {
        displayVersionInfo();
        return 0;
    }
    else if (contains_any(args, {"-h", "--help"}))
    {
        displayHelpInfo();
        return 0;
    }

    // Define local variables that will be used for compilation
    compiler_state
        state; // Compiler state (holds variables, sections, functions, etc)
    vector<string> files_to_compile;
#ifdef STATIC_BUILDS
    bool no_static = false;
#endif
    string output_filename = "";
    string final_filename = "";

    // Check command line arguments
    if (args.size() >= 1)
    {
        for (string &arg : args)
        {
            if (arg.size() >= 1 && arg[0] != '-')
            {
                if (output_filename == "")
                    output_filename = arg;
                if (files_to_compile.size() > 0)
                {
                    warning(
                        "passing multiple LDPL source files to the\ncompiler is "
                        "deprecated and may be removed in the future.\nPlease use the "
                        "IMPORT statement instead.");
                }
                files_to_compile.push_back(arg);
            }
            else if (arg == "-r")
            {
                show_ir = true;
            }
#ifdef STATIC_BUILDS
            else if (arg == "-n" || arg == "--non-static")
            {
                no_static = true;
            }
#endif
            else if (arg.substr(0, 3) == "-o=")
            {
                final_filename = arg.substr(3);
            }
            else if (arg.substr(0, 3) == "-i=")
            {
                if (((arg.length() > 5) &&
                     (0 == arg.compare(arg.length() - 5, 5, ".ldpl"))) ||
                    ((arg.length() > 4) &&
                     (0 == arg.compare(arg.length() - 4, 4, ".lsc"))))
                {
                    if (files_to_compile.size() > 0)
                    {
                        error("To compile more than one file, use the IMPORT statement.");
                    }
                    files_to_compile.push_back(arg.substr(3));
                }
                else
                {
                    // pass everything but .ldpl and .lsc files to the c++ compiler
                    extensions.push_back(arg.substr(3)); // kill -i= prefix
                }
            }
            else if (arg.substr(0, 3) == "-f=")
            {
                // pass flags to the c++ compiler (for example -f=-lSDL)
                extension_flags.push_back(arg.substr(3));
            }
            else if (arg == "-c")
            {
                files_to_compile.push_back(arg);
            }
            else
            {
                cout << "Unknown option: " << arg << endl;
                cout << "Try 'ldpl -h' for more information." << endl;
                return 0;
            }
        }
    }

    // Add default initialization code to the generated source
    state.add_code("int main(int argc, char *argv[]){");
    state.add_code("cout.precision(numeric_limits<ldpl_number>::digits10);");

    // Add default variable declaration code to the generated code
    state.variables[""]["ARGV"] = {2, 3}; // List of text
    state.add_var_code("ldpl_list<graphemedText> " + fix_identifier("ARGV", true) + ";");
    state.variables[""]["ERRORCODE"] = {1}; // Declared in ldpl_lib.cpp
    state.variables[""]["ERRORTEXT"] = {2}; // Declared in ldpl_lib.cpp
    state.add_code("for(int i = 1; i < argc; ++i)");
    state.add_code(fix_identifier("ARGV", true) +
                   ".inner_collection.push_back(argv[i]);");

    // Fail if no filename was passed
    if (files_to_compile.size() == 0)
        error("no source file specified.");

    // For each filename passed, compile each file into one big code
    for (string &filename : files_to_compile)
    {
        // Reset state section for this file
        state.section_state = 0;
        // If the filename was a filename or the user requested standard input
        if (filename != "-c")
        {
            bullet_msg("Loading " + filename);
            load_and_compile(filename, state);
        }
        else
        {
            bullet_msg("Waiting for standard input...");
            accept_and_compile(state);
        }
        // Fail if no procedure section was found
        if (state.section_state < 2)
            error("PROCEDURE section not found" +
                  (filename == "-c" ? "." : " in file '" + filename + "'."));
    }

    // Add return code to the generated main function
    state.add_code("return 0; \n}");

    // Add include for extensions
    if (!extensions.empty())
    {
        for (string &extension : extensions)
        {
            bullet_msg("Including C++ extension " + extension);
            state.add_code("#include \"" + extension + "\"");
        }
    }

    // If an expected subprocedure was not declared, raise an error
    if (state.expected_subprocedures.size() > 0)
        error("the subprocedure " + state.expected_subprocedures[0].first +
              " is called but never declared.");

    // If only to print the generated code (IR) was required
    if (show_ir)
    {
        cout << "#include \"" << LDPLLIBLOCATION << "/ldpl_lib.cpp\""
             << endl; // Add LDPL library
        for (string line : state.variable_code)
            cout << line << endl;
        for (string line : state.subroutine_code)
            cout << line << endl;
        for (string line : state.output_code)
            cout << line << endl;
        exit(0);
    }

    // Otherwise, save the generated code
    ofstream myfile;
    myfile.open("ldpl-temp.cpp");
    myfile << "#include \"" << LDPLLIBLOCATION << "/ldpl_lib.cpp\""
           << endl; // Add LDPL library
    for (string line : state.variable_code)
        myfile << line << endl;
    for (string line : state.subroutine_code)
        myfile << line << endl;
    for (string line : state.output_code)
        myfile << line << endl;
    myfile.close();

    // Generate output filename if not set by -o=
    if (final_filename.empty())
    {
        for (unsigned int i = 0; i < output_filename.size(); ++i)
        {
            if (output_filename[i] != '.')
                final_filename += output_filename[i];
            else
                break;
        }
        if (final_filename.size() == 0)
            final_filename = "ldpl-output";
        final_filename += "-bin";
    }

    // Generate the C++ compilation command
    string compile_line =
        "c++ ldpl-temp.cpp -std=c++11 -w -O3 -o " + final_filename;
#ifdef STATIC_BUILDS
    if (!no_static)
        compile_line += " -static-libgcc -static-libstdc++ ";
#endif
    if (!extension_flags.empty())
    {
        for (string &flag : extension_flags)
        {
            bullet_msg("using C++ switch " + flag);
            compile_line += " " + flag;
        }
    }

    // Compile the C++ code
    bullet_msg("Building " + final_filename);
    int compiled = system(compile_line.c_str());
#if defined(_WIN32)
    system("del ldpl-temp.cpp");
#else
    system("rm ldpl-temp.cpp");
#endif

    // Output if the code was compiled or not
    if (compiled == 0)
    {
        bullet_msg("Saved as " + final_filename);
        bullet_msg("\033[32;1mFile(s) compiled successfully.\033[0m");
    }
    else
    {
        error("compilation failed.");
    }
}
