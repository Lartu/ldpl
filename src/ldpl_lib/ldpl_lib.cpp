#include <limits.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#define NVM_FLOAT_EPSILON 0.00000001
#define CRLF "\n"

#define ldpl_number double
#define ldpl_vector ldpl_map
#define ldpl_text chText

using namespace std;

// -------------------------------------------------------

#ifndef CHTEXT
#define CHTEXT
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
class chText
{
private:
    vector<string> buffer;
    string stringRep;
    size_t chText_get_str_utf8length(const string cstr);
    void createFromString(const string &cstr);
    void createFromChar(const char *cstr);
    void createFromMem(const char *mem, size_t len);

public:
    size_t size() const;
    bool empty() const;
    size_t length() const;
    string str_rep();
    chText();
    chText(const string &x);
    chText(const double &f);
    chText &operator=(const string &x);
    chText(const char *x);
    chText &operator=(const char *x);
    chText(const char *x, size_t len);
    chText(char x);
    chText &operator=(char x);
    chText operator[](size_t i) const;
    string &operator[](int i);
    friend ostream &operator<<(ostream &out, const chText &c);
    friend chText operator+(const chText &c1, const chText &c2);
    friend chText operator+(const string &c1, const chText &c2);
    friend chText operator+(const chText &c1, const string &c2);
    friend chText operator+(const char *c1, const chText &c2);
    friend chText operator+(const chText &c1, const char *c2);
    friend bool operator<(const chText &c1, const chText &c2);
    friend bool operator>(const chText &c1, const chText &c2);
    friend bool operator==(const chText &ch1, const chText &ch2);
    friend bool operator==(const chText &ch1, const string &ch2);
    friend bool operator==(const chText &ch1, const char *ch2);
    friend bool operator==(const string &ch2, const chText &ch1);
    friend bool operator==(const char *ch2, const chText &ch1);
    friend bool operator==(const chText &ch1, const char ch2);
    friend bool operator==(const char ch2, const chText &ch1);
    friend bool operator!=(const chText &ch1, const chText &ch2);
    bool loadFile(const string &fileName);
    chText &operator+=(const chText &txt);
    chText &operator+=(const string &txt);
    chText &operator+=(const char *txt);
    bool isAlphanumeric();
    bool isAlphanumeric(size_t from);
    bool isNumber();
    double getNumber();
    chText substr(size_t from, size_t count);
    chText &erase(size_t from, size_t count);
    chText substr(size_t from);
    int compare(size_t from, size_t count, const chText &other);
    int compare(const chText &other);
};

ostream &operator<<(ostream &out, const chText &c);
chText operator+(const chText &c1, const chText &c2);
chText operator+(const string &c1, const chText &c2);
chText operator+(const chText &c1, const string &str);
chText operator+(const char *c1, const chText &c2);
chText operator+(const chText &c1, const char *str);
bool operator==(const chText &ch1, const chText &ch2);
bool operator==(const string &c1, const chText &ch2);
bool operator==(const chText &ch1, const string &c2);
bool operator==(const char *c1, const chText &ch2);
bool operator==(const chText &ch1, const char *c2);
bool operator==(const char c1, const chText &ch2);
bool operator==(const chText &ch1, const char c2);
bool operator<(const chText &c1, const chText &c2);
bool operator>(const chText &c1, const chText &c2);
bool operator!=(const chText &ch1, const chText &ch2);
chText to_ldpl_string(double x);
#endif

// Gets length of utf8-encoded c++ string
size_t chText::chText_get_str_utf8length(const string cstr)
{
    size_t len = cstr.size();
    size_t utf8len = 0;
    unsigned int c;
    for (size_t i = 0; i < len; i++)
    {
        size_t l = 0;
        c = (unsigned char)cstr[i];
        if (c >= 0 && c <= 127)
            l = 0;
        else if ((c & 0xE0) == 0xC0)
            l = 1;
        else if ((c & 0xF0) == 0xE0)
            l = 2;
        else if ((c & 0xF8) == 0xF0)
            l = 3;
        else if (c >= 248 && c <= 255)
            return 0; // invalid utf8
        i += l;
        utf8len++;
    }
    return utf8len;
}
// Fills buffer with utf8-encoded c++ string
void chText::createFromString(const string &cstr)
{
    createFromMem(cstr.c_str(), cstr.size());
}
// Fills buffer with utf8-encoded c++ string
void chText::createFromChar(const char *cstr)
{
    // If we copy the implementation we can do without the `strlen` call.
    createFromMem(cstr, strlen(cstr));
}
// Fills buffer with utf8-encoded c++ string
void chText::createFromMem(const char *cstr, size_t cstrlen)
{
    buffer.clear();
    size_t chPos = 0;
    for (size_t i = 0; i < cstrlen; ++i)
    {
        string ch = "";
        char c = cstr[i];
        if (c >= 0 && c <= 127)
        {
            ch += c;
        }
        else if ((c & 0xE0) == 0xC0)
        {
            ch += c;
            ch += cstr[++i];
        }
        else if ((c & 0xF0) == 0xE0)
        {
            ch += c;
            ch += cstr[++i];
            ch += cstr[++i];
        }
        else if ((c & 0xF8) == 0xF0)
        {
            ch += c;
            ch += cstr[++i];
            ch += cstr[++i];
            ch += cstr[++i];
        }
        buffer.push_back(ch);
        chPos++;
    }
}
size_t chText::size() const { return buffer.size(); }
bool chText::empty() const { return buffer.empty(); }
size_t chText::length() const { return size(); }
string chText::str_rep()
{
    stringRep = "";
    for (size_t i = 0; i < size(); ++i)
    {
        stringRep += buffer[i];
    }
    return stringRep;
}
// default constructor
chText::chText() {}
// conversion from string (constructor):
chText::chText(const string &x) { createFromString(x); }

// conversion from double (constructor):
chText::chText(const double &f)
{
    std::string str = to_string(f);
    str.erase(str.find_last_not_of('0') + 1, std::string::npos);
    str.erase(str.find_last_not_of('.') + 1, std::string::npos);
    createFromString(str);
}

// conversion from string (assignment):
chText &chText::operator=(const string &x)
{
    createFromString(x);
    return *this;
}
// conversion from char * (constructor):
chText::chText(const char *x) { createFromChar(x); }
// conversion from char * (assignment):
chText &chText::operator=(const char *x)
{
    createFromChar(x);
    return *this;
}
// conversion from char * and size (constructor):
chText::chText(const char *x, size_t len) { createFromMem(x, len); }
// conversion from char (constructor):
chText::chText(char x) { createFromMem(&x, 1); }
// conversion from char (assignment):
chText &chText::operator=(char x)
{
    createFromMem(&x, 1);
    return *this;
}
// [] for reading
chText chText::operator[](size_t i) const
{
    if (i >= buffer.size())
    {
        cout << "Out-of-bounds index access." << endl;
        exit(1);
    }
    chText c = buffer[i];
    return c;
}
// [] for setting
string &chText::operator[](int i)
{
    if (i >= buffer.size())
    {
        cout << "Out-of-bounds index access." << endl;
        exit(1);
    }
    return buffer[i];
}
bool chText::loadFile(const string &fileName)
{
    int fd = open(fileName.c_str(), O_RDONLY);
    if (fd == -1)
        return false;
    struct stat buf;
    if (fstat(fd, &buf) == -1)
    {
        close(fd);
        return false;
    }
    size_t flen = buf.st_size;
    void *fmmap = mmap(NULL, flen, PROT_READ, MAP_SHARED, fd, 0);
    if (fmmap == MAP_FAILED)
    {
        close(fd);
        return false;
    }
    createFromMem((const char *)fmmap, flen);
    munmap(fmmap, flen);
    close(fd);
    return true;
}
// += operator
chText &chText::operator+=(const chText &txt)
{
    buffer.insert(buffer.end(), txt.buffer.begin(), txt.buffer.end());
    return *this;
}
// += operator
chText &chText::operator+=(const string &txt)
{
    chText c2 = txt;
    buffer.insert(buffer.end(), c2.buffer.begin(), c2.buffer.end());
    return *this;
}
// += operator
chText &chText::operator+=(const char *txt)
{
    chText c2 = txt;
    buffer.insert(buffer.end(), c2.buffer.begin(), c2.buffer.end());
    return *this;
}

bool chText::isAlphanumeric()
{
    for (const string &s : buffer)
    {
        for (const char &c : s)
            if (!isalnum(c))
                return false;
    }
    return true;
}

bool chText::isAlphanumeric(size_t from)
{
    for (size_t i = from; i < size(); ++i)
    {
        for (const char &c : buffer[i])
            if (!isalnum(c))
                return false;
    }
    return true;
}

bool chText::isNumber()
{
    string number = "";
    for (size_t i = 0; i < size(); ++i)
    {
        number += buffer[i];
    }
    unsigned int firstchar = 0;
    if (number[0] == '-')
        firstchar = 1;
    if (number[firstchar] == '.')
        return false; //.12 is not a valid decimal in LDPL, 0.12 is.
    if (number[firstchar] == '+')
        return false; //+5 is not a valid decimal in LDPL, 5 is.
    istringstream iss(number);
    double f;
    iss >> f;
    bool isNumber = iss.eof() && !iss.fail();
    // If it is a number, it might be an octal literal (e.g. 025, 067, 003 etc)
    // so we proceed to fix the original number to make it decimal.
    if (isNumber)
    {
        string f_number = "";
        unsigned int i;
        for (i = 1; i < number.length(); ++i)
        {
            // If prev char not 0
            if (number[i - 1] != '0')
            {
                // if prev char is -, continue check
                if (number[i - 1] == '-')
                    f_number += '-';
                // if prev char is number, break
                else
                    break;
            }
            // If prev number is 0
            else if (number[i] == '.')
            {
                f_number += '0';
            }
        }
        f_number += number.substr(i - 1);
        number = f_number;
        return true;
    }
    else
    {
        return false;
    }
}

double chText::getNumber()
{
    string number = "";
    for (size_t i = 0; i < size(); ++i)
    {
        number += buffer[i];
    }
    return stod(number);
}

chText chText::substr(size_t from, size_t count)
{
    count = from + count > buffer.size() ? buffer.size() - from : count;
    chText newText;
    newText.buffer.insert(newText.buffer.begin(), buffer.begin() + from,
                          buffer.begin() + from + count);
    return newText;
}

chText &chText::erase(size_t from, size_t count)
{
    buffer.erase(buffer.begin() + from, buffer.begin() + from + count);
    return *this;
}

chText chText::substr(size_t from)
{
    chText newText;
    newText.buffer.insert(newText.buffer.begin(), buffer.begin() + from,
                          buffer.end());
    return newText;
}

// NOTE: returns 0 on equality, -1 if the string is shorter, and 1 in any other
// case.
int chText::compare(size_t from, size_t count, const chText &other)
{
    // Fix count to respect the actual end of the buffer.
    count = from + count > buffer.size() ? buffer.size() - from : count;
    // Compare sizes before anything else for efficiency.
    if (count < other.buffer.size())
        return -1;
    if (count > other.buffer.size())
        return 1;
    for (size_t i = from, j = 0; j < count; ++i, ++j)
        if (buffer[i] != other.buffer[j])
            return 1; // We already know it's not shorter, see above.
    return 0;
}

int chText::compare(const chText &other)
{
    if (*this == other)
        return 0;
    if (this->size() < other.size())
        return -1;
    else
        return 1;
}

ostream &operator<<(ostream &out, const chText &c)
{
    for (const string &s : c.buffer)
    {
        out << s;
    }
    return out;
}

chText operator+(const chText &c1, const chText &c2)
{
    chText res = c1;
    res.buffer.insert(res.buffer.end(), c2.buffer.begin(), c2.buffer.end());
    return res;
}

chText operator+(const string &c1, const chText &c2)
{
    chText res = c1;
    res.buffer.insert(res.buffer.end(), c2.buffer.begin(), c2.buffer.end());
    return res;
}

chText operator+(const chText &c1, const string &str)
{
    chText res = c1;
    chText c2 = str;
    res.buffer.insert(res.buffer.end(), c2.buffer.begin(), c2.buffer.end());
    return res;
}

chText operator+(const char *c1, const chText &c2)
{
    chText res = c1;
    res.buffer.insert(res.buffer.end(), c2.buffer.begin(), c2.buffer.end());
    return res;
}

chText operator+(const chText &c1, const char *str)
{
    chText res = c1;
    chText c2 = str;
    res.buffer.insert(res.buffer.end(), c2.buffer.begin(), c2.buffer.end());
    return res;
}

bool operator==(const chText &ch1, const chText &ch2)
{
    return ch1.buffer == ch2.buffer;
}

bool operator==(const string &c1, const chText &ch2)
{
    const chText ch1 = c1;
    return ch1 == ch2;
}

bool operator==(const chText &ch1, const string &c2)
{
    const chText ch2 = c2;
    return ch1 == ch2;
}

bool operator==(const char *c1, const chText &ch2)
{
    const chText ch1 = c1;
    return ch1 == ch2;
}

bool operator==(const chText &ch1, const char *c2)
{
    const chText ch2 = c2;
    return ch1 == ch2;
}

bool operator==(const char c1, const chText &ch2)
{
    const chText ch1 = c1;
    return ch1 == ch2;
}

bool operator==(const chText &ch1, const char c2)
{
    const chText ch2 = c2;
    return ch1 == ch2;
}

bool operator<(const chText &c1, const chText &c2)
{
    size_t max =
        c1.buffer.size() > c2.buffer.size() ? c2.buffer.size() : c1.buffer.size();
    for (size_t i = 0; i < max; ++i)
    {
        if (c1.buffer[i] < c2.buffer[i])
            return true;
        else if (c1.buffer[i] > c2.buffer[i])
            return false;
    }
    return c1.buffer.size() < c2.buffer.size();
}

bool operator>(const chText &c1, const chText &c2)
{
    size_t max =
        c1.buffer.size() > c2.buffer.size() ? c2.buffer.size() : c1.buffer.size();
    for (size_t i = 0; i < max; ++i)
    {
        if (c1.buffer[i] > c2.buffer[i])
            return true;
        else if (c1.buffer[i] < c2.buffer[i])
            return false;
    }
    return c1.buffer.size() > c2.buffer.size();
}

bool operator!=(const chText &ch1, const chText &ch2)
{
    return ch1.buffer != ch2.buffer;
}

// ---------------------------------------------------------------------------------------------------

// Global variables
ifstream file_loading_stream;
ofstream file_writing_stream;
string file_loading_line;
chText joinvar; // Generic temporary use text variable (used by join but can be
                // used by any other statement as well)
ldpl_number VAR_ERRORCODE = 0;
chText VAR_ERRORTEXT = "";

// Forward declarations
chText to_ldpl_string(ldpl_number x);
chText trimCopy(chText _line);

#ifndef LDPLMAP
#define LDPLMAP
template <typename T>
struct ldpl_map
{
    unordered_map<string, T> inner_collection;
    T &operator[](chText i);
    T &operator[](ldpl_number i);
    bool operator==(const ldpl_map<T> &map2) const;
};
#endif

template <typename T>
bool ldpl_map<T>::operator==(const ldpl_map<T> &map2) const
{
    return this->inner_collection == map2.inner_collection;
}

template <typename T>
T &ldpl_map<T>::operator[](chText i)
{
    return inner_collection[i.str_rep()];
}

template <typename T>
T &ldpl_map<T>::operator[](ldpl_number i)
{
    return inner_collection[to_ldpl_string(i).str_rep()];
}

#ifndef LDPLLIST
#define LDPLLIST
template <typename T>
struct ldpl_list
{
    vector<T> inner_collection;
    T &operator[](ldpl_number i);
    bool operator==(const ldpl_list<T> &list2) const;
};
#endif LDPLMAP

template <typename T>
bool ldpl_list<T>::operator==(const ldpl_list<T> &list2) const
{
    return this->inner_collection == list2.inner_collection;
}

template <typename T>
T &ldpl_list<T>::operator[](ldpl_number i)
{
    i = floor(i);
    if (i < 0 || i >= inner_collection.size())
    {
        cerr << "Runtime Error: LIST index " << i << " out of range [0, "
             << inner_collection.size() << ")" << endl;
        exit(1);
    }
    return inner_collection[i];
}

template <typename T>
void get_indices(ldpl_list<chText> &dest, ldpl_vector<T> &source)
{
    dest.inner_collection.clear();
    int i = 0;
    for (const auto &keyPair : source.inner_collection)
    {
        dest.inner_collection.push_back(keyPair.first);
        ++i;
    }
}

ldpl_number input_number()
{
    string s = "";
    while (true)
    {
        getline(cin, s);
        try
        {
            ldpl_number num = stod(s);
            return num;
        }
        catch (const invalid_argument &ia)
        {
            cout << "Redo from start: " << flush;
        }
    }
}

ldpl_number to_number(chText textNumber)
{
    string a = textNumber.str_rep();
    try
    {
        // This is used to disallow the use of hexadecimal and binary literals.
        for (char i : a)
            if ((i < '0' || i > '9') && i != '-' && i != '.')
                return 0;
        ldpl_number num = stod(a);
        return num;
    }
    catch (const invalid_argument &ia)
    {
        return 0;
    }
}

string input_string()
{
    string s = "";
    getline(cin, s);
    return s;
}

string input_until_eof()
{
    stringstream full;
    string s = "";
    bool firstLine = true;
    while (getline(cin, s))
    {
        if (!firstLine)
            s = "\n" + s;
        firstLine = false;
        full << s;
    }
    return full.str();
}

bool num_equal(ldpl_number a, ldpl_number b)
{
    return fabs(a - b) < NVM_FLOAT_EPSILON;
}

int str_cmp(chText a, chText b)
{
    if (a == b)
        return 0;
    if (a.str_rep() < b.str_rep())
        return -1;
    else
        return 1;
}

ldpl_number modulo(ldpl_number a, ldpl_number b)
{
    long f_a = floor(a);
    long f_b = floor(b);
    return (f_a % f_b + f_b) % f_b;
}

void join(const chText &a, const chText &b, chText &c)
{
    c = a + b;
}

// https://stackoverflow.com/a/27658515
chText str_replace(string s, string find, string replace)
{
    string result;
    size_t find_len = find.size();
    size_t pos, from = 0;
    while (string::npos != (pos = s.find(find, from)))
    {
        result.append(s, from, pos - from);
        result.append(replace);
        from = pos + find_len;
    }
    result.append(s, from, string::npos);
    return result;
}

ldpl_number get_char_num(chText chr)
{
    int length = chr.str_rep().size();
    if (length != 1)
    {
        VAR_ERRORTEXT =
            "Multibyte character received (probably UTF-8). Can't be parsed into a "
            "single number.";
        VAR_ERRORCODE = 1;
        return -1;
    }
    VAR_ERRORTEXT = "";
    VAR_ERRORCODE = 0;
    ldpl_number ord = (unsigned char)chr.str_rep()[0];
    return ord;
}

chText charat(const chText &s, ldpl_number pos)
{
    unsigned int _pos = floor(pos);
    return s[pos];
}

// Convert ldpl_number to LDPL string, killing trailing 0's
// https://stackoverflow.com/questions/16605967/ &
// https://stackoverflow.com/questions/13686482/
chText to_ldpl_string(ldpl_number x)
{
    ostringstream out;
    out.precision(10);
    out << fixed << x;
    string str = out.str();
    str.erase(str.find_last_not_of('0') + 1, string::npos);
    str.erase(str.find_last_not_of('.') + 1, string::npos);
    return str;
}

#include <array>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>

chText exec(const char *cmd)
{
    array<char, 128> buffer;
    string result;
    unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe)
    {
        throw runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
    {
        result += buffer.data();
    }
    return result;
}

#include <random>

ldpl_number get_random()
{
    random_device rd;
    mt19937 mt(rd());
    uniform_real_distribution<double> dist(0.0, 1.0);
    ldpl_number r = (ldpl_number)dist(mt);
    return r;
}

string expandHomeDirectory(string filename)
{
#if defined(_WIN32)
    return filename;
#else
    string homeDir = exec("echo $HOME").str_rep();
    homeDir = trimCopy(homeDir).str_rep();
    string newPath = "";
    for (size_t i = 0; i < filename.length(); ++i)
    {
        if (filename[i] == '~')
        {
            newPath += homeDir;
        }
        else
        {
            newPath += filename[i];
        }
    }
    // cout << "newPath:" << newPath << endl;
    return newPath;
#endif
}

// getLineSafe by https://gist.github.com/josephwb/df09e3a71679461fc104
std::istream &getlineSafe(std::istream &is, std::string &t)
{
    t.clear();
    std::istream::sentry se(is, true);
    std::streambuf *sb = is.rdbuf();

    for (;;)
    {
        int c = sb->sbumpc();
        switch (c)
        {
        case '\n':
            t += (char)c;
            return is;
        case '\r':
            t += (char)c;
            if (sb->sgetc() == '\n')
            {
                t += sb->sbumpc();
            }
            return is;
        case EOF:
            if (t.empty())
            {
                is.setstate(std::ios::eofbit);
            }
            return is;
        default:
            t += (char)c;
        }
    }
}

void load_file(chText filename, chText &destination)
{
    // Default to fail values.
    int fd = -1;
    void *fmap = MAP_FAILED;
    size_t flen = 0;
    struct stat buf;

    destination = "";
    VAR_ERRORTEXT = "The file '" + filename + "' couldn't be opened.";
    VAR_ERRORCODE = 1;

    string fileName = filename.str_rep();
    fd = open(expandHomeDirectory(fileName).c_str(), O_RDONLY);
    if (fd == -1)
        goto bail;
    if (fstat(fd, &buf) == -1)
        goto bail;
    flen = buf.st_size;
    fmap = mmap(NULL, flen, PROT_READ, MAP_SHARED, fd, 0);
    if (fmap == MAP_FAILED)
        goto bail;
    destination = chText((const char *)fmap, flen);
    VAR_ERRORTEXT = "";
    VAR_ERRORCODE = 0;

bail:
    // If we mapped it, we unmap it.
    if (fmap != MAP_FAILED)
        munmap(fmap, flen);
    if (fd != -1)
        close(fd);
}

// Used by append_ and write_.
void save_to_file(chText filename, chText content, ios_base::openmode mode)
{
    file_writing_stream.open(expandHomeDirectory(filename.str_rep()), mode);
    if (!file_writing_stream.is_open())
    {
        VAR_ERRORTEXT = "Could not open " + filename;
        VAR_ERRORCODE = 1;
        return;
    }
    file_writing_stream << content;
    if (file_writing_stream.bad())
    {
        VAR_ERRORTEXT = "Could not write to " + filename;
        VAR_ERRORCODE = 2;
        return;
    }
    VAR_ERRORTEXT = "";
    VAR_ERRORCODE = 0;
    file_writing_stream.close();
}

void append_to_file(chText filename, chText content)
{
    save_to_file(filename, content, ios_base::app);
}
void write_file(chText filename, chText content)
{
    save_to_file(filename, content, ios_base::out);
}

ldpl_number utf8GetIndexOf(chText haystack, chText needle)
{
    int lenHaystack = haystack.size();
    int lenNeedle = needle.size();
    if (lenHaystack < lenNeedle)
        return -1;
    int i = 0;
    while (i + lenNeedle <= lenHaystack)
    {
        if (haystack.substr(i, lenNeedle) == needle)
            return i;
        ++i;
    }
    return -1;
}

ldpl_number utf8Count(chText haystack, chText needle)
{
    int lenHaystack = haystack.size();
    int lenNeedle = needle.size();
    if (lenHaystack < lenNeedle)
        return 0;
    int i = 0;
    int count = 0;
    while (i + lenNeedle <= lenHaystack)
    {
        if (haystack.substr(i, lenNeedle) == needle)
            ++count;
        ++i;
    }
    return count;
}

// Converts string to uppercase
chText toUpperCopy(chText str)
{
    string out = str.str_rep();
    std::transform(out.begin(), out.end(), out.begin(), ::toupper);
    return out;
}

// Converts string to lowercase
chText toLowerCopy(chText str)
{
    string out = str.str_rep();
    std::transform(out.begin(), out.end(), out.begin(), ::tolower);
    return out;
}

// Removes all trailing and ending whitespace from a string
chText trimCopy(chText _line)
{
    string line = _line.str_rep();
    // If the string is empty
    if (line.size() == 0)
        return line;

    // If the string has only one character
    if (line.size() == 1 && !isspace(line[0]))
        return line;

    // Left trim
    int first = 0;
    for (unsigned int i = 0; i < line.size(); ++i)
    {
        if (!isspace(line[i]))
        {
            first = i;
            break;
        }
    }

    // Right trim
    int last = 0;
    for (unsigned int i = line.size() - 1; i >= 0; --i)
    {
        if (!isspace(line[i]))
        {
            last = i + 1;
            break;
        }
        //--i will break with unsigned int when reaching 0, so we check
        // if i == 0 and, if it is and it's not a space, we break
        if (i == 0)
        {
            last = 0;
            break;
        }
    }

    // Trim the string
    line = line.substr(first, last - first);

    return line;
}

ldpl_list<chText> utf8_split_list(chText haystack, chText needle)
{
    ldpl_list<chText> result;
    int lenHaystack = haystack.size();
    int lenNeedle = needle.size();
    if (lenNeedle > 0)
    {
        int i = 0;
        int last_start = 0;
        while (i + lenNeedle <= lenHaystack)
        {
            if (haystack.substr(i, lenNeedle) == needle)
            {
                chText token = haystack.substr(last_start, i - last_start);
                if (token.length() > 0)
                {
                    result.inner_collection.push_back(token);
                }
                i += lenNeedle;
                last_start = i;
            }
            else
            {
                ++i;
            }
        }
        // Grab everything after the last needle
        chText token = haystack.substr(last_start, lenHaystack - last_start);
        if (token.length() > 0)
        {
            result.inner_collection.push_back(token);
        }
    }
    else
    {
        // Split into individual characters
        for (int i = 0; i < lenHaystack; i++)
            result.inner_collection.push_back(charat(haystack, i));
    }
    return result;
}
