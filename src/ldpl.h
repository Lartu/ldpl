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

//TODO: Change vectors to maps
struct compiler_state{
    unsigned int section_state = 0; //0 no section, 1 data, 2 procedure
    unsigned int current_line = 0;
    bool in_subroutine = false;
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
    string open_subprocedure = "";
    void add_var_code(string code){
        this->variable_code.push_back(code);
    }
    void add_code(string code){
        if(open_subprocedure == "")
            this->output_code.push_back(code);
        else
            this->subroutine_code.push_back(code);
    }
    int open_ifs = 0;
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
void capitalize_tokens(vector<string> & tokens);
void load_and_compile(string & filename, compiler_state & state);
void replace_whitespace(string & code);
string fix_external_identifier(string identifier, bool isVariable);
string fix_identifier(string id, bool isv, compiler_state & s);
string fix_identifier(string identifier, bool isVariable);
string fix_identifier(string identifier);
