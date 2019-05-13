#include <sstream>
#include <math.h>
#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include <limits.h>
#include <unordered_map>
#include <stdlib.h>
#include <chrono>
#include <thread>
#include <time.h>

#define NVM_FLOAT_EPSILON 0.00000001
#define ldpl_number double
#define CRLF \"\\n\"

using namespace std;

//Global variables
ifstream file_loading_stream;
ofstream file_writing_stream;
string file_loading_line;
string joinvar;
ldpl_number VAR_ERRORCODE = 0;
string VAR_ERRORTEXT = \"\";

//Forward declarations
string to_ldpl_string(double x);

template<typename T>
struct ldpl_vector {
    unordered_map<string, T> inner_map;

    T& operator [] (const string& i) {
        return inner_map[i];
    }

    T& operator [] (ldpl_number i) {
        return inner_map[to_ldpl_string(i)];
    }
};

ldpl_number input_number(){
    string s = \"\";
    while(true){
        getline(cin, s);
        try {
            ldpl_number num = stod(s);
            return num;
        }
        catch (const std::invalid_argument& ia) {
            cout << \"Redo from start: \" << flush;
        }
    }
}

ldpl_number to_number(const string & a){
    try {
        //This is used to disallow the use of hexadecimal and binary literals.
        for(char i : a){
            if(i != '0' && i != '1' && i != '2' &&
               i != '3' && i != '4' && i != '5' &&
               i != '6' && i != '7' && i != '8' &&
               i != '9' && i != '-' && i != '.') return 0;
        }
        ldpl_number num = stod(a);
        return num;
    }
    catch (const std::invalid_argument& ia) {
        return 0;
    }
}


string input_string(){
    string s = \"\";
    getline(cin, s);
    return s;
}

string input_until_eof(){
    stringstream full;
    string s = \"\";
    bool firstLine = true;
    while (getline(cin, s)) {
        if(!firstLine) s = \"\\n\" + s;
        firstLine = false;
        full << s;
    }
    return full.str();
}

bool num_equal(ldpl_number a, ldpl_number b){
    return fabs(a - b) < NVM_FLOAT_EPSILON;
}

ldpl_number modulo(ldpl_number a, ldpl_number b){
    return (int) floor(a) % (int) floor(b);
}

void join(const string & a, const string & b, string & c){
    c = a + b;
}

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
    if (length==0) return \"\";
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
            else if (c>=248 && c<=255) return \"\";//invalid utf8
        }
        if (length !=INT_MAX && j+length-start<=0) return \"\";
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
        else if (c>=248 && c<=255) return \"\";//invalid utf8
    }
    if (j==start) { realstart=i; }
    if (j>=start && (length==INT_MAX || j<=start+length)) { reallength=i-realstart; }

    return str.substr(realstart,reallength);
}

ldpl_number str_len(const string & a){
    return utf8_strlen(a);
}

ldpl_number get_char_num(const string & chr){
    int length = chr.size();
    if (length != 1) {
        VAR_ERRORTEXT = \"Multibyte character received (probably UTF-8). Can't be parsed into a single number.\";
        VAR_ERRORCODE = 1;
        return 0;
    }
    VAR_ERRORTEXT = \"\";
    VAR_ERRORCODE = 0;
    ldpl_number ord = (unsigned char) chr[0];
    return ord;
}

unordered_map<size_t, ldpl_vector<string>> char_cache;
string charat(const string & s, ldpl_number pos) {
    unsigned int ipos = floor(pos);
    size_t hash = std::hash<string>{}(s);
    if(char_cache.find(hash)==char_cache.end()){
        ldpl_vector<string> v;
        ldpl_number x = 0;
        for(size_t i = 0; i < s.length();){
            int cplen = 1;
            if((s[i] & 0xf8) == 0xf0) cplen = 4;
            else if((s[i] & 0xf0) == 0xe0) cplen = 3;
            else if((s[i] & 0xe0) == 0xc0) cplen = 2;
            if((i + cplen) > s.length()) cplen = 1;
            v[x++] = s.substr(i, cplen);
            i += cplen;
        }
        char_cache[hash] = v;
    }
    return char_cache[hash][ipos];
}

//Convert ldpl_number to LDPL string, killing trailing 0's
//https://stackoverflow.com/questions/16605967/ & https://stackoverflow.com/questions/13686482/
string to_ldpl_string(ldpl_number x){
    ostringstream out;
    out.precision(10);
    out << fixed << x;
    string str = out.str();
    str.erase(str.find_last_not_of('0') + 1, string::npos);
    str.erase(str.find_last_not_of('.') + 1, string::npos);
    return str;
}

#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>

string exec(const char* cmd) {
    //TODO: Check if this works on windows
    array<char, 128> buffer;
    string result;
    unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, \"r\"), pclose);
    if (!pipe) {
        throw runtime_error(\"popen() failed!\");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

#include <random>

ldpl_number get_random(){
    ldpl_number r = ((ldpl_number) rand() / (RAND_MAX)) + 1;
    r = r - (int) r;
    return r;
}

void load_file(string filename, string & destination)
{
    //Load file
    ifstream file(filename);
    //Fail if the file couldn't be loaded
    if(!file.is_open()){
        destination = \"\";
        VAR_ERRORTEXT = \"Error: The file '\" + filename + \"' couldn't be opened.\";
        VAR_ERRORCODE = 1;
        return;
    }
    //TODO: Turn this into a control variable that can be checked from LDPL.
    //Get file contents
    string text = \"\";
    string line = \"\";
    while(getline(file, line))
    {
        text += line + \"\\n\";
    }
    VAR_ERRORTEXT = \"\";
    VAR_ERRORCODE = 0;
    destination = text;
    file.close();
}
