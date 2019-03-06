//TODO arreglar que esto revienta si el programa tiene más de int lineas
//(no debería pasar eso ya, but who knows, revisar).
//TODO keystrokes (toco y actúa, espera hasta que no toco)

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <stack>
#include <climits>
#include <limits>
#include <cmath>
#include <map>
#include <cstdlib>
#include <random>
#include "cpptrim.h"

//Para exec incluyo esto también
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>

#define NVM_FLOAT_EPSILON 0.00000001

using namespace std;

bool debug = false;

class alfanum{
private:
    /*long*/ double n_value = 0;
    string t_value = "";
    bool p_is_number = true;
public:
    alfanum(/*long*/ double num){
        p_is_number = true;
        n_value = num;
    }
    alfanum(string text){
        p_is_number = false;
        t_value = text;
    }
    bool is_number(){
        return p_is_number;
    }
    /*long*/ double num_value(){
        return n_value;
    }
    string txt_value(){
        return t_value;
    }
    void print(){
        if(this->is_number()){
            cout << this -> num_value();
        }else{
            cout << this -> txt_value();
        }
    }
};

alfanum v_true(1);
alfanum v_false(0);

stack <alfanum> vm_stack;
map <string, unsigned long int> tag_map;
map <string, alfanum> aux_map;

//http://www.zedwood.com/article/cpp-utf8-strlen-function
int utf8_strlen(const string& str)
{
    int c,i,ix,q;
    for (q=0, i=0, ix=str.length(); i < ix; i++, q++)
    {
        c = (unsigned char) str[i];
        if      (c>=0   && c<=127) i+=0;
        else if ((c & 0xE0) == 0xC0) i+=1;
        else if ((c & 0xF0) == 0xE0) i+=2;
        else if ((c & 0xF8) == 0xF0) i+=3;
        else return 0;
    }
    return q;
}

//http://www.zedwood.com/article/cpp-utf-8-mb_substr-function
string utf8_substr(const string &str,int start, int length=INT_MAX)
{
    int i,ix,j,realstart,reallength;
    if (length==0) return "";
    if (start<0 || length <0)
    {
        //find j=utf8_strlen(str);
        for(j=0,i=0,ix=str.length(); i<ix; i+=1, j++)
        {
            unsigned char c= str[i];
            if      (c>=0   && c<=127) i+=0;
            else if (c>=192 && c<=223) i+=1;
            else if (c>=224 && c<=239) i+=2;
            else if (c>=240 && c<=247) i+=3;
            else if (c>=248 && c<=255) return "";//invalid utf8
        }
        if (length !=INT_MAX && j+length-start<=0) return "";
        if (start  < 0 ) start+=j;
        if (length < 0 ) length=j+length-start;       
    }
 
    j=0,realstart=0,reallength=0;
    for(i=0,ix=str.length(); i<ix; i+=1, j++)
    {
        if (j==start) { realstart=i; }
        if (j>=start && (length==INT_MAX || j<=start+length)) { reallength=i-realstart; }
        unsigned char c= str[i];
        if      (c>=0   && c<=127) i+=0;
        else if (c>=192 && c<=223) i+=1;
        else if (c>=224 && c<=239) i+=2;
        else if (c>=240 && c<=247) i+=3;
        else if (c>=248 && c<=255) return "";//invalid utf8
    }
    if (j==start) { realstart=i; }
    if (j>=start && (length==INT_MAX || j<=start+length)) { reallength=i-realstart; }
 
    return str.substr(realstart,reallength);
}

// Stylize: 
// Función que toma las lineas, les agrega su número de linea y las trimea.
// Also elimina lineas vacías.
vector<string> stylize(vector<string> & lines)
{
    vector<string> new_lines;
    for(unsigned long int i = 0; i < lines.size(); ++i){
        //Trim each line
        trim(lines[i]);
        //Remove empty lines
        if(lines[i].size() == 0) continue;
        //Remove comments
        if(lines[i][0] == ';') continue;
        //Add tags to the tag map
        if(lines[i][0] == '@'){
            if(lines[i].size() > 1)
                tag_map.insert(make_pair(lines[i].substr(1), new_lines.size()));
            else{
                cerr << "\033[1;31mError: Found tag without name.";
                cerr << "\033[0m" << endl;
                exit(1);
            }
        }
        //Add line
        new_lines.push_back(lines[i]);
    }
    return new_lines;
}

void panic(const char* message) {
	fprintf(stderr, "\033[1;31mNariVM Panic:\033[0m ");
	fprintf(stderr, "%s\n", message);
	exit(-1);
}

std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

bool is_number (string number);

/*long*/ double get_number(string number){
    /*long*/ double num;
    try {
        num = stold(number);
    }
    catch (const invalid_argument& ia) {
        num = 0;
    }
    return num;
}

void check_stack_size(unsigned int size){
    if(vm_stack.size() < size){
        cerr << "\033[1;31mError: Expected stack elements: \033[1;37m";
        cerr << size;
        cerr << "\033[1;31m, available: \033[1;37m";
        cerr << vm_stack.size();
        cerr << "\033[0m" << endl;
        exit(1);
    }
}

// Execute
// Executa las lineas xdxd
void execute(vector<string> & lines)
{
    for(unsigned long int i = 0; i < lines.size(); ++i){
		if(debug) cout << i+1 << " ) " << lines[i] << endl;
        string token = lines[i];
        // - Push number -
        if(is_number(token)){
            alfanum value(get_number(token));
            vm_stack.push(value);
        }
        // - Push string -
        else if(token.size() >= 2 && token[0] == '"' && token[token.size() - 1] == '"'){
            alfanum value(token.substr(1, token.size()-2));
            vm_stack.push(value);
        }
        // - Tag (ignored) -
        else if(token.size() > 1 && token[0] == '@'){
            continue;
        }
        // - Print top of stack -
        else if(token == "PRINT"){
            check_stack_size(1);
            vm_stack.top().print();
            cout << flush;
            vm_stack.pop();
        }
        // - Print top of stack -
        else if(token == "PRINTLN"){
            check_stack_size(1);
            vm_stack.top().print();
            cout << endl;
            vm_stack.pop();
        }
        // - Add top of stack -
        else if(token == "+"){
            check_stack_size(2);
            alfanum a = vm_stack.top();
            vm_stack.pop();
            alfanum b = vm_stack.top();
            vm_stack.pop();
            alfanum c(b.num_value() + a.num_value());
            vm_stack.push(c);
        }
        // - Sub top of stack -
        else if(token == "-"){
            check_stack_size(2);
            alfanum a = vm_stack.top();
            vm_stack.pop();
            alfanum b = vm_stack.top();
            vm_stack.pop();
            alfanum c(b.num_value() - a.num_value());
            vm_stack.push(c);
        }
        // - Mul top of stack -
        else if(token == "*"){
            check_stack_size(2);
            alfanum a = vm_stack.top();
            vm_stack.pop();
            alfanum b = vm_stack.top();
            vm_stack.pop();
            alfanum c(b.num_value() * a.num_value());
            vm_stack.push(c);
        }
        // - Div top of stack -
        else if(token == "/"){
            check_stack_size(2);
            alfanum a = vm_stack.top();
            vm_stack.pop();
            alfanum b = vm_stack.top();
            vm_stack.pop();
            alfanum c(b.num_value() / a.num_value());
            vm_stack.push(c);
        }
        // - Mod top of stack -
        else if(token == "%"){
            check_stack_size(2);
            alfanum a = vm_stack.top();
            vm_stack.pop();
            alfanum b = vm_stack.top();
            vm_stack.pop();
            alfanum c((unsigned long long) floor(b.num_value()) % (unsigned long long) floor(a.num_value()));
            vm_stack.push(c);
        }
        // - Join top of stack -
        else if(token == "JOIN"){
            check_stack_size(2);
            alfanum a = vm_stack.top();
            vm_stack.pop();
            alfanum b = vm_stack.top();
            vm_stack.pop();
            alfanum c(b.txt_value() + a.txt_value());
            vm_stack.push(c);
        }
        // - Discard top of stack -
        else if(token == "POP"){
            check_stack_size(1);
            vm_stack.pop();
        }
        // - Duplicate top of stack -
        else if(token == "COPY"){
            check_stack_size(1);
            vm_stack.push(vm_stack.top());
        }
        // - Swap top of stack with second element -
        else if(token == "SWAP"){
            check_stack_size(2);
            alfanum a = vm_stack.top();
            vm_stack.pop();
            alfanum b = vm_stack.top();
            vm_stack.pop();
            vm_stack.push(a);
            vm_stack.push(b);
        }
        // - Rotate the three topmost elements -
        else if(token == "ROTATE"){
            check_stack_size(3);
            alfanum a = vm_stack.top();
            vm_stack.pop();
            alfanum b = vm_stack.top();
            vm_stack.pop();
            alfanum c = vm_stack.top();
            vm_stack.pop();
            vm_stack.push(b);
            vm_stack.push(a);
            vm_stack.push(c);
        }
        // - Compare topmost two elements -
        else if(token == "=="){
            check_stack_size(2);
            alfanum a = vm_stack.top();
            vm_stack.pop();
            alfanum b = vm_stack.top();
            vm_stack.pop();
            if(a.is_number() != b.is_number()){
                vm_stack.push(v_false);
                continue;
            }
            if(a.is_number() && b.is_number()){
                if(fabs(a.num_value() - b.num_value()) < NVM_FLOAT_EPSILON)
                    vm_stack.push(v_true);
                else
                    vm_stack.push(v_false);
                continue;
            }else{
                if(a.txt_value() == b.txt_value())
                    vm_stack.push(v_true);
                else
                    vm_stack.push(v_false);
                continue;
            }
        }
        // - Compare topmost two elements -
        else if(token == "<"){
            check_stack_size(2);
            alfanum a = vm_stack.top();
            vm_stack.pop();
            alfanum b = vm_stack.top();
            vm_stack.pop();
            if(b.num_value() < a.num_value())
                    vm_stack.push(v_true);
                else
                    vm_stack.push(v_false);
                continue;
        }
        // - Compare topmost two elements -
        else if(token == "<="){
            check_stack_size(2);
            alfanum a = vm_stack.top();
            vm_stack.pop();
            alfanum b = vm_stack.top();
            vm_stack.pop();
            if(b.num_value() < a.num_value() || fabs(a.num_value() - b.num_value()) < NVM_FLOAT_EPSILON)
                    vm_stack.push(v_true);
                else
                    vm_stack.push(v_false);
                continue;
        }
        // - Compare topmost two elements -
        else if(token == ">"){
            check_stack_size(2);
            alfanum a = vm_stack.top();
            vm_stack.pop();
            alfanum b = vm_stack.top();
            vm_stack.pop();
            if(b.num_value() > a.num_value())
                    vm_stack.push(v_true);
                else
                    vm_stack.push(v_false);
                continue;
        }
        // - Compare topmost two elements -
        else if(token == ">="){
            check_stack_size(2);
            alfanum a = vm_stack.top();
            vm_stack.pop();
            alfanum b = vm_stack.top();
            vm_stack.pop();
            if(b.num_value() > a.num_value() || fabs(a.num_value() - b.num_value()) < NVM_FLOAT_EPSILON)
                    vm_stack.push(v_true);
                else
                    vm_stack.push(v_false);
                continue;
        }
        // - Negate top of stack -
        else if(token == "NOT"){
            check_stack_size(1);
            alfanum a = vm_stack.top();
            vm_stack.pop();
            if(a.num_value() == 0)
                vm_stack.push(v_true);
            else
                vm_stack.push(v_false);
        }
        // - Length of top of stack string -
        else if(token == "LENGTH"){
            check_stack_size(1);
            alfanum a = vm_stack.top();
            vm_stack.pop();
            alfanum c(utf8_strlen(a.txt_value()));
            vm_stack.push(c);
        }
        // - Char at position from top of stack string -
        else if(token == "CHARAT"){
            check_stack_size(2);
            alfanum a = vm_stack.top(); // String
            vm_stack.pop();
            alfanum b = vm_stack.top(); // Index
            vm_stack.pop();
            //TODO suboptimal (among all this suboptimal crap of a vm)
            string text = utf8_substr(a.txt_value(), b.num_value(), 1);
            alfanum c(text);
            vm_stack.push(c);
        }
        // - Conditional Jump -
        else if(token.size() > 3 && token.substr(0, 3) == "IF:"){
            check_stack_size(1);
            alfanum a = vm_stack.top();
            vm_stack.pop();
            if(a.num_value() == 0) continue;
            string tag_name = token.substr(3);
            unsigned long int tag_line = 0;
            try {
                tag_line = tag_map.at(tag_name);
            }
            catch (const out_of_range & ia) {
                cerr << "\033[1;31mError: Undeclared tag: \033[1;37m";
                cerr << tag_name;
                cerr << "\033[0m" << endl;
                exit(1);
            }
            i = tag_line - 1;
        }
        // - Jump -
        else if(token.size() > 4 && token.substr(0, 4) == "JMP:"){
            string tag_name = token.substr(4);
            unsigned long int tag_line = 0;
            try {
                tag_line = tag_map.at(tag_name);
            }
            catch (const out_of_range & ia) {
                cerr << "\033[1;31mError: Undeclared tag: \033[1;37m";
                cerr << tag_name;
                cerr << "\033[0m" << endl;
                exit(1);
            }
            i = tag_line - 1;
        }
        // - Store Auxiliar Value -
        else if(token.size() > 6 && token.substr(0, 6) == "TOAUX:"){
            check_stack_size(1);
            alfanum a = vm_stack.top(); // Value to store
            vm_stack.pop();
            string aux_name = token.substr(6);
            //Not optimal
            if(aux_map.count(aux_name) == 0){
                aux_map.insert(make_pair(aux_name, a));
            }else{
                aux_map.at(aux_name) = a;
            }
        }
        // - Get Auxiliar Value defaulting to 0 if the variable doesn't exist -
        else if(token.size() > 4 && token.substr(0, 4) == "AUX:"){
            string aux_name = token.substr(4);
            //Not optimal
            if(aux_map.count(aux_name) == 0){
                alfanum c(0);
                vm_stack.push(c);
            }else{
                vm_stack.push(aux_map.at(aux_name));
            }
        }
        // - Get Auxiliar Value defaulting to "" if the variable doesn't exist -
        else if(token.size() > 6 && token.substr(0, 6) == "AUX-S:"){
            string aux_name = token.substr(6);
            //Not optimal
            if(aux_map.count(aux_name) == 0){
                alfanum c("");
                vm_stack.push(c);
            }else{
                vm_stack.push(aux_map.at(aux_name));
            }
        }
        // - Print Stack for Debugging Purposes -
        else if(token == "PRINT-STACK"){
            stack <alfanum> a = vm_stack;
            cout << endl << "****** STACK ******" << endl;
            while (!a.empty()) {
                cout << " -> ";
                a.top().print();
                cout << " ";
                cout << (a.top().is_number() ? "(num)" : "(txt)");
                cout << endl;
                a.pop();
            }
            cout << "*******************" << endl << endl;
        }
        // - Debug mode on / off -
        else if(token == "DEBUG"){
            debug = !debug;
        }
        // - Input text to stack -
        else if(token == "INPUT"){
			string s = "";
			getline(cin, s);
            alfanum in(s);
            vm_stack.push(in);
        }
        // - Read standard input -
        else if(token == "INPUT-FULL"){
            stringstream full;
            string s;
            bool firstLine = true;
            while (getline(cin, s)) {
                if(!firstLine) s = "\n" + s;
                firstLine = false;
                full << s;
            }
            alfanum in(full.str());
            vm_stack.push(in);
        }
        // - Input number to stack -
        else if(token == "INPUT-NUM"){
            while(true){
                string s = "";
                getline(cin, s);
                if(is_number(s))
                {
                    alfanum in(get_number(s));
                    vm_stack.push(in);
                    break;
                }
                else
                {
                    cout << "Redo from start: " << flush;
                }
            }
        }
        // - Halt and exit -
        else if(token == "HALT"){
			exit(0);
        }
        // - Run system command and push output -
        else if(token == "SYS-EXEC-OUT"){
			check_stack_size(1);
            alfanum a = vm_stack.top();
            vm_stack.pop();
            alfanum c(exec(a.txt_value().c_str()));
            vm_stack.push(c);
        }
        // - Run system command and push exit value -
        else if(token == "SYS-EXEC"){
			check_stack_size(1);
            alfanum a = vm_stack.top();
            vm_stack.pop();
            alfanum c(system(a.txt_value().c_str()));
            vm_stack.push(c);
        }
        // - Get number value of string -
        else if(token == "TO-NUM"){
			check_stack_size(1);
            alfanum a = vm_stack.top();
            if(!a.is_number()){
                vm_stack.pop();
                alfanum c(get_number(a.txt_value()));
                vm_stack.push(c);
            }
        }
        // - Get string representation of number -
        else if(token == "TO-STR"){
			check_stack_size(1);
            alfanum a = vm_stack.top();
            if(a.is_number()){
                vm_stack.pop();
                string num = to_string(a.num_value());
                num.erase (num.find_last_not_of('0') + 1, std::string::npos);
                if(num[num.size() - 1] == '.')
                    num = num.substr(0, num.size()-1);
                alfanum c(num);
                vm_stack.push(c);
            }
        }
        // - Check if top of stack is a number string WITHOUT popping it -
        else if(token == "IS-NUM"){
			check_stack_size(1);
            alfanum a = vm_stack.top();
            alfanum c(is_number(a.txt_value()) ? 1 : 0);
            vm_stack.push(c);
        }
        // - Get aboslute value of number -
        else if(token == "ABS"){
			check_stack_size(1);
            alfanum a = vm_stack.top();
            vm_stack.pop();
            alfanum c(abs(a.num_value()));
            vm_stack.push(c);
        }
        // - Round number -
        else if(token == "FLOOR"){
			check_stack_size(1);
            alfanum a = vm_stack.top();
            vm_stack.pop();
            alfanum c(floor(a.num_value()));
            vm_stack.push(c);
        }
        // - Round number -
        else if(token == "CEIL"){
			check_stack_size(1);
            alfanum a = vm_stack.top();
            vm_stack.pop();
            alfanum c(ceil(a.num_value()));
            vm_stack.push(c);
        }
        // - Pop jump-
        else if(token == "JMP-POP"){
			check_stack_size(1);
            string tag_name = vm_stack.top().txt_value();
            unsigned long int tag_line = 0;
            try {
                tag_line = tag_map.at(tag_name);
            }
            catch (const out_of_range & ia) {
                cerr << "\033[1;31mError: Undeclared tag: \033[1;37m";
                cerr << tag_name;
                cerr << "\033[0m" << endl;
                exit(1);
            }
            i = tag_line - 1;
        }
        // - Conditional Pop Jump -
        else if(token == "IF-POP"){
            check_stack_size(2);
            alfanum a = vm_stack.top();
            vm_stack.pop();
            if(a.num_value() == 0) continue;
            string tag_name = vm_stack.top().txt_value();
            unsigned long int tag_line = 0;
            try {
                tag_line = tag_map.at(tag_name);
            }
            catch (const out_of_range & ia) {
                cerr << "\033[1;31mError: Undeclared tag: \033[1;37m";
                cerr << tag_name;
                cerr << "\033[0m" << endl;
                exit(1);
            }
            i = tag_line - 1;
        }
        // - Push stack height -
        else if(token == "STACK-SIZE"){
            vm_stack.push(vm_stack.size());
        }
        // - Push IP -
        else if(token == "IP"){
            alfanum value(i+1);
            vm_stack.push(value);
        }
        // - IP jump-
        else if(token == "JMP-IP-POP"){
			check_stack_size(1);
            unsigned long int tag_line = vm_stack.top().num_value();
            vm_stack.pop();
            i = tag_line - 1;
        }
        // - Conditional IP Jump -
        else if(token == "IF-IP-POP"){
            check_stack_size(2);
            alfanum a = vm_stack.top();
            vm_stack.pop();
            if(a.num_value() == 0) continue;
            unsigned long int tag_line = vm_stack.top().num_value();
            i = tag_line - 1;
        }
		// - Store Auxiliar Value POP -
        else if(token == "TOAUX-POP"){
            check_stack_size(2);
            alfanum a = vm_stack.top(); // Value to store
            vm_stack.pop();
            string aux_name = vm_stack.top().txt_value();
            vm_stack.pop();
            //Not optimal
            if(aux_map.count(aux_name) == 0){
                aux_map.insert(make_pair(aux_name, a));
            }else{
                aux_map.at(aux_name) = a;
            }
        }
        // - Get Auxiliar Value POP defaulting to 0 if the variable doesn't exist -
        else if(token == "AUX-POP"){
			check_stack_size(1);
            string aux_name = vm_stack.top().txt_value();
            vm_stack.pop();
            //Not optimal
            if(aux_map.count(aux_name) == 0){
                alfanum c(0);
                vm_stack.push(c);
            }else{
                vm_stack.push(aux_map.at(aux_name));
            }
        }
        // - Get Auxiliar Value POP defaulting to "" if the variable doesn't exist -
        else if(token == "AUX-S-POP"){
			check_stack_size(1);
            string aux_name = vm_stack.top().txt_value();
            vm_stack.pop();
            //Not optimal
            if(aux_map.count(aux_name) == 0){
                alfanum c("");
                vm_stack.push(c);
            }else{
                vm_stack.push(aux_map.at(aux_name));
            }
        }
        //Random number
        else if(token == "RANDOM"){
            /*long*/ double r = rand() * NVM_FLOAT_EPSILON;
            r = r - (int) r;
            alfanum c(r);
            vm_stack.push(c);
        }
        //Number to character
        else if(token == "CHR"){
                check_stack_size(1);
                alfanum a = vm_stack.top();
                vm_stack.pop();
            char s[2];
            s[0] = (char)a.num_value();
            s[1] = '\0';
                vm_stack.push(alfanum(string(&s[0])));
        }
        //Character to number
        else if(token == "ORD"){
                check_stack_size(1);
                alfanum a = vm_stack.top();
                vm_stack.pop();
                string chr = a.txt_value();
                if (chr.size() != 1) {
                    cerr << "\033[1;31mError: ORD expects a string of length 1, got: \033[1;37m";
                    cerr << chr << endl;
                    cerr << "\033[0m" << endl;
                    exit(1);
            }
            int ord = chr[0];
                vm_stack.push(ord);
        }
        // - Discard top of stack -
        else if(token == "BEEP"){
          system("beep");
        }
        // - Error -
        else{
            cerr << "\033[1;31mError: Unexpected command: \033[1;37m";
            cerr << token;
            cerr << "\033[0m" << endl;
            exit(1);
        }
    }
}

void nvm (vector<string> & lines){
    // - Seed RNG -
    srand(time(NULL));
    // - Set cout precision -
    cout.precision(numeric_limits</*long*/ double>::digits10);
    // - Stilize obtained lines -
    lines = stylize(lines);
    // - Execute lines -
    execute(lines);
}
