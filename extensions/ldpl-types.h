#ifndef LDPLTYPES

#define LDPLTYPES
#define ldpl_number double
#define ldpl_vector ldpl_map
#define ldpl_text chText

#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string.h>
#include <unordered_map>
#include <math.h>

using namespace std;

#ifndef CHTEXT
#define CHTEXT
class chText {
    private:
    vector<string> buffer;
    string stringRep;

    // Gets length of utf8-encoded c++ string
    size_t chText_get_str_utf8length(const string cstr){
        size_t len = cstr.size();
        size_t utf8len = 0;
        unsigned int c;
        for (size_t i = 0; i < len; i++)
        {
            size_t l = 0;
            c = (unsigned char) cstr[i];
            if (c >= 0 && c <= 127) l = 0;
            else if ((c & 0xE0) == 0xC0) l = 1;
            else if ((c & 0xF0) == 0xE0) l = 2;
            else if ((c & 0xF8) == 0xF0) l = 3;
            else if (c>=248 && c<=255) return 0; //invalid utf8
            i += l;
            utf8len++;
        }
        return utf8len;
    }
    // Fills buffer with utf8-encoded c++ string
    void createFromString(const string & cstr){
        buffer.clear();
        size_t cstrlen = cstr.size();
        size_t chPos = 0;
        for(size_t i = 0; i < cstrlen; ++i){
            string ch = "";
            char c = cstr[i];
            if (c >= 0 && c <= 127){
                ch += c;
            }
            else if ((c & 0xE0) == 0xC0){
                ch += c;
                ch += cstr[++i];
            }
            else if ((c & 0xF0) == 0xE0){
                ch += c;
                ch += cstr[++i];
                ch += cstr[++i];
            }
            else if ((c & 0xF8) == 0xF0){
                ch += c;
                ch += cstr[++i];
                ch += cstr[++i];
                ch += cstr[++i];
            }
            buffer.push_back(ch);
            chPos++;
        }
    }
    // Fills buffer with utf8-encoded c++ string
    void createFromChar(const char * cstr){
        buffer.clear();
        size_t cstrlen = strlen(cstr);
        size_t chPos = 0;
        for(size_t i = 0; i < cstrlen; ++i){
            string ch = "";
            char c = cstr[i];
            if (c >= 0 && c <= 127){
                ch += c;
            }
            else if ((c & 0xE0) == 0xC0){
                ch += c;
                ch += cstr[++i];
            }
            else if ((c & 0xF0) == 0xE0){
                ch += c;
                ch += cstr[++i];
                ch += cstr[++i];
            }
            else if ((c & 0xF8) == 0xF0){
                ch += c;
                ch += cstr[++i];
                ch += cstr[++i];
                ch += cstr[++i];
            }
            buffer.push_back(ch);
            chPos++;
        }
    }

    public:
    size_t size(){
        return buffer.size();
    }
    bool empty(){
        return buffer.empty();
    }
    size_t length(){
        return size();
    }
    string str_rep(){
        stringRep = "";
        for(size_t i = 0; i < size(); ++i){
            stringRep += buffer[i];
        }
        return stringRep;
    }
    // default constructor
    chText(){}
    // conversion from string (constructor):
    chText (const string& x) {
        createFromString(x);
    }

    // conversion from double (constructor):
    chText (const double& f) {
        std::string str = to_string (f);
        str.erase(str.find_last_not_of('0') + 1, std::string::npos);
        str.erase ( str.find_last_not_of('.') + 1, std::string::npos );
        createFromString(str);
    }

    // conversion from string (assignment):
    chText& operator= (const string& x) {
        createFromString(x);
        return *this;
    }
    // conversion from char * (constructor):
    chText (const char * x) {
        createFromChar(x);
    }
    // conversion from char * (assignment):
    chText& operator= (const char * x) {
        createFromChar(x);
        return *this;
    }
    // conversion from char (constructor):
    chText (char x) {
        string a = "";
        a += x;
        createFromString(a);
    }
    // conversion from char (assignment):
    chText& operator= (char x) {
        string a = "";
        a += x;
        createFromString(a);
        return *this;
    }
    // [] for reading
    chText operator [](size_t i) const {
        if(i >= buffer.size()){
            cout << "Out-of-bounds index access." << endl;
            exit(1);
        }
        chText c = buffer[i];
        return c;
    }
    // [] for setting
    string & operator [](int i) {
        if(i >= buffer.size()){
            cout << "Out-of-bounds index access." << endl;
            exit(1);
        }
        return buffer[i];
    }
    // Output operator
    friend ostream & operator << (ostream &out, const chText &c);
    // + operator
    friend chText operator+(const chText &c1, const chText &c2);
    // + operator
    friend chText operator+(const string &c1, const chText &c2);
    // + operator
    friend chText operator+(const chText &c1, const string &c2);
    // + operator
    friend chText operator+(const char * c1, const chText &c2);
    // + operator
    friend chText operator+(const chText &c1, const char * c2);
    // < operator
    friend bool operator<(const chText &c1, const chText &c2);
    // > operator
    friend bool operator>(const chText &c1, const chText &c2);
    // Equality operator
    friend bool operator==(const chText& ch1, const chText& ch2);
    // Equality operator
    friend bool operator==(const chText& ch1, const string& ch2);
    // Equality operator
    friend bool operator==(const chText& ch1, const char * ch2);
    // Equality operator
    friend bool operator==(const string& ch2, const chText& ch1);
    // Equality operator
    friend bool operator==(const char * ch2, const chText& ch1);
    // Equality operator
    friend bool operator==(const chText& ch1, const char ch2);
    // Equality operator
    friend bool operator==(const char ch2, const chText& ch1);
    // Equality operator
    friend bool operator!=(const chText& ch1, const chText& ch2);
    // Load a file into this chText
    bool loadFile(const string &fileName){
        string line;
        string fileContents = "";
        ifstream myfile (fileName);
        if (myfile.is_open())
        {
            while ( getline (myfile,line) )
            {
                fileContents += line + "\\n";
            }
            myfile.close();
        }
        else return false; 
        createFromString(fileContents);
        return true;
    }
    // += operator
    chText & operator += (const chText & txt){
        for(const string & s : txt.buffer){
            buffer.push_back(s);
        }
        return *this;
    }
    // += operator
    chText & operator += (const string & txt){
        chText c2 = txt;
        for(const string & s : c2.buffer){
            buffer.push_back(s);
        }
        return *this;
    }
    // += operator
    chText & operator += (const char * txt){
        chText c2 = txt;
        for(const string & s : c2.buffer){
            buffer.push_back(s);
        }
        return *this;
    }

    bool isAlphanumeric(){
        for(const string & s : buffer){
            for(const char & c : s)
                if(!isalnum(c)) return false;
        }
        return true;
    }

    bool isAlphanumeric(size_t from){
        for(size_t i = from; i < size(); ++i){
            for(const char & c : buffer[i])
                if(!isalnum(c)) return false;
        }
        return true;
    }

    bool isNumber(){
        string number = "";
        for(size_t i = 0; i < size(); ++i){
            number += buffer[i];
        }
        unsigned int firstchar = 0;
        if(number[0] == '-') firstchar = 1;
        if(number[firstchar] == '.') return false; //.12 is not a valid decimal in LDPL, 0.12 is.
        if(number[firstchar] == '+') return false; //+5 is not a valid decimal in LDPL, 5 is.
        istringstream iss(number);
        double f;
        iss >> f;
        bool isNumber = iss.eof() && !iss.fail();
        //If it is a number, it might be an octal literal (e.g. 025, 067, 003 etc)
        //so we proceed to fix the original number to make it decimal.
        if(isNumber){
            string f_number = "";
            unsigned int i;
            for(i = 1; i < number.length(); ++i){
                //If prev char not 0
                if(number[i - 1] != '0'){
                    //if prev char is -, continue check
                    if(number[i - 1] == '-') f_number += '-';
                    //if prev char is number, break
                    else break;
                }
                //If prev number is 0
                else if(number[i] == '.') {
                    f_number += '0';
                }
            }
            f_number += number.substr(i - 1);
            number = f_number;
            return true;
        }else{
            return false;
        }
    }

    double getNumber(){
        string number = "";
        for(size_t i = 0; i < size(); ++i){
            number += buffer[i];
        }
        return stod(number);
    }

    chText substr(size_t from, size_t count){
        chText newText;
        for(size_t i = from; i < from + count; ++i){
            if(i >= buffer.size()) break;
            newText.buffer.push_back(buffer[i]);
        }
        return newText;
    }

    chText & erase(size_t from, size_t count){
        for(size_t i = 0; i < count; ++i)
            buffer.erase(buffer.begin() + from);
        return *this;
    }

    chText substr(size_t from){
        chText newText;
        for(size_t i = from; i < from + buffer.size(); ++i){
            if(i >= buffer.size()) break;
            newText.buffer.push_back(buffer[i]);
        }
        return newText;
    }

    int compare(size_t from, size_t count, chText other){
        chText newText;
        for(size_t i = from; i < from + count; ++i){
            if(i >= buffer.size()) break;
            newText.buffer.push_back(buffer[i]);
        }
        if (newText == other) return 0;
        if (newText.size() < other.size()) return -1;
        else return 1;
    }

    int compare(chText other){
        if (*this == other) return 0;
        if (this->size() < other.size()) return -1;
        else return 1;
    }
};

ostream & operator << (ostream &out, const chText &c);
chText operator+(const chText &c1, const chText &c2);
chText operator+(const string &c1, const chText &c2);
chText operator+(const chText &c1, const string &str);
chText operator+(const char * c1, const chText &c2);
chText operator+(const chText &c1, const char * str);
bool operator==(const chText& ch1, const chText& ch2);
bool operator==(const string& c1, const chText& ch2);
bool operator==(const chText& ch1, const string& c2);
bool operator==(const char * c1, const chText& ch2);
bool operator==(const chText& ch1, const char * c2);
bool operator==(const char c1, const chText& ch2);
bool operator==(const chText& ch1, const char c2);
bool operator<(const chText &c1, const chText &c2);
bool operator>(const chText &c1, const chText &c2);
bool operator!=(const chText& ch1, const chText& ch2);
#endif

chText to_ldpl_string(double x);

#ifndef LDPLMAP
#define LDPLMAP
template<typename T>
struct ldpl_map {
    unordered_map<string, T> inner_collection;

    T& operator [] (chText i) {
        return inner_collection[i.str_rep()];
    }

    T& operator [] (ldpl_number i) {
        return inner_collection[to_ldpl_string(i).str_rep()];
    }
};
#endif

#ifndef LDPLLIST
#define LDPLLIST
template<typename T>
struct ldpl_list {
    vector<T> inner_collection;

    T& operator [] (ldpl_number i) {
        i = floor(i);
        if (i < 0 || i >= inner_collection.size()) {
            cerr << "Runtime Error: LIST index " << i << " out of range [0, "
                 << inner_collection.size() << ")" << endl;
            exit(1);
        }
        return inner_collection[i];
    }
};
#endif LDPLMAP

#endif
