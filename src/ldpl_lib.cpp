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
#include <vector>

#define NVM_FLOAT_EPSILON 0.00000001
#define ldpl_number long double
#define CRLF \"\\n\"
#define ldpl_vector ldpl_map

using namespace std;

// ----------------- MPREAL ------------------------------



// -------------------------------------------------------

#ifndef CHTEXT
#define CHTEXT
#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string.h>

using namespace std;

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
            string ch = \"\";
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
            string ch = \"\";
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
        stringRep = \"\";
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
        string a = \"\";
        a += x;
        createFromString(a);
    }
    // conversion from char (assignment):
    chText& operator= (char x) {
        string a = \"\";
        a += x;
        createFromString(a);
        return *this;
    }
    // [] for reading
    chText operator [](size_t i) const {
        if(i >= buffer.size()){
            cout << \"Out-of-bounds index access.\" << endl;
            exit(1);
        }
        chText c = buffer[i];
        return c;
    }
    // [] for setting
    string & operator [](int i) {
        if(i >= buffer.size()){
            cout << \"Out-of-bounds index access.\" << endl;
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
        string fileContents = \"\";
        ifstream myfile (fileName);
        if (myfile.is_open())
        {
            while ( getline (myfile,line) )
            {
                fileContents += line + \"\\n\";
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
        string number = \"\";
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
            string f_number = \"\";
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
        string number = \"\";
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

ostream & operator << (ostream &out, const chText &c){
    for(const string & s : c.buffer){
        out << s;
    }
    return out;
}

chText operator+(const chText &c1, const chText &c2){
    chText res = c1;
	for(const string & s : c2.buffer){
        res.buffer.push_back(s);
    }
    return res;
}

chText operator+(const string &c1, const chText &c2){
    chText res = c1;
	for(const string & s : c2.buffer){
        res.buffer.push_back(s);
    }
    return res;
}

chText operator+(const chText &c1, const string &str){
    chText res = c1;
    chText c2 = str;
	for(const string & s : c2.buffer){
        res.buffer.push_back(s);
    }
    return res;
}

chText operator+(const char * c1, const chText &c2){
    chText res = c1;
	for(const string & s : c2.buffer){
        res.buffer.push_back(s);
    }
    return res;
}

chText operator+(const chText &c1, const char * str){
    chText res = c1;
    chText c2 = str;
	for(const string & s : c2.buffer){
        res.buffer.push_back(s);
    }
    return res;
}

bool operator==(const chText& ch1, const chText& ch2){
    return ch1.buffer == ch2.buffer;
}

bool operator==(const string& c1, const chText& ch2){
    const chText ch1 = c1;
    return ch1 == ch2;
}

bool operator==(const chText& ch1, const string& c2){
    const chText ch2 = c2;
    return ch1 == ch2;
}

bool operator==(const char * c1, const chText& ch2){
    const chText ch1 = c1;
    return ch1 == ch2;
}

bool operator==(const chText& ch1, const char * c2){
    const chText ch2 = c2;
    return ch1 == ch2;
}

bool operator==(const char c1, const chText& ch2){
    const chText ch1 = c1;
    return ch1 == ch2;
}

bool operator==(const chText& ch1, const char c2){
    const chText ch2 = c2;
    return ch1 == ch2;
}

bool operator<(const chText &c1, const chText &c2){
	size_t max = c1.buffer.size() > c2.buffer.size() ? c2.buffer.size() : c1.buffer.size();
    for(size_t i = 0; i < max; ++i){
        if(c1.buffer[i] < c2.buffer[i]) return true;
        else if (c1.buffer[i] > c2.buffer[i]) return false;
    }
    return false;
}

bool operator>(const chText &c1, const chText &c2){
	size_t max = c1.buffer.size() > c2.buffer.size() ? c2.buffer.size() : c1.buffer.size();
    for(size_t i = 0; i < max; ++i){
        if(c1.buffer[i] > c2.buffer[i]) return true;
        else if (c1.buffer[i] < c2.buffer[i]) return false;
    }
    return false;
}

bool operator!=(const chText& ch1, const chText& ch2){
    return ch1.buffer != ch2.buffer;
}

#endif

// ---------------------------------------------------------------------------------------------------

//Global variables
ifstream file_loading_stream;
ofstream file_writing_stream;
string file_loading_line;
chText joinvar; //Generic temporary use text variable (used by join but can be used by any other statement as well)
ldpl_number VAR_ERRORCODE = 0;
chText VAR_ERRORTEXT = \"\";

//Forward declarations
chText to_ldpl_string(double x);
chText trimCopy(chText _line);

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
            cerr << \"Runtime Error: LIST index \" << i << \" out of range [0, \"
                 << inner_collection.size() << \")\" << endl;
            exit(1);
        }
        return inner_collection[i];
    }
};
#endif LDPLMAP


template<typename T>
void get_indices(ldpl_list<chText> & dest, ldpl_vector<T> & source){
    dest.inner_collection.clear();
    int i = 0;
    for (const auto &keyPair : source.inner_collection) {
        dest.inner_collection.push_back(keyPair.first);
        ++i;
    }
}

ldpl_number input_number(){
    string s = \"\";
    while(true){
        getline(cin, s);
        try {
            ldpl_number num = stod(s);
            return num;
        }
        catch (const invalid_argument& ia) {
            cout << \"Redo from start: \" << flush;
        }
    }
}

ldpl_number to_number(chText & textNumber){
    string a = textNumber.str_rep();
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
    catch (const invalid_argument& ia) {
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

int str_cmp(chText a, chText b) {
  return a.compare(b);
}

ldpl_number modulo(ldpl_number a, ldpl_number b){
    return (int) floor(a) % (int) floor(b);
}

void join(const chText & a, const chText & b, chText & c){
    c = a + b;
}

//https://stackoverflow.com/a/27658515
chText str_replace(string s, string find, string replace){
    string result;
    size_t find_len = find.size();
    size_t pos, from=0;
    while (string::npos != (pos=s.find(find,from))){
        result.append(s, from, pos-from);
        result.append(replace);
        from = pos + find_len;
    }
    result.append(s, from , string::npos);
    return result;
}

ldpl_number get_char_num(chText chr){
    int length = chr.str_rep().size();
    if (length != 1) {
        VAR_ERRORTEXT = \"Multibyte character received (probably UTF-8). Can't be parsed into a single number.\";
        VAR_ERRORCODE = 1;
        return -1;
    }
    VAR_ERRORTEXT = \"\";
    VAR_ERRORCODE = 0;
    ldpl_number ord = (unsigned char) chr.str_rep()[0];
    return ord;
}

chText charat(const chText & s, ldpl_number pos){
    unsigned int _pos = floor(pos);
    return s[pos];
}

//Convert ldpl_number to LDPL string, killing trailing 0's
//https://stackoverflow.com/questions/16605967/ & https://stackoverflow.com/questions/13686482/
chText to_ldpl_string(ldpl_number x){
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

chText exec(const char* cmd) {
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
    random_device rd;
    mt19937 mt(rd());
    uniform_real_distribution<double> dist(0.0, 1.0);
    ldpl_number r = (ldpl_number) dist(mt);
    return r;
}

string expandHomeDirectory(string filename){
    #if defined(_WIN32)
        return filename;	
    #else
        string homeDir = exec(\"echo $HOME\").str_rep();
        homeDir = trimCopy(homeDir).str_rep();
        string newPath = \"\";
        for(size_t i = 0; i < filename.length(); ++i){
            if(filename[i] == '~'){
                newPath += homeDir;
            }else{
                newPath += filename[i];
            }
        }
        //cout << \"newPath:\" << newPath << endl;
        return newPath;
    #endif
}

//getLineSafe by https://gist.github.com/josephwb/df09e3a71679461fc104
std::istream& getlineSafe(std::istream& is, std::string& t) {
    t.clear();
    std::istream::sentry se(is, true);
    std::streambuf* sb = is.rdbuf();

    for (;;) {
        int c = sb->sbumpc();
		switch (c) {
			case '\\n':
			    t += (char)c;
				return is;
			case '\\r':
			    t += (char)c;
				if (sb->sgetc() == '\\n') {
					t += sb->sbumpc();
				}
				return is;
			case EOF:
				if (t.empty()) {
					is.setstate(std::ios::eofbit);
				}
				return is;
			default:
				t += (char)c;
		}
    }
}

void load_file(chText filename, chText & destination)
{
    //Load file
    ifstream file(expandHomeDirectory(filename.str_rep()));
    //Fail if the file couldn't be loaded
    if(!file.is_open()){
        destination = \"\";
        VAR_ERRORTEXT = \"The file '\" + filename + \"' couldn't be opened.\";
        VAR_ERRORCODE = 1;
        return;
    }
    //Get file contents
    destination = \"\";
    string line = \"\";
    while(getlineSafe(file, line))
    {
        destination += line;
    }
    VAR_ERRORTEXT = \"\";
    VAR_ERRORCODE = 0;
    file.close();
}

ldpl_number utf8GetIndexOf(chText haystack, chText needle){
    int lenHaystack = haystack.size();
    int lenNeedle = needle.size();
    if(lenHaystack < lenNeedle) return -1;
    int i = 0;
    while(i + lenNeedle <= lenHaystack){
        if(haystack.substr(i, lenNeedle) == needle) return i;
        ++i;
    }
    return -1;
}

ldpl_number utf8Count(chText haystack, chText needle){
    int lenHaystack = haystack.size();
    int lenNeedle = needle.size();
    if(lenHaystack < lenNeedle) return 0;
    int i = 0;
    int count = 0;
    while(i + lenNeedle <= lenHaystack){
        if(haystack.substr(i, lenNeedle) == needle) ++count;
        ++i;
    }
    return count;
}

//Removes all trailing and ending whitespace from a string
chText trimCopy(chText _line){
    string line = _line.str_rep();
    //If the string is empty
    if(line.size() == 0) return line;

    //If the string has only one character
    if(line.size() == 1 && !isspace(line[0])) return line;

    //Left trim
    int first = 0;
    for(unsigned int i = 0; i < line.size(); ++i){
        if (!isspace(line[i])){
            first = i;
            break;
        }
    }

    //Right trim
    int last = 0;
    for(unsigned int i = line.size()-1; i >= 0; --i){
        if (!isspace(line[i])){
            last = i+1;
            break;
        }
        //--i will break with unsigned int when reaching 0, so we check
        //if i == 0 and, if it is and it's not a space, we break
        if(i == 0){
            last = 0;
            break;
        }
    }

    //Trim the string
    line = line.substr(first, last-first);

    return line;
}

ldpl_list<chText> utf8_split_list(chText haystack, chText needle){
    ldpl_list<chText> result;
    int lenHaystack = haystack.size();
    int lenNeedle = needle.size();
    if (lenNeedle > 0) {
        int i = 0;
        int last_start = 0;
        while (i + lenNeedle <= lenHaystack) {
            if (haystack.substr(i, lenNeedle) == needle) {
                result.inner_collection.push_back(haystack.substr(last_start, i - last_start));
                i += lenNeedle;
                last_start = i;
            } else {
                ++i;
            }
        }
        // Grab everything after the last needle
        result.inner_collection.push_back(haystack.substr(last_start, lenHaystack - last_start));
    } else {
        // Split into individual characters
        for (int i = 0; i < lenHaystack; i++)
            result.inner_collection.push_back(charat(haystack, i));
    }
    return result;
}
