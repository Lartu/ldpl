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
#include <thread>
#include <iomanip>

#define NVM_FLOAT_EPSILON 0.00000001
#define CRLF "\n"

#define ldpl_number LdplNumber
#define ldpl_vector ldpl_map
#define ldpl_text graphemedText

using namespace std;

// Internal use variables (in lowercase so they don't interfere with user-declared ones)
time_t ldpl_time;
std::chrono::steady_clock::time_point program_start_time;

// -------------------------------------------------------

#ifndef LDPL_NUMBER
#define LDPL_NUMBER
#include <iostream>
#include <cmath>
#include "BigInt.hpp"

class LdplNumber
{
private:
    bool isInteger = false;
    double floatingValue;
    BigInt integerValue;

public:
    // Constructors
    LdplNumber()
    {
        integerValue = 0;
        isInteger = true;
    }
    LdplNumber(double v)
    {
        floatingValue = v;
        isInteger = false;
    }
    LdplNumber(int v)
    {
        integerValue = v;
        isInteger = true;
    }
    LdplNumber(long v)
    {
        integerValue = v;
        isInteger = true;
    }
    LdplNumber(size_t v)
    {
        integerValue = v;
        isInteger = true;
    }
    LdplNumber(long long v)
    {
        integerValue = v;
        isInteger = true;
    }
    LdplNumber(BigInt v)
    {
        integerValue = v;
        isInteger = true;
    }

    // Copy constructor
    LdplNumber(const LdplNumber &other)
    {
        isInteger = other.isInteger;
        if (other.isInteger)
        {
            integerValue = other.integerValue;
        }
        else
        {
            floatingValue = other.floatingValue;
        }
    }

    // Assignment operator
    LdplNumber &operator=(const LdplNumber &other)
    {

        if (this != &other)
        {
            isInteger = other.isInteger;
            if (isInteger)
            {
                integerValue = other.integerValue;
            }
            else
            {
                floatingValue = other.floatingValue;
            }
        }
        return *this;
    }

    // Arithmetic operators
    LdplNumber operator+(const LdplNumber &other) const
    {
        if (isInteger && other.isInteger)
        {
            return LdplNumber(integerValue + other.integerValue);
        }
        else if (!isInteger && !other.isInteger)
        {
            return LdplNumber(floatingValue + other.floatingValue);
        }
        else
        {
            if (isInteger)
            {
                return LdplNumber(integerValue.to_long_long() + other.floatingValue);
            }
            else
            {
                return LdplNumber(floatingValue + other.integerValue.to_long_long());
            }
        }
    }

    LdplNumber operator-(const LdplNumber &other) const
    {
        if (isInteger && other.isInteger)
        {
            return LdplNumber(integerValue - other.integerValue);
        }
        else if (!isInteger && !other.isInteger)
        {
            return LdplNumber(floatingValue - other.floatingValue);
        }
        else
        {
            if (isInteger)
            {
                return LdplNumber(integerValue.to_long_long() - other.floatingValue);
            }
            else
            {
                return LdplNumber(floatingValue - other.integerValue.to_long_long());
            }
        }
    }

    LdplNumber operator*(const LdplNumber &other) const
    {
        if (isInteger && other.isInteger)
        {
            return LdplNumber(integerValue * other.integerValue);
        }
        else if (!isInteger && !other.isInteger)
        {
            return LdplNumber(floatingValue * other.floatingValue);
        }
        else
        {
            if (isInteger)
            {
                return LdplNumber(integerValue.to_long_long() * other.floatingValue);
            }
            else
            {
                return LdplNumber(floatingValue * other.integerValue.to_long_long());
            }
        }
    }

    LdplNumber operator/(const LdplNumber &other) const
    {
        if ((other.isInteger && other.integerValue == 0) || (!other.isInteger && other.floatingValue == 0))
        {
            throw std::runtime_error("LDPL Error: Division by zero");
        }

        if (isInteger && other.isInteger)
        {
            return LdplNumber((double)integerValue.to_long_long() / (double)other.integerValue.to_long_long());
        }
        else if (!isInteger && !other.isInteger)
        {
            return LdplNumber(floatingValue / other.floatingValue);
        }
        else
        {
            if (isInteger)
            {
                return LdplNumber(integerValue.to_long_long() / other.floatingValue);
            }
            else
            {
                return LdplNumber(floatingValue / other.integerValue.to_long_long());
            }
        }
    }

    LdplNumber operator%(const LdplNumber &other) const
    {
        if ((other.isInteger && other.integerValue == 0) || (!other.isInteger && other.floatingValue == 0))
        {
            throw std::runtime_error("LDPL Error: Modulo by zero");
        }

        if (isInteger && other.isInteger)
        {
            return LdplNumber(integerValue % other.integerValue);
        }
        else if (!isInteger && !other.isInteger)
        {
            return LdplNumber((long long)floor(floatingValue) % (long long)floor(other.floatingValue));
        }
        else
        {
            if (isInteger)
            {
                return LdplNumber(integerValue % (long long)floor(other.floatingValue));
            }
            else
            {
                return LdplNumber(static_cast<long long>(floatingValue) % other.integerValue.to_long_long());
            }
        }
    }

    // Unary operators
    LdplNumber operator+() const
    {
        return *this;
    }

    LdplNumber operator-() const
    {
        if (isInteger)
        {
            return LdplNumber(-integerValue);
        }
        else
        {
            return LdplNumber(-floatingValue);
        }
    }

    // Compound assignment operators
    LdplNumber &operator+=(const LdplNumber &other)
    {
        if (isInteger && other.isInteger)
        {
            integerValue += other.integerValue;
        }
        else if (!isInteger && !other.isInteger)
        {
            floatingValue += other.floatingValue;
        }
        else
        {
            if (isInteger)
            {
                isInteger = false;
                floatingValue = integerValue.to_long_long() + other.floatingValue;
            }
            else
            {
                floatingValue = floatingValue + other.integerValue.to_long_long();
            }
        }
        return *this;
    }

    LdplNumber &operator-=(const LdplNumber &other)
    {
        if (isInteger && other.isInteger)
        {
            integerValue -= other.integerValue;
        }
        else if (!isInteger && !other.isInteger)
        {
            floatingValue -= other.floatingValue;
        }
        else
        {
            if (isInteger)
            {
                isInteger = false;
                floatingValue = integerValue.to_long_long() - other.floatingValue;
            }
            else
            {
                floatingValue = floatingValue - other.integerValue.to_long_long();
            }
        }
        return *this;
    }

    LdplNumber &operator*=(const LdplNumber &other)
    {
        if (isInteger && other.isInteger)
        {
            integerValue *= other.integerValue;
        }
        else if (!isInteger && !other.isInteger)
        {
            floatingValue *= other.floatingValue;
        }
        else
        {
            if (isInteger)
            {
                isInteger = false;
                floatingValue = integerValue.to_long_long() * other.floatingValue;
            }
            else
            {
                floatingValue = floatingValue * other.integerValue.to_long_long();
            }
        }
        return *this;
    }

    LdplNumber &operator/=(const LdplNumber &other)
    {
        if ((other.isInteger && other.integerValue == 0) || (!other.isInteger && other.floatingValue == 0))
        {
            throw std::runtime_error("LDPL Error: Division by zero");
        }
        if (isInteger && other.isInteger)
        {
            isInteger = false;
            floatingValue = (double)integerValue.to_long_long() / (double)other.integerValue.to_long_long();
        }
        else if (!isInteger && !other.isInteger)
        {
            floatingValue /= other.floatingValue;
        }
        else
        {
            if (isInteger)
            {
                isInteger = false;
                floatingValue = integerValue.to_long_long() / other.floatingValue;
            }
            else
            {
                floatingValue = floatingValue / other.integerValue.to_long_long();
            }
        }
        return *this;
    }

    // Increment and decrement operators
    LdplNumber &operator++()
    { // Prefix increment
        if (isInteger)
        {
            ++integerValue;
        }
        else
        {
            ++floatingValue;
        }
        return *this;
    }

    LdplNumber operator++(int)
    { // Postfix increment
        LdplNumber temp = *this;
        if (isInteger)
        {
            ++integerValue;
        }
        else
        {
            ++floatingValue;
        }
        return temp;
    }

    LdplNumber &operator--()
    { // Prefix decrement
        if (isInteger)
        {
            --integerValue;
        }
        else
        {
            --floatingValue;
        }
        return *this;
    }

    LdplNumber operator--(int)
    { // Postfix decrement
        LdplNumber temp = *this;
        if (isInteger)
        {
            --integerValue;
        }
        else
        {
            --floatingValue;
        }
        return temp;
    }

    // Comparison operators
    bool operator==(const LdplNumber &other) const
    {
        if (isInteger && other.isInteger)
        {
            return integerValue == other.integerValue;
        }
        else if (!isInteger && !other.isInteger)
        {
            return floatingValue == other.floatingValue;
        }
        else
        {
            if (isInteger)
            {
                // For these to be equal, the other floating value must be equal to its integer value
                if (other.floatingValue - floor(other.floatingValue) < NVM_FLOAT_EPSILON)
                {
                    // and equal to the integer value of this number
                    BigInt otherIntegerValue = (long long)floor(other.floatingValue);
                    return integerValue == otherIntegerValue;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                // For these to be equal, this floating value must be equal to its integer value
                if (floatingValue - floor(floatingValue) < NVM_FLOAT_EPSILON)
                {
                    // and equal to the integer value of the other number
                    BigInt otherIntegerValue = (long long)floor(floatingValue);
                    return other.integerValue == otherIntegerValue;
                }
                else
                {
                    return false;
                }
            }
        }
    }

    bool operator!=(const LdplNumber &other) const
    {
        return !(*this == other);
    }

    bool operator<(const LdplNumber &other) const
    {
        if (isInteger && other.isInteger)
        {
            return integerValue < other.integerValue;
        }
        else if (!isInteger && !other.isInteger)
        {
            return floatingValue < other.floatingValue;
        }
        else
        {
            if (isInteger)
            {
                return integerValue.to_long_long() < other.floatingValue;
            }
            else
            {
                return floatingValue < other.integerValue.to_long_long();
            }
        }
    }

    bool operator>(const LdplNumber &other) const
    {
        return !(*this < other || *this == other);
    }

    bool operator<=(const LdplNumber &other) const
    {
        return *this < other || *this == other;
    }

    bool operator>=(const LdplNumber &other) const
    {
        return *this > other || *this == other;
    }

    friend bool operator<(int lhs, const LdplNumber &rhs)
    {
        return LdplNumber(lhs) < rhs;
    }

    friend bool operator>(int lhs, const LdplNumber &rhs)
    {
        return LdplNumber(lhs) > rhs;
    }

    friend bool operator<=(int lhs, const LdplNumber &rhs)
    {
        return LdplNumber(lhs) <= rhs;
    }

    friend bool operator>=(int lhs, const LdplNumber &rhs)
    {
        return LdplNumber(lhs) >= rhs;
    }

    friend bool operator<(double lhs, const LdplNumber &rhs)
    {
        return LdplNumber(lhs) < rhs;
    }

    friend bool operator>(double lhs, const LdplNumber &rhs)
    {
        return LdplNumber(lhs) > rhs;
    }

    friend bool operator<=(double lhs, const LdplNumber &rhs)
    {
        return LdplNumber(lhs) <= rhs;
    }

    friend bool operator>=(double lhs, const LdplNumber &rhs)
    {
        return LdplNumber(lhs) >= rhs;
    }

    // Stream insertion operator
    friend std::ostream &operator<<(std::ostream &os, const LdplNumber &num);

    // Stream extraction operator
    friend std::istream &operator>>(std::istream &is, LdplNumber &num)
    {
        string inputStream;
        is >> inputStream;
        if (inputStream.find('.') != std::string::npos)
        {
            num.isInteger = true;
            num.integerValue = inputStream;
        }
        else
        {
            num.isInteger = false;
            num.floatingValue = stod(inputStream);
        }
        return is;
    }

    int to_int() const
    {
        if (isInteger)
        {
            return integerValue.to_long_long();
        }
        else
        {
            return (int)floatingValue;
        }
    }

    int to_long_long() const
    {
        if (isInteger)
        {
            return integerValue.to_long_long();
        }
        else
        {
            return (long long)floatingValue;
        }
    }

    double to_double() const
    {
        if (isInteger)
        {
            return integerValue.to_long_long();
        }
        else
        {
            return floatingValue;
        }
    }

    size_t to_size_t() const
    {
        if (isInteger)
        {
            return integerValue.to_long_long();
        }
        else
        {
            return (int)floatingValue;
        }
    }

    bool IsInteger()
    {
        return isInteger;
    }

    BigInt internalIntegerValue()
    {
        return integerValue;
    }

    double internalFloatingValue()
    {
        return floatingValue;
    }

    friend LdplNumber operator*(int lhs, const LdplNumber &rhs) {
        return LdplNumber(lhs) * rhs;
    }

    friend LdplNumber operator+(int lhs, const LdplNumber &rhs) {
        return LdplNumber(lhs) + rhs;
    }

    friend LdplNumber operator-(int lhs, const LdplNumber &rhs) {
        return LdplNumber(lhs) - rhs;
    }

    friend LdplNumber operator/(int lhs, const LdplNumber &rhs) {
        return LdplNumber(lhs) / rhs;
    }

    friend LdplNumber operator%(int lhs, const LdplNumber &rhs) {
        return LdplNumber(lhs) % rhs;
    }

    friend LdplNumber operator*(double lhs, const LdplNumber &rhs) {
        return LdplNumber(lhs) * rhs;
    }

    friend LdplNumber operator+(double lhs, const LdplNumber &rhs) {
        return LdplNumber(lhs) + rhs;
    }

    friend LdplNumber operator-(double lhs, const LdplNumber &rhs) {
        return LdplNumber(lhs) - rhs;
    }

    friend LdplNumber operator/(double lhs, const LdplNumber &rhs) {
        return LdplNumber(lhs) / rhs;
    }

    friend LdplNumber operator%(double lhs, const LdplNumber &rhs) {
        return LdplNumber(lhs) % rhs;
    }
};

LdplNumber floor(LdplNumber a)
{
    if (a.IsInteger())
    {
        return a;
    }
    else
    {
        return LdplNumber(floor(a.internalFloatingValue()));
    }
}

LdplNumber ceil(LdplNumber a)
{
    if (a.IsInteger())
    {
        return a;
    }
    else
    {
        return LdplNumber(ceil(a.internalFloatingValue()));
    }
}

#endif

#ifndef GRAPHEMED_TEXT
#define GRAPHEMED_TEXT
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
class graphemedText
{
private:
    bool graphemeIndexDirty = true;
    vector<size_t> graphemeIndexMap;
    string stringRep;
    void createFromString(const string &cstr);
    void createFromChar(const char *cstr);
    void createFromMem(const char *mem, size_t len);
    void regenerateGraphemeIndex();

public:
    size_t size();
    bool empty() const;
    size_t length();
    string str_rep() const;
    graphemedText();
    graphemedText(const string &x);
    explicit graphemedText(const double &f);
    graphemedText &operator=(const string &x);
    graphemedText(const char *x);
    graphemedText &operator=(const char *x);
    graphemedText(const char *x, size_t len);
    explicit graphemedText(char x);
    graphemedText &operator=(char x);
    string operator[](size_t i);
    // string operator[](int i);
    friend ostream &operator<<(ostream &out, const graphemedText &c);
    friend graphemedText operator+(const graphemedText &c1, const graphemedText &c2);
    friend graphemedText operator+(const string &c1, const graphemedText &c2);
    friend graphemedText operator+(const graphemedText &c1, const string &c2);
    friend graphemedText operator+(const char *c1, const graphemedText &c2);
    friend graphemedText operator+(const graphemedText &c1, const char *c2);
    friend bool operator<(const graphemedText &c1, const graphemedText &c2);
    friend bool operator>(const graphemedText &c1, const graphemedText &c2);
    friend bool operator==(const graphemedText &ch1, const graphemedText &ch2);
    friend bool operator==(const graphemedText &ch1, const string &ch2);
    friend bool operator==(const graphemedText &ch1, const char *ch2);
    friend bool operator==(const string &ch2, const graphemedText &ch1);
    friend bool operator==(const char *ch2, const graphemedText &ch1);
    friend bool operator==(const graphemedText &ch1, const char ch2);
    friend bool operator==(const char ch2, const graphemedText &ch1);
    friend bool operator!=(const graphemedText &ch1, const graphemedText &ch2);
    bool loadFile(const string &fileName);
    graphemedText &operator+=(const graphemedText &txt);
    graphemedText &operator+=(const string &txt);
    graphemedText &operator+=(const char *txt);
    bool isAlphanumeric();
    bool isAlphanumeric(size_t from);
    bool isNumber() const;
    double getNumber() const;
    graphemedText substr(size_t from, size_t count);
    graphemedText &erase(size_t from, size_t count);
    graphemedText substr(size_t from);
    int compare(size_t from, size_t count, const graphemedText &other);
    int compare(const graphemedText &other);
};

ostream &operator<<(ostream &out, const graphemedText &c);
graphemedText operator+(const graphemedText &c1, const graphemedText &c2);
graphemedText operator+(const string &c1, const graphemedText &c2);
graphemedText operator+(const graphemedText &c1, const string &str);
graphemedText operator+(const char *c1, const graphemedText &c2);
graphemedText operator+(const graphemedText &c1, const char *str);
bool operator==(const graphemedText &ch1, const graphemedText &ch2);
bool operator==(const string &c1, const graphemedText &ch2);
bool operator==(const graphemedText &ch1, const string &c2);
bool operator==(const char *c1, const graphemedText &ch2);
bool operator==(const graphemedText &ch1, const char *c2);
bool operator==(const char c1, const graphemedText &ch2);
bool operator==(const graphemedText &ch1, const char c2);
bool operator<(const graphemedText &c1, const graphemedText &c2);
bool operator>(const graphemedText &c1, const graphemedText &c2);
bool operator!=(const graphemedText &ch1, const graphemedText &ch2);
graphemedText to_ldpl_string(ldpl_number x);
#endif

void graphemedText::regenerateGraphemeIndex()
{
    if (graphemeIndexDirty)
    {
        graphemeIndexDirty = false;

        graphemeIndexMap.clear();
        size_t i = 0;
        size_t currentGraphemeSize = 0;

        while (i < stringRep.length())
        {
            unsigned char c = stringRep[i];
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

            currentGraphemeSize += charLen;

            // Check for combining characters (this is a simplified check)
            bool isCombiningCharacter = false;
            if (charLen > 1)
            {
                int cp = 0;
                if (charLen == 2)
                {
                    cp = ((c & 0x1F) << 6) | (stringRep[i + 1] & 0x3F);
                }
                else if (charLen == 3)
                {
                    cp = ((c & 0x0F) << 12) | ((stringRep[i + 1] & 0x3F) << 6) | (stringRep[i + 2] & 0x3F);
                }
                else if (charLen == 4)
                {
                    cp = ((c & 0x07) << 18) | ((stringRep[i + 1] & 0x3F) << 12) | ((stringRep[i + 2] & 0x3F) << 6) | (stringRep[i + 3] & 0x3F);
                }
                if (cp >= 0x0300 && cp <= 0x036F)
                {
                    isCombiningCharacter = true;
                }
            }

            if (!isCombiningCharacter)
            {
                if (currentGraphemeSize > 0)
                {
                    graphemeIndexMap.push_back(i);
                }
                currentGraphemeSize = 0;
            }
            else
            {
                currentGraphemeSize += charLen;
            }

            i += charLen;
        }

        if (currentGraphemeSize > 0)
        {
            graphemeIndexMap.push_back(i);
        }

        // '\0'
        graphemeIndexMap.push_back(stringRep.length());
    }
}

// Fills buffer with utf8-encoded c++ string
void graphemedText::createFromString(const string &cstr)
{
    graphemeIndexDirty = true;
    stringRep = cstr;
}
// Fills buffer with utf8-encoded c++ string
void graphemedText::createFromChar(const char *cstr)
{
    graphemeIndexDirty = true;
    stringRep = cstr;
}
// Fills buffer with utf8-encoded c++ string
void graphemedText::createFromMem(const char *cstr, size_t cstrlen)
{
    graphemeIndexDirty = true;
    stringRep = string(cstr, cstrlen);
}
size_t graphemedText::size()
{
    if (stringRep.length() <= 1)
        return stringRep.length();
    regenerateGraphemeIndex();
    return graphemeIndexMap.size() - 1;
}
bool graphemedText::empty() const
{
    return stringRep.empty();
}
size_t graphemedText::length()
{
    return size();
}
string graphemedText::str_rep() const
{
    return stringRep;
}
// default constructor
graphemedText::graphemedText()
{
}
// conversion from string (constructor):
graphemedText::graphemedText(const string &x)
{
    createFromString(x);
}

// conversion from double (constructor):
graphemedText::graphemedText(const double &f)
{
    std::string str = to_string(f);
    str.erase(str.find_last_not_of('0') + 1, std::string::npos);
    str.erase(str.find_last_not_of('.') + 1, std::string::npos);
    createFromString(str);
}

// conversion from string (assignment):
graphemedText &graphemedText::operator=(const string &x)
{
    createFromString(x);
    return *this;
}
// conversion from char * (constructor):
graphemedText::graphemedText(const char *x)
{
    createFromChar(x);
}
// conversion from char * (assignment):
graphemedText &graphemedText::operator=(const char *x)
{
    createFromChar(x);
    return *this;
}
// conversion from char * and size (constructor):
graphemedText::graphemedText(const char *x, size_t len)
{
    createFromMem(x, len);
}
// conversion from char (constructor):
graphemedText::graphemedText(char x)
{
    createFromMem(&x, 1);
}
// conversion from char (assignment):
graphemedText &graphemedText::operator=(char x)
{
    createFromMem(&x, 1);
    return *this;
}
// [] for reading
string graphemedText::operator[](size_t i)
{
    regenerateGraphemeIndex();
    if (i >= size())
    {
        cout << "Out-of-bounds index access." << endl;
        exit(1);
    }
    return stringRep.substr(graphemeIndexMap[i], graphemeIndexMap[i + 1] - graphemeIndexMap[i]);
}

bool graphemedText::loadFile(const string &fileName)
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
graphemedText &graphemedText::operator+=(const graphemedText &txt)
{
    graphemeIndexDirty + true;
    stringRep += txt.stringRep;
    return *this;
}
// += operator
graphemedText &graphemedText::operator+=(const string &txt)
{
    graphemeIndexDirty + true;
    stringRep += txt;
    return *this;
}
// += operator
graphemedText &graphemedText::operator+=(const char *txt)
{
    graphemeIndexDirty + true;
    stringRep += txt;
    return *this;
}

bool graphemedText::isAlphanumeric()
{
    for (const char &c : stringRep)
        if (!isalnum(c))
            return false;
    return true;
}

bool graphemedText::isAlphanumeric(size_t from)
{
    for (size_t i = from; i < stringRep.length(); ++i)
    {
        if (!isalnum(stringRep[i]))
            return false;
    }
    return true;
}

bool graphemedText::isNumber() const
{
    unsigned int firstchar = 0;
    if (stringRep[0] == '-')
        firstchar = 1;
    if (stringRep[firstchar] == '.')
        return false; //.12 is not a valid decimal in LDPL, 0.12 is.
    if (stringRep[firstchar] == '+')
        return false; //+5 is not a valid decimal in LDPL, 5 is.
    istringstream iss(stringRep);
    double f;
    iss >> f;
    bool isNumber = iss.eof() && !iss.fail();
    // If it is a number, it might be an octal literal (e.g. 025, 067, 003 etc)
    // so we proceed to fix the original number to make it decimal.
    if (isNumber)
    {
        string f_number = "";
        unsigned int i;
        for (i = 1; i < stringRep.length(); ++i)
        {
            // If prev char not 0
            if (stringRep[i - 1] != '0')
            {
                // if prev char is -, continue check
                if (stringRep[i - 1] == '-')
                    f_number += '-';
                // if prev char is number, break
                else
                    break;
            }
            // If prev number is 0
            else if (stringRep[i] == '.')
            {
                f_number += '0';
            }
        }
        f_number += stringRep.substr(i - 1);
        return true;
    }
    else
    {
        return false;
    }
}

double graphemedText::getNumber() const
{
    return stod(stringRep);
}

graphemedText graphemedText::substr(size_t from, size_t count)
{
    regenerateGraphemeIndex();
    count = from + count > size() ? size() - from : count;
    return stringRep.substr(graphemeIndexMap[from], graphemeIndexMap[from + count] - graphemeIndexMap[from]);
}

graphemedText &graphemedText::erase(size_t from, size_t count)
{
    graphemeIndexMap.erase(graphemeIndexMap.begin() + from, graphemeIndexMap.begin() + from + count);
    stringRep = "";
    for (size_t i = from; i < from + count; ++i)
    {
        stringRep += graphemeIndexMap[i];
    }
    return *this;
}

graphemedText graphemedText::substr(size_t from)
{
    regenerateGraphemeIndex();
    string new_text = "";
    for (size_t i = from; i < size(); ++i)
    {
        new_text += graphemeIndexMap[i];
    }
    return new_text;
}

ostream &operator<<(ostream &out, const graphemedText &c)
{
    out << c.stringRep;
    return out;
}

graphemedText operator+(const graphemedText &c1, const graphemedText &c2)
{
    return c1.stringRep + c2.stringRep;
}

graphemedText operator+(const string &c1, const graphemedText &c2)
{
    return c1 + c2.stringRep;
}

graphemedText operator+(const graphemedText &c1, const string &str)
{
    return c1.stringRep + str;
}

graphemedText operator+(const char *c1, const graphemedText &c2)
{
    return c1 + c2.stringRep;
}

graphemedText operator+(const graphemedText &c1, const char *str)
{
    return c1.stringRep + str;
}

bool operator==(const graphemedText &ch1, const graphemedText &ch2)
{
    return ch1.stringRep == ch2.stringRep;
}

bool operator==(const string &c1, const graphemedText &ch2)
{
    return c1 == ch2.stringRep;
}

bool operator==(const graphemedText &ch1, const string &c2)
{
    return ch1.stringRep == c2;
}

bool operator==(const char *c1, const graphemedText &ch2)
{
    return c1 == ch2.stringRep;
}

bool operator==(const graphemedText &ch1, const char *c2)
{
    return c2 == ch1.stringRep;
}

bool operator==(const char c1, const graphemedText &ch2)
{
    return "" + c1 == ch2.stringRep;
}

bool operator==(const graphemedText &ch1, const char c2)
{
    return "" + c2 == ch1.stringRep;
}

bool operator<(const graphemedText &c1, const graphemedText &c2)
{
    return c1.stringRep < c2.stringRep;
}

bool operator>(const graphemedText &c1, const graphemedText &c2)
{
    return c1.stringRep > c2.stringRep;
}

bool operator!=(const graphemedText &ch1, const graphemedText &ch2)
{
    return ch1.stringRep != ch2.stringRep;
}

// ---------------------------------------------------------------------------------------------------

// Global variables
ifstream file_loading_stream;
ofstream file_writing_stream;
string file_loading_line;
ldpl_number VAR_ERRORCODE = 0;
graphemedText VAR_ERRORTEXT = (string) "";
ldpl_text joinvar; // Generic temporary use text variable (used by join but can be

// Forward declarations
graphemedText trimCopy(graphemedText _line);

#ifndef LDPLMAP
#define LDPLMAP
template <typename T>
struct ldpl_map
{
    unordered_map<string, T> inner_collection;
    T &operator[](graphemedText i);
    T &operator[](ldpl_number i);
    T &operator[](int i);
    bool operator==(const ldpl_map<T> &map2) const;
};
#endif

template <typename T>
bool ldpl_map<T>::operator==(const ldpl_map<T> &map2) const
{
    return this->inner_collection == map2.inner_collection;
}

template <typename T>
T &ldpl_map<T>::operator[](graphemedText i)
{
    return inner_collection[i.str_rep()];
}

template <typename T>
T &ldpl_map<T>::operator[](int i)
{
    return inner_collection[to_ldpl_string(i).str_rep()];
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
    return inner_collection[i.to_size_t()];
}

template <typename T>
void get_indices(ldpl_list<graphemedText> &dest, ldpl_vector<T> &source)
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

ldpl_number to_number(graphemedText textNumber)
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
    return a == b;
}

int str_cmp(graphemedText a, graphemedText b)
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
    LdplNumber f_a = floor(a);
    LdplNumber f_b = floor(b);
    return ((f_a % f_b) + f_b) % f_b;
}

// https://stackoverflow.com/a/27658515
graphemedText str_replace(string s, string find, string replace)
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

ldpl_number get_char_num(graphemedText chr)
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

graphemedText getAsciiChar(ldpl_number value)
{
    if (value < 0)
        return (string) "?";
    if (value > 127)
        return (string) "?";
    return graphemedText((char)(value.to_int()));
}

void join(const graphemedText &a, const graphemedText &b, graphemedText &c)
{
    c = a + b;
}

graphemedText charat(graphemedText &s, ldpl_number pos)
{
    size_t _pos = pos.to_size_t();
    return s[_pos];
}

graphemedText charat(char* s, ldpl_number pos)
{
    graphemedText newString = (string) s;
    size_t _pos = pos.to_size_t();
    return newString[_pos];
}

// Convert ldpl_number to LDPL string, killing trailing 0's
// https://stackoverflow.com/questions/16605967/ &
// https://stackoverflow.com/questions/13686482/
graphemedText to_ldpl_string(ldpl_number x)
{
    if (x.IsInteger())
    {
        return x.internalIntegerValue().to_string();
    }
    else
    {
        ostringstream out;
        out << fixed << setprecision(5) << x.to_double();
        string str = out.str();
        str.erase(str.find_last_not_of('0') + 1, string::npos);
        str.erase(str.find_last_not_of('.') + 1, string::npos);
        return str;
    }
}

#include <array>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>

graphemedText exec(const char *cmd)
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

void load_file(graphemedText filename, graphemedText &destination)
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
    destination = graphemedText((const char *)fmap, flen);
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
void save_to_file(graphemedText filename, graphemedText content, ios_base::openmode mode)
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

void append_to_file(graphemedText filename, graphemedText content)
{
    save_to_file(filename, content, ios_base::app);
}
void write_file(graphemedText filename, graphemedText content)
{
    save_to_file(filename, content, ios_base::out);
}

ldpl_number utf8GetIndexOf(graphemedText haystack, graphemedText needle)
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

ldpl_number utf8Count(graphemedText haystack, graphemedText needle)
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
graphemedText toUpperCopy(graphemedText str)
{
    string out = str.str_rep();
    std::transform(out.begin(), out.end(), out.begin(), ::toupper);
    return out;
}

// Converts string to lowercase
graphemedText toLowerCopy(graphemedText str)
{
    string out = str.str_rep();
    std::transform(out.begin(), out.end(), out.begin(), ::tolower);
    return out;
}

// Removes all trailing and ending whitespace from a string
graphemedText trimCopy(graphemedText _line)
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

void utf8_split_list(ldpl_list<graphemedText> &result, graphemedText haystack, graphemedText needle)
{
    result.inner_collection.clear();
    const int lenHaystack = haystack.size();
    const int lenNeedle = needle.size();
    if (lenNeedle > 0)
    {
        int i = 0;
        int last_start = 0;
        bool success = false;
        while (i + lenNeedle <= lenHaystack)
        {
            success = true;
            for (size_t x = 0; x < lenNeedle; ++x)
            {
                if (haystack[i + x] != needle[x])
                {
                    success = false;
                    break;
                }
            }
            if (success)
            {

                graphemedText token = haystack.substr(last_start, i - last_start);
                result.inner_collection.push_back(token);
                i += lenNeedle;
                last_start = i;
            }
            else
            {
                ++i;
            }
        }
        // Grab everything after the last needle
        graphemedText token = haystack.substr(last_start, lenHaystack - last_start);
        result.inner_collection.push_back(token);
    }
    else
    {
        // Split into individual characters
        for (int i = 0; i < lenHaystack; i++)
            result.inner_collection.push_back(charat(haystack, i));
    }
}

// Stream insertion operator
std::ostream &operator<<(std::ostream &os, const LdplNumber &num)
{
    if (num.isInteger)
    {
        os << num.integerValue.to_string();
    }
    else
    {
        os << to_ldpl_string(num.floatingValue);
    }
    return os;
}