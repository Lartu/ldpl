#ifndef LDPL_H
#define LDPL_H
/* --- STD Includes --- */
#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <locale>
#include <map>
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

/* --- Global Variables --- */
bool show_ir = false; // Show internal representation?
string &escape_c_quotes(string &str);
vector<string> extensions;      // C++ extensions to add to the file
vector<string> extension_flags; // Flags to pass to the C++ Compiler

/* --- Custom Data Types --- */
#include "data_types/code_location.h"
#include "data_types/compiler_state.h"

/* --- Function Declarations --- */
void displayVersionInfo();
void displayHelpInfo();
string exec(const char *cmd);
string expandHomeDirectory(string &filename);
void bullet_msg(const string &msg);
void warning(const string &msg);
void error(const string &msg);
void compile(vector<string> &lines, compiler_state &state);
void tokenize(const string &line, vector<string> &tokens, code_location &where, bool uppercase, char splitChar);
void split_vector(const string &token, string &var_name, vector<string> &indexes, compiler_state &state);
void compile_line(vector<string> &tokens, compiler_state &state);
bool line_like(string model_line, const vector<string> &tokens, compiler_state &state); // Important to pass tokens by copy
bool is_number(const string &number);
bool is_natural(const string number);
bool is_string(const string &token);
bool is_num_var(const string &token, compiler_state &state);
bool is_txt_var(const string &token, compiler_state &state);
bool is_scalar_variable(const string &token, compiler_state &state);
bool is_num(const string &token, compiler_state &state);
bool is_txt(const string &token, compiler_state &state);
bool is_scalar_value(const string &token, compiler_state &state);
bool is_value(const string &token, compiler_state &state);
bool is_num_map(const string &token, compiler_state &state);
bool is_txt_map(const string &token, compiler_state &state);
bool is_scalar_map(const string &token, compiler_state &state);
bool variable_exists(const string &token, compiler_state &state);
bool is_subprocedure(const string &token, compiler_state &state);
string get_c_variable(compiler_state &state, string &variable);
string get_c_expression(compiler_state &state, string &expression);
string get_c_char_array(compiler_state &state, string &text);
string get_c_string(compiler_state &state, string &expression);
string get_c_number(compiler_state &state, string &expression);
string get_c_condition(compiler_state &state, vector<string> tokens);
string get_c_condition(compiler_state &state, vector<string> tokens, unsigned int &ct);
string &escape_c_quotes(string &str);
string &escape_c_backslashes(string &str);
void load_and_compile(string &filename, compiler_state &state);
void accept_and_compile(compiler_state &state);
void replace_whitespace(string &code);
string fix_external_identifier(string identifier, bool isVariable);
string fix_identifier(string id, bool isv, compiler_state &s);
string fix_identifier(string identifier, bool isVariable);
string fix_identifier(string identifier);
bool in_procedure_section(compiler_state &state);
vector<unsigned int> variable_type(const string &token, compiler_state &state);
void open_subprocedure_code(compiler_state &state);
void add_call_code(string &subprocedure, vector<string> &parameters, compiler_state &state);
string current_os();
bool is_map(const string &token, compiler_state &state);
void badcode(const string &msg, const code_location where);
#endif