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
#define ldpl_number double
#define CRLF \"\\n\"
#define ldpl_vector ldpl_map

using namespace std;

//Global variables
ifstream file_loading_stream;
ofstream file_writing_stream;
string file_loading_line;
string joinvar; //Generic temporary use text variable (used by join but can be used by any other statement as well)
ldpl_number VAR_ERRORCODE = 0;
string VAR_ERRORTEXT = \"\";

//Forward declarations
string to_ldpl_string(double x);

template<typename T>
struct ldpl_map {
    unordered_map<string, T> inner_collection;

    T& operator [] (const string& i) {
        return inner_collection[i];
    }

    T& operator [] (ldpl_number i) {
        return inner_collection[to_ldpl_string(i)];
    }
};

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

template<typename T>
void get_indices(ldpl_vector<string> & dest, ldpl_vector<T> & source){
    dest.inner_collection.clear();
    int i = 0;
    for (const auto &keyPair : source.inner_collection) {
        dest[i] = keyPair.first;
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

//https://stackoverflow.com/a/27658515
string str_replace(string const& s, string const& find, string const& replace){
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

string charat(const string & s, ldpl_number pos){
    unsigned int _pos = floor(pos);
    return utf8_substr(s, pos, 1);
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
    random_device rd;
    mt19937 mt(rd());
    uniform_real_distribution<double> dist(0.0, 1.0);
    ldpl_number r = (ldpl_number) dist(mt);
    return r;
}

void load_file(string filename, string & destination)
{
    //Load file
    ifstream file(filename);
    //Fail if the file couldn't be loaded
    if(!file.is_open()){
        destination = \"\";
        VAR_ERRORTEXT = \"The file '\" + filename + \"' couldn't be opened.\";
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

ldpl_list<string> utf8_split_list(const string & haystack, const string & needle){
    ldpl_list<string> result;
    int lenHaystack = utf8_strlen(haystack);
    int lenNeedle = utf8_strlen(needle);
    if (lenNeedle > 0) {
        int i = 0;
        int last_start = 0;
        while (i + lenNeedle <= lenHaystack) {
            if (utf8_substr(haystack, i, lenNeedle) == needle) {
                result.inner_collection.push_back(utf8_substr(haystack, last_start, i - last_start));
                i += lenNeedle;
                last_start = i;
            } else {
                ++i;
            }
        }
        // Grab everything after the last needle
        result.inner_collection.push_back(utf8_substr(haystack, last_start, lenHaystack - last_start));
    } else {
        // Split into individual characters
        for (int i = 0; i < lenHaystack; i++)
            result.inner_collection.push_back(charat(haystack, i));
    }
    return result;
}

ldpl_vector<string> utf8_split(const string & haystack, const string & needle){
    ldpl_vector<string> result;
    ldpl_list<string> list_result = utf8_split_list(haystack, needle);
    for (int i = 0; i < list_result.inner_collection.size();  i++)
        result[i] = list_result[i];
    return result;
}

ldpl_number utf8GetIndexOf(string a, string needle){
    string haystack = a;
    int lenHaystack = utf8_strlen(haystack);
    int lenNeedle = utf8_strlen(needle);
    if(lenHaystack < lenNeedle) return -1;
    int i = 0;
    while(i + lenNeedle <= lenHaystack){
        if(utf8_substr(haystack, i, lenNeedle) == needle) return i;
        ++i;
    }
    return -1;
}

ldpl_number utf8Count(string a, string needle){
    string haystack = a;
    int lenHaystack = utf8_strlen(haystack);
    int lenNeedle = utf8_strlen(needle);
    if(lenHaystack < lenNeedle) return 0;
    int i = 0;
    int count = 0;
    while(i + lenNeedle <= lenHaystack){
        if(utf8_substr(haystack, i, lenNeedle) == needle) ++count;
        ++i;
    }
    return count;
}

//Removes all trailing and ending whitespace from a string
string trimCopy(string line){
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

//------------------------------------------------------------------------------------------------------------------------

/*
 * Dirent interface for Microsoft Visual Studio
 *
 * Copyright (C) 1998-2019 Toni Ronkko
 * This file is part of dirent.  Dirent may be freely distributed
 * under the MIT license.  For all details and documentation, see
 * https://github.com/tronkko/dirent
 */
#ifndef DIRENT_H
#define DIRENT_H

/* Hide warnings about unreferenced local functions */
#if defined(__clang__)
#   pragma clang diagnostic ignored \"-Wunused-function\"
#elif defined(_MSC_VER)
#   pragma warning(disable:4505)
#elif defined(__GNUC__)
#   pragma GCC diagnostic ignored \"-Wunused-function\"
#endif

/*
 * Include windows.h without Windows Sockets 1.1 to prevent conflicts with
 * Windows Sockets 2.0.
 */
#ifndef WIN32_LEAN_AND_MEAN
#   define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#include <stdio.h>
#include <stdarg.h>
#include <wchar.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

/* Indicates that d_type field is available in dirent structure */
#define _DIRENT_HAVE_D_TYPE

/* Indicates that d_namlen field is available in dirent structure */
#define _DIRENT_HAVE_D_NAMLEN

/* Entries missing from MSVC 6.0 */
#if !defined(FILE_ATTRIBUTE_DEVICE)
#   define FILE_ATTRIBUTE_DEVICE 0x40
#endif

/* File type and permission flags for stat(), general mask */
#if !defined(S_IFMT)
#   define S_IFMT _S_IFMT
#endif

/* Directory bit */
#if !defined(S_IFDIR)
#   define S_IFDIR _S_IFDIR
#endif

/* Character device bit */
#if !defined(S_IFCHR)
#   define S_IFCHR _S_IFCHR
#endif

/* Pipe bit */
#if !defined(S_IFFIFO)
#   define S_IFFIFO _S_IFFIFO
#endif

/* Regular file bit */
#if !defined(S_IFREG)
#   define S_IFREG _S_IFREG
#endif

/* Read permission */
#if !defined(S_IREAD)
#   define S_IREAD _S_IREAD
#endif

/* Write permission */
#if !defined(S_IWRITE)
#   define S_IWRITE _S_IWRITE
#endif

/* Execute permission */
#if !defined(S_IEXEC)
#   define S_IEXEC _S_IEXEC
#endif

/* Pipe */
#if !defined(S_IFIFO)
#   define S_IFIFO _S_IFIFO
#endif

/* Block device */
#if !defined(S_IFBLK)
#   define S_IFBLK 0
#endif

/* Link */
#if !defined(S_IFLNK)
#   define S_IFLNK 0
#endif

/* Socket */
#if !defined(S_IFSOCK)
#   define S_IFSOCK 0
#endif

/* Read user permission */
#if !defined(S_IRUSR)
#   define S_IRUSR S_IREAD
#endif

/* Write user permission */
#if !defined(S_IWUSR)
#   define S_IWUSR S_IWRITE
#endif

/* Execute user permission */
#if !defined(S_IXUSR)
#   define S_IXUSR 0
#endif

/* Read group permission */
#if !defined(S_IRGRP)
#   define S_IRGRP 0
#endif

/* Write group permission */
#if !defined(S_IWGRP)
#   define S_IWGRP 0
#endif

/* Execute group permission */
#if !defined(S_IXGRP)
#   define S_IXGRP 0
#endif

/* Read others permission */
#if !defined(S_IROTH)
#   define S_IROTH 0
#endif

/* Write others permission */
#if !defined(S_IWOTH)
#   define S_IWOTH 0
#endif

/* Execute others permission */
#if !defined(S_IXOTH)
#   define S_IXOTH 0
#endif

/* Maximum length of file name */
#if !defined(PATH_MAX)
#   define PATH_MAX MAX_PATH
#endif
#if !defined(FILENAME_MAX)
#   define FILENAME_MAX MAX_PATH
#endif
#if !defined(NAME_MAX)
#   define NAME_MAX FILENAME_MAX
#endif

/* File type flags for d_type */
#define DT_UNKNOWN 0
#define DT_REG S_IFREG
#define DT_DIR S_IFDIR
#define DT_FIFO S_IFIFO
#define DT_SOCK S_IFSOCK
#define DT_CHR S_IFCHR
#define DT_BLK S_IFBLK
#define DT_LNK S_IFLNK

/* Macros for converting between st_mode and d_type */
#define IFTODT(mode) ((mode) & S_IFMT)
#define DTTOIF(type) (type)

/*
 * File type macros.  Note that block devices, sockets and links cannot be
 * distinguished on Windows and the macros S_ISBLK, S_ISSOCK and S_ISLNK are
 * only defined for compatibility.  These macros should always return false
 * on Windows.
 */
#if !defined(S_ISFIFO)
#   define S_ISFIFO(mode) (((mode) & S_IFMT) == S_IFIFO)
#endif
#if !defined(S_ISDIR)
#   define S_ISDIR(mode) (((mode) & S_IFMT) == S_IFDIR)
#endif
#if !defined(S_ISREG)
#   define S_ISREG(mode) (((mode) & S_IFMT) == S_IFREG)
#endif
#if !defined(S_ISLNK)
#   define S_ISLNK(mode) (((mode) & S_IFMT) == S_IFLNK)
#endif
#if !defined(S_ISSOCK)
#   define S_ISSOCK(mode) (((mode) & S_IFMT) == S_IFSOCK)
#endif
#if !defined(S_ISCHR)
#   define S_ISCHR(mode) (((mode) & S_IFMT) == S_IFCHR)
#endif
#if !defined(S_ISBLK)
#   define S_ISBLK(mode) (((mode) & S_IFMT) == S_IFBLK)
#endif

/* Return the exact length of the file name without zero terminator */
#define _D_EXACT_NAMLEN(p) ((p)->d_namlen)

/* Return the maximum size of a file name */
#define _D_ALLOC_NAMLEN(p) ((PATH_MAX)+1)


#ifdef __cplusplus
extern \"C\" {
#endif


/* Wide-character version */
struct _wdirent {
    /* Always zero */
    long d_ino;

    /* File position within stream */
    long d_off;

    /* Structure size */
    unsigned short d_reclen;

    /* Length of name without \\0 */
    size_t d_namlen;

    /* File type */
    int d_type;

    /* File name */
    wchar_t d_name[PATH_MAX+1];
};
typedef struct _wdirent _wdirent;

struct _WDIR {
    /* Current directory entry */
    struct _wdirent ent;

    /* Private file data */
    WIN32_FIND_DATAW data;

    /* True if data is valid */
    int cached;

    /* Win32 search handle */
    HANDLE handle;

    /* Initial directory name */
    wchar_t *patt;
};
typedef struct _WDIR _WDIR;

/* Multi-byte character version */
struct dirent {
    /* Always zero */
    long d_ino;

    /* File position within stream */
    long d_off;

    /* Structure size */
    unsigned short d_reclen;

    /* Length of name without \\0 */
    size_t d_namlen;

    /* File type */
    int d_type;

    /* File name */
    char d_name[PATH_MAX+1];
};
typedef struct dirent dirent;

struct DIR {
    struct dirent ent;
    struct _WDIR *wdirp;
};
typedef struct DIR DIR;


/* Dirent functions */
static DIR *opendir (const char *dirname);
static _WDIR *_wopendir (const wchar_t *dirname);

static struct dirent *readdir (DIR *dirp);
static struct _wdirent *_wreaddir (_WDIR *dirp);

static int readdir_r(
    DIR *dirp, struct dirent *entry, struct dirent **result);
static int _wreaddir_r(
    _WDIR *dirp, struct _wdirent *entry, struct _wdirent **result);

static int closedir (DIR *dirp);
static int _wclosedir (_WDIR *dirp);

static void rewinddir (DIR* dirp);
static void _wrewinddir (_WDIR* dirp);

static int scandir (const char *dirname, struct dirent ***namelist,
    int (*filter)(const struct dirent*),
    int (*compare)(const struct dirent**, const struct dirent**));

static int alphasort (const struct dirent **a, const struct dirent **b);

static int versionsort (const struct dirent **a, const struct dirent **b);


/* For compatibility with Symbian */
#define wdirent _wdirent
#define WDIR _WDIR
#define wopendir _wopendir
#define wreaddir _wreaddir
#define wclosedir _wclosedir
#define wrewinddir _wrewinddir


/* Internal utility functions */
static WIN32_FIND_DATAW *dirent_first (_WDIR *dirp);
static WIN32_FIND_DATAW *dirent_next (_WDIR *dirp);

static int dirent_mbstowcs_s(
    size_t *pReturnValue,
    wchar_t *wcstr,
    size_t sizeInWords,
    const char *mbstr,
    size_t count);

static int dirent_wcstombs_s(
    size_t *pReturnValue,
    char *mbstr,
    size_t sizeInBytes,
    const wchar_t *wcstr,
    size_t count);

static void dirent_set_errno (int error);


/*
 * Open directory stream DIRNAME for read and return a pointer to the
 * internal working area that is used to retrieve individual directory
 * entries.
 */
static _WDIR*
_wopendir(
    const wchar_t *dirname)
{
    _WDIR *dirp;
    DWORD n;
    wchar_t *p;

    /* Must have directory name */
    if (dirname == NULL  ||  dirname[0] == '\\0') {
        dirent_set_errno (ENOENT);
        return NULL;
    }

    /* Allocate new _WDIR structure */
    dirp = (_WDIR*) malloc (sizeof (struct _WDIR));
    if (!dirp) {
        return NULL;
    }

    /* Reset _WDIR structure */
    dirp->handle = INVALID_HANDLE_VALUE;
    dirp->patt = NULL;
    dirp->cached = 0;

    /*
     * Compute the length of full path plus zero terminator
     *
     * Note that on WinRT there's no way to convert relative paths
     * into absolute paths, so just assume it is an absolute path.
     */
#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
    /* Desktop */
    n = GetFullPathNameW (dirname, 0, NULL, NULL);
#else
    /* WinRT */
    n = wcslen (dirname);
#endif

    /* Allocate room for absolute directory name and search pattern */
    dirp->patt = (wchar_t*) malloc (sizeof (wchar_t) * n + 16);
    if (dirp->patt == NULL) {
        goto exit_closedir;
    }

    /*
     * Convert relative directory name to an absolute one.  This
     * allows rewinddir() to function correctly even when current
     * working directory is changed between opendir() and rewinddir().
     *
     * Note that on WinRT there's no way to convert relative paths
     * into absolute paths, so just assume it is an absolute path.
     */
#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
    /* Desktop */
    n = GetFullPathNameW (dirname, n, dirp->patt, NULL);
    if (n <= 0) {
        goto exit_closedir;
    }
#else
    /* WinRT */
    wcsncpy_s (dirp->patt, n+1, dirname, n);
#endif

    /* Append search pattern \\* to the directory name */
    p = dirp->patt + n;
    switch (p[-1]) {
    case '\\\\':
    case '/':
    case ':':
        /* Directory ends in path separator, e.g. c:\\temp\\ */
        /*NOP*/;
        break;

    default:
        /* Directory name doesn't end in path separator */
        *p++ = '\\\\';
    }
    *p++ = '*';
    *p = '\\0';

    /* Open directory stream and retrieve the first entry */
    if (!dirent_first (dirp)) {
        goto exit_closedir;
    }

    /* Success */
    return dirp;

    /* Failure */
exit_closedir:
    _wclosedir (dirp);
    return NULL;
}

/*
 * Read next directory entry.
 *
 * Returns pointer to static directory entry which may be overwritten by
 * subsequent calls to _wreaddir().
 */
static struct _wdirent*
_wreaddir(
    _WDIR *dirp)
{
    struct _wdirent *entry;

    /*
     * Read directory entry to buffer.  We can safely ignore the return value
     * as entry will be set to NULL in case of error.
     */
    (void) _wreaddir_r (dirp, &dirp->ent, &entry);

    /* Return pointer to statically allocated directory entry */
    return entry;
}

/*
 * Read next directory entry.
 *
 * Returns zero on success.  If end of directory stream is reached, then sets
 * result to NULL and returns zero.
 */
static int
_wreaddir_r(
    _WDIR *dirp,
    struct _wdirent *entry,
    struct _wdirent **result)
{
    WIN32_FIND_DATAW *datap;

    /* Read next directory entry */
    datap = dirent_next (dirp);
    if (datap) {
        size_t n;
        DWORD attr;

        /*
         * Copy file name as wide-character string.  If the file name is too
         * long to fit in to the destination buffer, then truncate file name
         * to PATH_MAX characters and zero-terminate the buffer.
         */
        n = 0;
        while (n < PATH_MAX  &&  datap->cFileName[n] != 0) {
            entry->d_name[n] = datap->cFileName[n];
            n++;
        }
        entry->d_name[n] = 0;

        /* Length of file name excluding zero terminator */
        entry->d_namlen = n;

        /* File type */
        attr = datap->dwFileAttributes;
        if ((attr & FILE_ATTRIBUTE_DEVICE) != 0) {
            entry->d_type = DT_CHR;
        } else if ((attr & FILE_ATTRIBUTE_DIRECTORY) != 0) {
            entry->d_type = DT_DIR;
        } else {
            entry->d_type = DT_REG;
        }

        /* Reset dummy fields */
        entry->d_ino = 0;
        entry->d_off = 0;
        entry->d_reclen = sizeof (struct _wdirent);

        /* Set result address */
        *result = entry;

    } else {

        /* Return NULL to indicate end of directory */
        *result = NULL;

    }

    return /*OK*/0;
}

/*
 * Close directory stream opened by opendir() function.  This invalidates the
 * DIR structure as well as any directory entry read previously by
 * _wreaddir().
 */
static int
_wclosedir(
    _WDIR *dirp)
{
    int ok;
    if (dirp) {

        /* Release search handle */
        if (dirp->handle != INVALID_HANDLE_VALUE) {
            FindClose (dirp->handle);
        }

        /* Release search pattern */
        free (dirp->patt);

        /* Release directory structure */
        free (dirp);
        ok = /*success*/0;

    } else {

        /* Invalid directory stream */
        dirent_set_errno (EBADF);
        ok = /*failure*/-1;

    }
    return ok;
}

/*
 * Rewind directory stream such that _wreaddir() returns the very first
 * file name again.
 */
static void
_wrewinddir(
    _WDIR* dirp)
{
    if (dirp) {
        /* Release existing search handle */
        if (dirp->handle != INVALID_HANDLE_VALUE) {
            FindClose (dirp->handle);
        }

        /* Open new search handle */
        dirent_first (dirp);
    }
}

/* Get first directory entry (internal) */
static WIN32_FIND_DATAW*
dirent_first(
    _WDIR *dirp)
{
    WIN32_FIND_DATAW *datap;
    DWORD error;

    /* Open directory and retrieve the first entry */
    dirp->handle = FindFirstFileExW(
        dirp->patt, FindExInfoStandard, &dirp->data,
        FindExSearchNameMatch, NULL, 0);
    if (dirp->handle != INVALID_HANDLE_VALUE) {

        /* a directory entry is now waiting in memory */
        datap = &dirp->data;
        dirp->cached = 1;

    } else {

        /* Failed to open directory: no directory entry in memory */
        dirp->cached = 0;
        datap = NULL;

        /* Set error code */
        error = GetLastError ();
        switch (error) {
        case ERROR_ACCESS_DENIED:
            /* No read access to directory */
            dirent_set_errno (EACCES);
            break;

        case ERROR_DIRECTORY:
            /* Directory name is invalid */
            dirent_set_errno (ENOTDIR);
            break;

        case ERROR_PATH_NOT_FOUND:
        default:
            /* Cannot find the file */
            dirent_set_errno (ENOENT);
        }

    }
    return datap;
}

/*
 * Get next directory entry (internal).
 *
 * Returns
 */
static WIN32_FIND_DATAW*
dirent_next(
    _WDIR *dirp)
{
    WIN32_FIND_DATAW *p;

    /* Get next directory entry */
    if (dirp->cached != 0) {

        /* A valid directory entry already in memory */
        p = &dirp->data;
        dirp->cached = 0;

    } else if (dirp->handle != INVALID_HANDLE_VALUE) {

        /* Get the next directory entry from stream */
        if (FindNextFileW (dirp->handle, &dirp->data) != FALSE) {
            /* Got a file */
            p = &dirp->data;
        } else {
            /* The very last entry has been processed or an error occurred */
            FindClose (dirp->handle);
            dirp->handle = INVALID_HANDLE_VALUE;
            p = NULL;
        }

    } else {

        /* End of directory stream reached */
        p = NULL;

    }

    return p;
}

/*
 * Open directory stream using plain old C-string.
 */
static DIR*
opendir(
    const char *dirname)
{
    struct DIR *dirp;

    /* Must have directory name */
    if (dirname == NULL  ||  dirname[0] == '\\0') {
        dirent_set_errno (ENOENT);
        return NULL;
    }

    /* Allocate memory for DIR structure */
    dirp = (DIR*) malloc (sizeof (struct DIR));
    if (!dirp) {
        return NULL;
    }
    {
        int error;
        wchar_t wname[PATH_MAX + 1];
        size_t n;

        /* Convert directory name to wide-character string */
        error = dirent_mbstowcs_s(
            &n, wname, PATH_MAX + 1, dirname, PATH_MAX + 1);
        if (error) {
            /*
             * Cannot convert file name to wide-character string.  This
             * occurs if the string contains invalid multi-byte sequences or
             * the output buffer is too small to contain the resulting
             * string.
             */
            goto exit_free;
        }


        /* Open directory stream using wide-character name */
        dirp->wdirp = _wopendir (wname);
        if (!dirp->wdirp) {
            goto exit_free;
        }

    }

    /* Success */
    return dirp;

    /* Failure */
exit_free:
    free (dirp);
    return NULL;
}

/*
 * Read next directory entry.
 */
static struct dirent*
readdir(
    DIR *dirp)
{
    struct dirent *entry;

    /*
     * Read directory entry to buffer.  We can safely ignore the return value
     * as entry will be set to NULL in case of error.
     */
    (void) readdir_r (dirp, &dirp->ent, &entry);

    /* Return pointer to statically allocated directory entry */
    return entry;
}

/*
 * Read next directory entry into called-allocated buffer.
 *
 * Returns zero on success.  If the end of directory stream is reached, then
 * sets result to NULL and returns zero.
 */
static int
readdir_r(
    DIR *dirp,
    struct dirent *entry,
    struct dirent **result)
{
    WIN32_FIND_DATAW *datap;

    /* Read next directory entry */
    datap = dirent_next (dirp->wdirp);
    if (datap) {
        size_t n;
        int error;

        /* Attempt to convert file name to multi-byte string */
        error = dirent_wcstombs_s(
            &n, entry->d_name, PATH_MAX + 1, datap->cFileName, PATH_MAX + 1);

        /*
         * If the file name cannot be represented by a multi-byte string,
         * then attempt to use old 8+3 file name.  This allows traditional
         * Unix-code to access some file names despite of unicode
         * characters, although file names may seem unfamiliar to the user.
         *
         * Be ware that the code below cannot come up with a short file
         * name unless the file system provides one.  At least
         * VirtualBox shared folders fail to do this.
         */
        if (error  &&  datap->cAlternateFileName[0] != '\\0') {
            error = dirent_wcstombs_s(
                &n, entry->d_name, PATH_MAX + 1,
                datap->cAlternateFileName, PATH_MAX + 1);
        }

        if (!error) {
            DWORD attr;

            /* Length of file name excluding zero terminator */
            entry->d_namlen = n - 1;

            /* File attributes */
            attr = datap->dwFileAttributes;
            if ((attr & FILE_ATTRIBUTE_DEVICE) != 0) {
                entry->d_type = DT_CHR;
            } else if ((attr & FILE_ATTRIBUTE_DIRECTORY) != 0) {
                entry->d_type = DT_DIR;
            } else {
                entry->d_type = DT_REG;
            }

            /* Reset dummy fields */
            entry->d_ino = 0;
            entry->d_off = 0;
            entry->d_reclen = sizeof (struct dirent);

        } else {

            /*
             * Cannot convert file name to multi-byte string so construct
             * an erroneous directory entry and return that.  Note that
             * we cannot return NULL as that would stop the processing
             * of directory entries completely.
             */
            entry->d_name[0] = '?';
            entry->d_name[1] = '\\0';
            entry->d_namlen = 1;
            entry->d_type = DT_UNKNOWN;
            entry->d_ino = 0;
            entry->d_off = -1;
            entry->d_reclen = 0;

        }

        /* Return pointer to directory entry */
        *result = entry;

    } else {

        /* No more directory entries */
        *result = NULL;

    }

    return /*OK*/0;
}

/*
 * Close directory stream.
 */
static int
closedir(
    DIR *dirp)
{
    int ok;
    if (dirp) {

        /* Close wide-character directory stream */
        ok = _wclosedir (dirp->wdirp);
        dirp->wdirp = NULL;

        /* Release multi-byte character version */
        free (dirp);

    } else {

        /* Invalid directory stream */
        dirent_set_errno (EBADF);
        ok = /*failure*/-1;

    }
    return ok;
}

/*
 * Rewind directory stream to beginning.
 */
static void
rewinddir(
    DIR* dirp)
{
    /* Rewind wide-character string directory stream */
    _wrewinddir (dirp->wdirp);
}

/*
 * Scan directory for entries.
 */
static int
scandir(
    const char *dirname,
    struct dirent ***namelist,
    int (*filter)(const struct dirent*),
    int (*compare)(const struct dirent**, const struct dirent**))
{
    struct dirent **files = NULL;
    size_t size = 0;
    size_t allocated = 0;
    const size_t init_size = 1;
    DIR *dir = NULL;
    struct dirent *entry;
    struct dirent *tmp = NULL;
    size_t i;
    int result = 0;

    /* Open directory stream */
    dir = opendir (dirname);
    if (dir) {

        /* Read directory entries to memory */
        while (1) {

            /* Enlarge pointer table to make room for another pointer */
            if (size >= allocated) {
                void *p;
                size_t num_entries;

                /* Compute number of entries in the enlarged pointer table */
                if (size < init_size) {
                    /* Allocate initial pointer table */
                    num_entries = init_size;
                } else {
                    /* Double the size */
                    num_entries = size * 2;
                }

                /* Allocate first pointer table or enlarge existing table */
                p = realloc (files, sizeof (void*) * num_entries);
                if (p != NULL) {
                    /* Got the memory */
                    files = (dirent**) p;
                    allocated = num_entries;
                } else {
                    /* Out of memory */
                    result = -1;
                    break;
                }

            }

            /* Allocate room for temporary directory entry */
            if (tmp == NULL) {
                tmp = (struct dirent*) malloc (sizeof (struct dirent));
                if (tmp == NULL) {
                    /* Cannot allocate temporary directory entry */
                    result = -1;
                    break;
                }
            }

            /* Read directory entry to temporary area */
            if (readdir_r (dir, tmp, &entry) == /*OK*/0) {

                /* Did we get an entry? */
                if (entry != NULL) {
                    int pass;

                    /* Determine whether to include the entry in result */
                    if (filter) {
                        /* Let the filter function decide */
                        pass = filter (tmp);
                    } else {
                        /* No filter function, include everything */
                        pass = 1;
                    }

                    if (pass) {
                        /* Store the temporary entry to pointer table */
                        files[size++] = tmp;
                        tmp = NULL;

                        /* Keep up with the number of files */
                        result++;
                    }

                } else {

                    /*
                     * End of directory stream reached => sort entries and
                     * exit.
                     */
                    qsort (files, size, sizeof (void*),
                        (int (*) (const void*, const void*)) compare);
                    break;

                }

            } else {
                /* Error reading directory entry */
                result = /*Error*/ -1;
                break;
            }

        }

    } else {
        /* Cannot open directory */
        result = /*Error*/ -1;
    }

    /* Release temporary directory entry */
    free (tmp);

    /* Release allocated memory on error */
    if (result < 0) {
        for (i = 0; i < size; i++) {
            free (files[i]);
        }
        free (files);
        files = NULL;
    }

    /* Close directory stream */
    if (dir) {
        closedir (dir);
    }

    /* Pass pointer table to caller */
    if (namelist) {
        *namelist = files;
    }
    return result;
}

/* Alphabetical sorting */
static int
alphasort(
    const struct dirent **a, const struct dirent **b)
{
    return strcoll ((*a)->d_name, (*b)->d_name);
}

/* Sort versions */
static int
versionsort(
    const struct dirent **a, const struct dirent **b)
{
    /* FIXME: implement strverscmp and use that */
    return alphasort (a, b);
}

/* Convert multi-byte string to wide character string */
static int
dirent_mbstowcs_s(
    size_t *pReturnValue,
    wchar_t *wcstr,
    size_t sizeInWords,
    const char *mbstr,
    size_t count)
{
    int error;

#if defined(_MSC_VER)  &&  _MSC_VER >= 1400

    /* Microsoft Visual Studio 2005 or later */
    error = mbstowcs_s (pReturnValue, wcstr, sizeInWords, mbstr, count);

#else

    /* Older Visual Studio or non-Microsoft compiler */
    size_t n;

    /* Convert to wide-character string (or count characters) */
    n = mbstowcs (wcstr, mbstr, sizeInWords);
    if (!wcstr  ||  n < count) {

        /* Zero-terminate output buffer */
        if (wcstr  &&  sizeInWords) {
            if (n >= sizeInWords) {
                n = sizeInWords - 1;
            }
            wcstr[n] = 0;
        }

        /* Length of resulting multi-byte string WITH zero terminator */
        if (pReturnValue) {
            *pReturnValue = n + 1;
        }

        /* Success */
        error = 0;

    } else {

        /* Could not convert string */
        error = 1;

    }

#endif
    return error;
}

/* Convert wide-character string to multi-byte string */
static int
dirent_wcstombs_s(
    size_t *pReturnValue,
    char *mbstr,
    size_t sizeInBytes, /* max size of mbstr */
    const wchar_t *wcstr,
    size_t count)
{
    int error;

#if defined(_MSC_VER)  &&  _MSC_VER >= 1400

    /* Microsoft Visual Studio 2005 or later */
    error = wcstombs_s (pReturnValue, mbstr, sizeInBytes, wcstr, count);

#else

    /* Older Visual Studio or non-Microsoft compiler */
    size_t n;

    /* Convert to multi-byte string (or count the number of bytes needed) */
    n = wcstombs (mbstr, wcstr, sizeInBytes);
    if (!mbstr  ||  n < count) {

        /* Zero-terminate output buffer */
        if (mbstr  &&  sizeInBytes) {
            if (n >= sizeInBytes) {
                n = sizeInBytes - 1;
            }
            mbstr[n] = '\\0';
        }

        /* Length of resulting multi-bytes string WITH zero-terminator */
        if (pReturnValue) {
            *pReturnValue = n + 1;
        }

        /* Success */
        error = 0;

    } else {

        /* Cannot convert string */
        error = 1;

    }

#endif
    return error;
}

/* Set errno variable */
static void
dirent_set_errno(
    int error)
{
#if defined(_MSC_VER)  &&  _MSC_VER >= 1400

    /* Microsoft Visual Studio 2005 and later */
    _set_errno (error);

#else

    /* Non-Microsoft compiler or older Microsoft compiler */
    errno = error;

#endif
}


#ifdef __cplusplus
}
#endif
#endif /*DIRENT_H*/


//------------------------------------------------------------------------------------------------------------------------

ldpl_list<string> GetDirectoryFiles(const string dir) {
    //Taken from http://www.codebind.com/cpp-tutorial/cpp-program-list-files-directory-windows-linux/
    ldpl_list<string> files;
    shared_ptr<DIR> directory_ptr(opendir(dir.c_str()), [](DIR* dir){ dir && closedir(dir); });
    struct dirent *dirent_ptr;
    VAR_ERRORTEXT = \"\";
    VAR_ERRORCODE = 0;
    if (!directory_ptr) {
        VAR_ERRORTEXT = \"Error opening directory '\" + dir + \"'.\";
        VAR_ERRORCODE = 1;
        return files;
    }

    while ((dirent_ptr = readdir(directory_ptr.get())) != nullptr) {
        string fileName = string(dirent_ptr->d_name);
        if(fileName != \".\" && fileName != \"..\") files.inner_collection.push_back(fileName);
    }
    return files;
}

//------------------------------------------------------------------------------------------------------------------------