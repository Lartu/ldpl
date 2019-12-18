/*
    NOTE:
    THE CLASS LDPL_TEXT (CHTEXT) IS USED LIKE ANY NORMAL C++ STRING BUT IT
    IS NOT THE STD::STRING. IN ORDER TO GET AN STD::STRING FROM A CHTEXT
    YOU HAVE TO CALL THE .str_rep() FUNCTION. Other than that they are
    pretty similar.
*/

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
        size_t chText_get_str_utf8length(const string cstr);
        void createFromString(const string & cstr);
        void createFromChar(const char * cstr);
    public:
        size_t size();
        bool empty();
        size_t length();
        string str_rep();
        chText();
        chText (const string& x);
        chText (const double& f);
        chText& operator= (const string& x);
        chText (const char * x);
        chText& operator= (const char * x);
        chText (char x);
        chText& operator= (char x);
        chText operator [](size_t i) const;
        string & operator [](int i);
        friend ostream & operator << (ostream &out, const chText &c);
        friend chText operator+(const chText &c1, const chText &c2);
        friend chText operator+(const string &c1, const chText &c2);
        friend chText operator+(const chText &c1, const string &c2);
        friend chText operator+(const char * c1, const chText &c2);
        friend chText operator+(const chText &c1, const char * c2);
        friend bool operator<(const chText &c1, const chText &c2);
        friend bool operator>(const chText &c1, const chText &c2);
        friend bool operator==(const chText& ch1, const chText& ch2);
        friend bool operator==(const chText& ch1, const string& ch2);
        friend bool operator==(const chText& ch1, const char * ch2);
        friend bool operator==(const string& ch2, const chText& ch1);
        friend bool operator==(const char * ch2, const chText& ch1);
        friend bool operator==(const chText& ch1, const char ch2);
        friend bool operator==(const char ch2, const chText& ch1);
        friend bool operator!=(const chText& ch1, const chText& ch2);
        bool loadFile(const string &fileName);
        chText & operator += (const chText & txt);
        chText & operator += (const string & txt);
        chText & operator += (const char * txt);
        bool isAlphanumeric();
        bool isAlphanumeric(size_t from);
        bool isNumber();
        double getNumber();
        chText substr(size_t from, size_t count);
        chText & erase(size_t from, size_t count);
        chText substr(size_t from);
        int compare(size_t from, size_t count, chText other);
        int compare(chText other);
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
    chText to_ldpl_string(double x);
    #endif

    #ifndef LDPLMAP
    #define LDPLMAP
    template<typename T>
    struct ldpl_map {
        unordered_map<string, T> inner_collection;
        T& operator [] (chText i);
        T& operator [] (ldpl_number i);
    };
    #endif

    #ifndef LDPLLIST
    #define LDPLLIST
    template<typename T>
    struct ldpl_list {
        vector<T> inner_collection;
        T& operator [] (ldpl_number i);
    };
    #endif LDPLMAP
#endif
