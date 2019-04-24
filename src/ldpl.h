#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stack>
#include <map>
#include <queue>
#include <locale>
#include "cpptrim.h"
#include <sstream>

using namespace std;

#if !defined(__APPLE__) && !defined(__ANDROID__)
#define STATIC_BUILDS 1
#endif

//TODO: Change vectors to maps
struct compiler_state{
    unsigned int section_state = 0; //0 no section, 1 data, 2 procedure
    unsigned int current_line = 0;
    string current_file = "";
    //Code to output (plain C code)
    vector<string> variable_code;
    vector<string> output_code;
    vector<string> subroutine_code; //code outside main()
    //variables
    map<string, unsigned int> variables;
    map<string, bool> externals; //variables defined in c++ extensions
    //1 number, 2 text, 3 number vector, 4 text vector
    vector<string> subprocedures;
    void add_var_code(string code){
        this->variable_code.push_back(code);
    }
    void add_code(string code){
        if(!in_subprocedure)
            this->output_code.push_back(code);
        else
            this->subroutine_code.push_back(code);
    }
    bool open_quote = false;
    bool in_subprocedure = false;
    int open_whiles = 0;
    stack<int> block_stack; //0 sub-procedure, 1 if, 2 while
    void open_subprocedure(){
        in_subprocedure = true;
        block_stack.push(0);
    }
    void close_subprocedure(){
        in_subprocedure = false;
        block_stack.pop();
    }
    bool closing_subprocedure(){
        return !block_stack.empty() && block_stack.top() == 0;
    }
    void open_if(){
        block_stack.push(1);
    }
    void close_if(){
        block_stack.pop();
    }
    bool closing_if(){
        return !block_stack.empty() && block_stack.top() == 1;
    }
    void open_while(){
        ++open_whiles;
        block_stack.push(2);
    }
    void close_while(){
        --open_whiles;
        block_stack.pop();
    }
    bool closing_while(){
        return !block_stack.empty() && block_stack.top() == 2;
    }
    stack<string> working_dir;
};

void error(const string & msg);
void compile(vector<string> & lines, compiler_state & state);
void tokenize(string & line, unsigned int line_num, vector<string> & tokens, string & current_file);
void split_vector(string & line, queue<string> & tokens);
void compile_line(vector<string> & tokens, unsigned int line_num, compiler_state & state);
bool line_like(string model_line, vector<string> & tokens, compiler_state & state); //Important to pass tokens by copy
bool is_number(string number);
bool is_natural(string number);
bool is_label(string & token);
bool is_string(string & token);
bool is_vector_index(string & token);
bool is_num_var(string & token, compiler_state & state);
bool is_txt_var(string & token, compiler_state & state);
bool is_variable(string & token, compiler_state & state);
bool is_external(string & token, compiler_state & state);
bool variable_exists(string & token, compiler_state & state);
bool is_subprocedure(string & token, compiler_state & state);
string get_c_variable(compiler_state & state, string & variable);
string get_c_expression(compiler_state & state, string & expression);
string & escape_c_quotes(string & str);
void capitalize_tokens(vector<string> & tokens);
void load_and_compile(string & filename, compiler_state & state);
void replace_whitespace(string & code);
string fix_external_identifier(string identifier, bool isVariable);
string fix_identifier(string id, bool isv, compiler_state & s);
string fix_identifier(string identifier, bool isVariable);
string fix_identifier(string identifier);
