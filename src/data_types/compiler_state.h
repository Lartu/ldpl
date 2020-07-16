//TODO: Change vectors to maps
struct compiler_state{
    unsigned int section_state = 0;
    //0 no section, 1 data or local, 2 procedure, 3 sub-procedure start, 4 parameters
    //Code to output (plain C code)
    code_location where = { "", 0 };
    vector<string> variable_code;
    vector<string> output_code;
    vector<string> subroutine_code; //code outside main()
    bool declaring_parallel = false;
    //variables
    map<string, map<string, vector<unsigned int>>> variables; //map<subprocedure (or "" for main), map<variable name, vector<types>>> (variables are stored here)
    map<string, bool> externals; //variables defined in c++ extensions
    //1 number, 2 text, 3 list, 4 map --> <2, 3, 4, 4> means, for example, map of map of list of text
    map<string, vector<string>> subprocedures; // subprocedure -> list of parameter identifiers
    map<string, bool> parallels; // subprocedure -> is parallel
    void add_var_code(string code){
        this->variable_code.push_back(code);
    }
    void add_code(string code) {
        auto& output = current_subprocedure == "" ? this->output_code : this->subroutine_code;
        output.push_back(code);
    }
    void add_code(string code, code_location where) {
        auto& output = current_subprocedure == "" ? this->output_code : this->subroutine_code;
        // TODO add a debug flag for this
        //if (line_num)
        //    output.push_back("#line " + to_string(where.current_line) + " \"" + escape_c_quotes(where.current_file) + "\"");
        output.push_back(code);
    }
    bool open_quote = false;
    string current_subprocedure = "";
    int open_loops = 0;
    stack<int> block_stack; //0 sub-procedure, 1 if or else if, 2 while/for, 3 else
    void open_subprocedure(string & subprocedure){
        current_subprocedure = subprocedure;
        block_stack.push(0);
    }
    void open_parallel(string & subprocedure){
        declaring_parallel = true;
        open_subprocedure(subprocedure);
    }
    void close_subprocedure(){
        declaring_parallel = false;
        current_subprocedure = "";
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
    void open_else(){
        block_stack.top() = 3;
    }
    bool closing_else(){
        return !block_stack.empty() && block_stack.top() == 3;
    }
    void open_loop(){
        ++open_loops;
        block_stack.push(2);
    }
    void close_loop(){
        --open_loops;
        block_stack.pop();
    }
    bool closing_loop(){
        return !block_stack.empty() && block_stack.top() == 2;
    }
    //We keep track of declared variables used in range-based loops
    int range_vars = 0;
    string new_range_var(){
        return "RVAR_" + to_string(range_vars++);
    }
    //We keep track of declared variables used to pass literal parameters
    int literal_paramter_vars = 0;
    string new_literal_parameter_var(){
        return "LPVAR_" + to_string(literal_paramter_vars++);
    }
    //Adds a subprocedure that has been called but hasn't been declared.
    //If it hasn't been declared when compilation reaches the end of the source,
    //an error is risen.
    vector<pair<string, vector<vector<unsigned int>>>> expected_subprocedures; // subprocedure -> list of parameter types
    void add_expected_subprocedure(string name, string fixed_name, vector<vector<unsigned int>> & types){
        for(auto & subprocedure : expected_subprocedures) if (subprocedure.first == name) return;
        expected_subprocedures.emplace_back(name, types);
        string code = "void " + fixed_name + "(";
        for (size_t i = 0; i < types.size(); ++i) {
            code += get_c_type(types[i]) + "&";
            if (i < types.size() - 1) code += ", ";
        }
        code += ");";
        add_var_code(code);
    }
    void remove_expected_subprocedure(string & name){
        for (auto it = expected_subprocedures.begin(); it != expected_subprocedures.end(); ++it) {
            if (it->first == name) {
                expected_subprocedures.erase(it);
                return;
            }
        }
    }
    bool correct_subprocedure_types(string & name, vector<vector<unsigned int>> & types){
        for(auto & subprocedure : expected_subprocedures) if (subprocedure.first == name)
            return types == subprocedure.second;
        for(auto & subprocedure : subprocedures) if(subprocedure.first == name) {
            vector<vector<unsigned  int>> actual_types;
            for (auto & parameter : subprocedure.second) actual_types.push_back(variables[name][parameter]);
            return types == actual_types;
        }
        return true;
    }
    stack<string> working_dir;
    vector<pair<string, string>> custom_statements; // (statement model line, subprocedure name)

    string get_c_type(vector<unsigned int> & type) {
        string c_type = "";
        for (auto number_type: type) {
            if (number_type == 1) {
                c_type = "ldpl_number";
            } else if (number_type == 2) {
                c_type = "chText";
            } else if (number_type == 3) {
                c_type = "ldpl_list<" + c_type + ">";
            } else if (number_type == 4) {
                c_type = "ldpl_vector<" + c_type + ">";
            }
        }
        return c_type;
    }
};