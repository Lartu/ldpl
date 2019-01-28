#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "cpptrim.h"
#include <stack>

using namespace std;

struct compiler_state{
    uint section_state = 0; //0 no section, 1 data, 2 procedure
    uint current_line = 0;
    //Code to output (normally ASM or NVM)
    vector<string> output_code;
    //variables
    vector<pair<string, uint>> variables;
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
};

void error(const string & msg);
void compile(vector<string> & lines);
void tokenize(string & line, uint line_num, vector<string> & tokens);
void compile_line(vector<string> & tokens, uint line_num, compiler_state & state);
bool line_like(string model_line, vector<string> tokens, compiler_state & state); //Important to pass tokens by copy
bool is_number (string number);
bool is_string(string & token);
bool is_num_var(string & token, compiler_state & state);
bool is_txt_var(string & token, compiler_state & state);
bool is_variable(string & token, compiler_state & state);
bool variable_exists(string & token, compiler_state & state);
bool is_subprocedure(string & token, compiler_state & state);
