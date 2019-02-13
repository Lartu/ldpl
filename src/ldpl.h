#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stack>
#include <queue>
#include <locale>
#include "nvm.h"
#include "cpptrim.h"

using namespace std;

//TODO: Change vectors to maps
struct compiler_state{
    unsigned int section_state = 0; //0 no section, 1 data, 2 procedure
    unsigned int current_line = 0;
	string current_file = "";
    //Code to output (normally ASM or NVM)
    vector<string> output_code;
    //variables
    vector<pair<string, unsigned int>> variables;
    //1 number, 2 text, 3 number vector, 4 text vector
    vector<string> subprocedures;
    void add_code(string code){
        this->output_code.push_back(code);
    }
    string open_subprocedure = "";
    int if_number = 0;
    stack<int> if_stack;
    int add_if(){
        ++if_number;
        if_stack.push(if_number);
        if_stack.push(if_number);
        return if_number;
    }
    int while_number = 0;
    stack<int> while_stack;
    int add_while(){
        ++while_number;
        while_stack.push(while_number);
        return while_number;
    }
    stack<string> working_dir;
};

void error(const string & msg);
void compile(vector<string> & lines, compiler_state & state);
void tokenize(string & line, unsigned int line_num, vector<string> & tokens, string & current_file);
void split_vector(string & line, queue<string> & tokens);
void compile_line(vector<string> & tokens, unsigned int line_num, compiler_state & state);
bool line_like(string model_line, vector<string> tokens, compiler_state & state); //Important to pass tokens by copy
bool is_number (string number);
bool is_natural (string number);
bool is_string(string & token);
bool is_num_var(string & token, compiler_state & state);
bool is_txt_var(string & token, compiler_state & state);
bool is_variable(string & token, compiler_state & state);
bool variable_exists(string & token, compiler_state & state);
bool is_subprocedure(string & token, compiler_state & state);
void get_var_value(compiler_state & state, string & variable);
void set_var_value(compiler_state & state, string & variable);
void capitalize_tokens(vector<string> & tokens);
void load_and_compile(string & filename, compiler_state & state);
void replace_whitespace(string & code);