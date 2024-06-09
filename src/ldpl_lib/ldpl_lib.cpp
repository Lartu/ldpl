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
#include <array>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <random>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#define NVM_FLOAT_EPSILON 0.00000001
#define CRLF "\r\n"

#define ldpl_number double
#define ldpl_vector ldpl_map
#define string string

using namespace std;

// Gets length of utf8-encoded c++ string
size_t get_str_utf8length(const string cstr)
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

/* bool loadFile(const string &fileName)
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
}*/

bool isAlphanumeric(const string &str)
{
    for (const char &c : str)
        if (!isalnum(c))
            return false;
    return true;
}

bool isAlphanumeric(const string &str, size_t from)
{
    for (size_t i = from; i < str.length(); ++i)
    {
        if (!isalnum(str[i]))
            return false;
    }
    return true;
}

bool isNumber(const string &number)
{
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
        return true;
    }
    else
    {
        return false;
    }
}

double getNumber(const string &number)
{
    return stod(number);
}

string substr(const string &s, size_t from, size_t count)
{
    if (from > s.length())
        return "";
    return s.substr(from, count);
}

/*string &string::erase(size_t from, size_t count)
{
    buffer.erase(buffer.begin() + from, buffer.begin() + from + count);
    return *this;
}*/

string substr(const string &s, size_t from)
{
    if (from > s.length())
        return "";
    return s.substr();
}

// NOTE: returns 0 on equality, -1 if the string is shorter, and 1 in any other
// case.
/* int compare(const string &s, size_t from, size_t count, const string &other)
{
    // TODO: Wtf is the point of this
    // Fix count to respect the actual end of the buffer.
    count = from + count > s.length() ? s.length() - from : count;
    // Compare sizes before anything else for efficiency.
    if (count < other.length())
        return -1;
    if (count > other.length())
        return 1;
    for (size_t i = from, j = 0; j < count; ++i, ++j)
        if (s[i] != other[j])
            return 1; // We already know it's not shorter, see above.
    return 0;
}*/

/*int string::compare(const string &other)
{
    if (*this == other)
        return 0;
    if (this->size() < other.size())
        return -1;
    else
        return 1;
}*/

// ---------------------------------------------------------------------------------------------------

// Global variables
ifstream file_loading_stream;
ofstream file_writing_stream;
string file_loading_line;
string joinvar; // Generic temporary use text variable (used by join but can be
                // used by any other statement as well)
ldpl_number VAR_ERRORCODE = 0;
string VAR_ERRORTEXT = "";

// Forward declarations
string to_ldpl_string(ldpl_number x);
string trimCopy(const string &line);

#ifndef LDPLMAP
#define LDPLMAP
template <typename T>
struct ldpl_map
{
    unordered_map<string, T> inner_collection;
    T &operator[](string i);
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
T &ldpl_map<T>::operator[](string i)
{
    return inner_collection[i];
}

template <typename T>
T &ldpl_map<T>::operator[](ldpl_number i)
{
    return inner_collection[to_ldpl_string(i)];
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
void get_indices(ldpl_list<string> &dest, ldpl_vector<T> &source)
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

ldpl_number to_number(const string &textNumber)
{
    try
    {
        // This is used to disallow the use of hexadecimal and binary literals.
        for (char i : textNumber)
            if ((i < '0' || i > '9') && i != '-' && i != '.')
                return 0;
        ldpl_number num = stod(textNumber);
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

int str_cmp(const string &a, const string &b)
{
    if (a == b)
        return 0;
    if (a < b)
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

void join(const string &a, const string &b, string &c)
{
    c = a + b;
}

// https://stackoverflow.com/a/27658515
string str_replace(const string &s, const string &find, const string &replace)
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

ldpl_number get_char_num(const string &chr)
{
    int length = chr.length();
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
    ldpl_number ord = (unsigned char)chr[0];
    return ord;
}

string charat(const string &s, ldpl_number pos)
{
    int graphemeCount = 0;
    size_t i = 0;

    while (i < s.size())
    {
        unsigned char c = s[i];
        size_t charLen = 0;

        if (c < 0x80)
        { // 1-byte character
            charLen = 1;
        }
        else if ((c & 0xE0) == 0xC0)
        { // 2-byte character
            charLen = 2;
        }
        else if ((c & 0xF0) == 0xE0)
        { // 3-byte character
            charLen = 3;
        }
        else if ((c & 0xF8) == 0xF0)
        { // 4-byte character
            charLen = 4;
        }

        bool isCombiningCharacter = false;
        if (charLen > 1)
        {
            int cp = 0;
            if (charLen == 2)
            {
                cp = ((c & 0x1F) << 6) | (s[i + 1] & 0x3F);
            }
            else if (charLen == 3)
            {
                cp = ((c & 0x0F) << 12) | ((s[i + 1] & 0x3F) << 6) | (s[i + 2] & 0x3F);
            }
            else if (charLen == 4)
            {
                cp = ((c & 0x07) << 18) | ((s[i + 1] & 0x3F) << 12) | ((s[i + 2] & 0x3F) << 6) | (s[i + 3] & 0x3F);
            }
            if (cp >= 0x0300 && cp <= 0x036F)
            {
                isCombiningCharacter = true;
            }
        }

        if (!isCombiningCharacter)
        {
            if (graphemeCount == pos)
            {
                return s.substr(i, charLen);
            }
            graphemeCount++;
        }

        i += charLen;
    }

    return ""; // Return an empty string if the position is out of range
}

// Convert ldpl_number to LDPL string, killing trailing 0's
// https://stackoverflow.com/questions/16605967/ &
// https://stackoverflow.com/questions/13686482/
string to_ldpl_string(ldpl_number x)
{
    ostringstream out;
    out.precision(10);
    out << fixed << x;
    string str = out.str();
    str.erase(str.find_last_not_of('0') + 1, string::npos);
    str.erase(str.find_last_not_of('.') + 1, string::npos);
    return str;
}

string exec(const char *cmd)
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

ldpl_number get_random()
{
    random_device rd;
    mt19937 mt(rd());
    uniform_real_distribution<double> dist(0.0, 1.0);
    ldpl_number r = (ldpl_number)dist(mt);
    return r;
}

string expandHomeDirectory(const string &filename)
{
#if defined(_WIN32)
    return filename;
#else
    string homeDir = exec("echo $HOME");
    homeDir = trimCopy(homeDir);
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

void load_file(const string &filename, string &destination)
{
    // Default to fail values.
    int fd = -1;
    void *fmap = MAP_FAILED;
    size_t flen = 0;
    struct stat buf;

    destination = "";
    VAR_ERRORTEXT = "The file '" + filename + "' couldn't be opened.";
    VAR_ERRORCODE = 1;

    fd = open(expandHomeDirectory(filename).c_str(), O_RDONLY);
    if (fd == -1)
        goto bail;
    if (fstat(fd, &buf) == -1)
        goto bail;
    flen = buf.st_size;
    fmap = mmap(NULL, flen, PROT_READ, MAP_SHARED, fd, 0);
    if (fmap == MAP_FAILED)
        goto bail;
    destination = string((const char *)fmap, flen);
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
void save_to_file(const string &filename, const string &content, ios_base::openmode mode)
{
    file_writing_stream.open(expandHomeDirectory(filename), mode);
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

void append_to_file(const string &filename, const string &content)
{
    save_to_file(filename, content, ios_base::app);
}
void write_file(const string &filename, const string &content)
{
    save_to_file(filename, content, ios_base::out);
}

ldpl_number utf8GetIndexOf(const string &haystack, const string &needle)
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

ldpl_number utf8Count(const string &haystack, const string &needle)
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
string toUpperCopy(string s)
{
    std::transform(s.begin(), s.end(), s.begin(), ::toupper);
    return s;
}

// Converts string to lowercase
string toLowerCopy(string s)
{
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

// Removes all trailing and ending whitespace from a string
string trimCopy(const string &line)
{
    // If the string is empty
    if (line.length() == 0)
        return line;

    // If the string has only one character
    if (line.length() == 1 && !isspace(line[0]))
        return line;

    // Left trim
    int first = 0;
    for (unsigned int i = 0; i < line.length(); ++i)
    {
        if (!isspace(line[i]))
        {
            first = i;
            break;
        }
    }

    // Right trim
    int last = 0;
    for (unsigned int i = line.length() - 1; i >= 0; --i)
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

    // Trim the string and return
    return line.substr(first, last - first);
}

ldpl_list<string> utf8_split_list(const string &haystack, const string &needle)
{
    ldpl_list<string> result;
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
                string token = haystack.substr(last_start, i - last_start);
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
        string token = haystack.substr(last_start, lenHaystack - last_start);
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

int countGraphemes(const string &str)
{
    int graphemeCount = 0;
    size_t i = 0;

    while (i < str.size())
    {
        unsigned char c = str[i];
        size_t charLen = 0;

        if (c < 0x80)
        { // 1-byte character
            charLen = 1;
        }
        else if ((c & 0xE0) == 0xC0)
        { // 2-byte character
            charLen = 2;
        }
        else if ((c & 0xF0) == 0xE0)
        { // 3-byte character
            charLen = 3;
        }
        else if ((c & 0xF8) == 0xF0)
        { // 4-byte character
            charLen = 4;
        }

        // Check for combining characters (this is a simplified check)
        bool isCombiningCharacter = false;
        if (charLen > 1)
        {
            int cp = 0;
            if (charLen == 2)
            {
                cp = ((c & 0x1F) << 6) | (str[i + 1] & 0x3F);
            }
            else if (charLen == 3)
            {
                cp = ((c & 0x0F) << 12) | ((str[i + 1] & 0x3F) << 6) | (str[i + 2] & 0x3F);
            }
            else if (charLen == 4)
            {
                cp = ((c & 0x07) << 18) | ((str[i + 1] & 0x3F) << 12) | ((str[i + 2] & 0x3F) << 6) | (str[i + 3] & 0x3F);
            }
            if (cp >= 0x0300 && cp <= 0x036F)
            {
                isCombiningCharacter = true;
            }
        }

        if (!isCombiningCharacter)
        {
            graphemeCount++;
        }

        i += charLen;
    }

    return graphemeCount;
}