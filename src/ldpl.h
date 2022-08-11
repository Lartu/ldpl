/* --- STD Includes --- */
#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <locale>
#include <map>
#include <memory>
#include <queue>
#include <sstream>
#include <stack>
#include <string>
#include <vector>

/* --- Namespace Usage --- */
using namespace std;

/* --- Custom Libraries --- */
#include "libraries/cpptrim.h"
#include "libraries/vector_contains.h"

/* --- OS Dependant Libraries --- */
#ifdef _WIN32	
    #include <windows.h>
#else	
    #include <sys/wait.h>
#endif

/* --- Static Builds? --- */
#if !defined(__APPLE__) && !defined(__ANDROID__)
#define STATIC_BUILDS 1
#endif

/* --- Global Variables --- */
bool show_ir = false; //Show internal representation?
string & escape_c_quotes(string & str);
vector<string> extensions; //C++ extensions to add to the file
vector<string> extension_flags; //Flags to pass to the C++ Compiler

/* --- Custom Data Types --- */
#include "data_types/code_location.h"
#include "data_types/compiler_state.h"

/* --- Function Declarations --- */
void displayVersionInfo();
void displayHelpInfo();
string exec(const char *cmd);
string expandHomeDirectory(string &filename);
void bullet_msg(const string & msg);
void warning(const string & msg);
void error(const string & msg);
void compile(vector<string> & lines, compiler_state & state);
void tokenize(string & line, vector<string> & tokens, code_location & where, bool uppercase, char splitChar);
void split_vector(string & token, string & var_name, vector<string> & indexes, compiler_state & state);
void compile_line(vector<string> & tokens, compiler_state & state);
bool line_like(string model_line, vector<string> & tokens, compiler_state & state); //Important to pass tokens by copy
bool is_number(string & number);
bool is_natural(string number);
bool is_label(string & token);
bool is_math_symbol(string & symbol);
bool is_string(string & token);
bool is_num_var(string & token, compiler_state & state);
bool is_txt_var(string & token, compiler_state & state);
bool is_scalar_variable(string & token, compiler_state & state);
bool is_num_expr(string & token, compiler_state & state);
bool is_txt_expr(string & token, compiler_state & state);
bool is_expression(string & token, compiler_state & state);
bool is_num_map(string & token, compiler_state & state);
bool is_txt_map(string & token, compiler_state & state);
bool is_num_list(string & token, compiler_state & state);
bool is_txt_list(string & token, compiler_state & state);
bool is_list_list(string & token, compiler_state & state);
bool is_map_map(string & token, compiler_state & state);
bool is_map_list(string & token, compiler_state & state);
bool is_scalar_map(string & token, compiler_state & state);
bool is_scalar_list(string & token, compiler_state & state);
bool is_external(string & token, compiler_state & state);
bool variable_exists(string & token, compiler_state & state);
bool is_subprocedure(string & token, compiler_state & state);
string get_c_variable(compiler_state & state, string & variable);
string get_c_expression(compiler_state & state, string & expression);
string get_c_char_array(compiler_state & state, string & text);
string get_c_string(compiler_state & state, string & expression);
string get_c_number(compiler_state & state, string & expression);
string get_c_condition(compiler_state & state, vector<string> tokens);
string get_c_condition(compiler_state & state, vector<string> tokens, unsigned int & ct);
string & escape_c_quotes(string & str);
void load_and_compile(string & filename, compiler_state & state);
void accept_and_compile(compiler_state & state);
void replace_whitespace(string & code);
string fix_external_identifier(string identifier, bool isVariable);
string fix_identifier(string id, bool isv, compiler_state & s);
string fix_identifier(string identifier, bool isVariable);
string fix_identifier(string identifier);
bool in_procedure_section(compiler_state & state);
vector<unsigned int> variable_type(string & token, compiler_state & state);
void open_subprocedure_code(compiler_state & state);
void add_call_code(string & subprocedure, vector<string> & parameters, compiler_state & state);
void add_call_parallel_code(string & subprocedure, string & var_name, vector<string> & parameters, compiler_state & state);
string current_os();
void add_stop_parallel_code(string & var_name, compiler_state & state);
void add_wait_parallel_code(string & var_name, compiler_state & state);
bool is_parallel(string & token, compiler_state & state);
bool is_map(string &token, compiler_state &state);
void badcode(const string &msg, const code_location where);
