#include <sstream>
#include <math.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <limits>
#include <limits.h>
#include <unordered_map>
#include <stdlib.h>
#include <chrono>
#include <thread>
#include <time.h>
#include <vector>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#define NVM_FLOAT_EPSILON 0.00000001
#define ldpl_number double
#define CRLF \"\\n\"
#define ldpl_vector ldpl_map
#define ldpl_list vector

#define ZERO_ERROR() { VAR_ERRORTEXT = \"\"; VAR_ERRORCODE = 0; }
#define TCP_ERROR(text, code, ret) { VAR_ERRORTEXT = text; VAR_ERRORCODE = (ldpl_number)code; return ret; }
#define RECV_BUF_SIZE 1024*10

using namespace std;

//Global variables
ifstream file_loading_stream;
ofstream file_writing_stream;
string file_loading_line;
string joinvar; //Generic temporary use text variable (used by join but can be used by any other statement as well)
ldpl_number VAR_ERRORCODE = 0;
string VAR_ERRORTEXT = \"\";
int server_fd, last_fd;
unordered_map<int, string> client_ips; //Map client fd to IPv4
int tcp_connection = -1; //TCP connection as client.
char tcp_input_buf[RECV_BUF_SIZE];
fd_set masterfds, tempfds;
uint16_t maxfd;

//Forward declarations
string to_ldpl_string(double x);

template<typename T>
struct ldpl_map {
    unordered_map<string, T> inner_map;

    T& operator [] (const string& i) {
        return inner_map[i];
    }

    T& operator [] (ldpl_number i) {
        return inner_map[to_ldpl_string(i)];
    }

    void clear(){
        inner_map.clear();
    }

    ldpl_number count(){
        return inner_map.size();
    }
};

template<typename T>
void get_indices(ldpl_vector<string> & dest, ldpl_vector<T> & source){
    dest.clear();
    int i = 0;
    for (const auto &keyPair : source.inner_map) {
        dest[i] = keyPair.first;
        ++i;
    }
}

template<typename T>
bool operator == (const ldpl_vector<T> &v1, const ldpl_vector<T> &v2){
    return (v1.inner_map == v2.inner_map);
}

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

ldpl_vector<string> utf8_split(const string & s, const string & sep){
    ldpl_vector<string> v;
    unsigned int z = 0;
    for(unsigned int i = 0; i < s.length();){
        int cplen = 1;
        int c = s[i];
        if      (c>=0 && c<=127)     cplen=1;
        else if ((c & 0xE0) == 0xC0) cplen=2;
        else if ((c & 0xF0) == 0xE0) cplen=3;
        else if ((c & 0xF8) == 0xF0) cplen=4;
        string cp = s.substr(i, cplen);
        if(sep.empty()) v[z++] = cp;
        else if(sep == cp) ++z;
        else v[z] += cp;
        i+=cplen;
    }
    return v;
}

ldpl_list<string> utf8_split_list(const string & s, const string & sep){
    ldpl_list<string> v;
    unsigned int z = 0;
    string current_token = \"\";
    for(unsigned int i = 0; i < s.length();){
        int cplen = 1;
        int c = s[i];
        if      (c>=0 && c<=127)     cplen=1;
        else if ((c & 0xE0) == 0xC0) cplen=2;
        else if ((c & 0xF0) == 0xE0) cplen=3;
        else if ((c & 0xF8) == 0xF0) cplen=4;
        string cp = s.substr(i, cplen); //Current char
        if(sep.empty()){
            v.push_back(cp);
        }else if(sep == cp){
            if(current_token.size() > 0) v.push_back(current_token);
            current_token = \"\";
        }else{
            current_token += cp;
        }
        i+=cplen;
    }
    if(current_token.size() > 0) v.push_back(current_token);
    return v;
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
std::string trimCopy(std::string line){
    //If the std::string is empty
    if(line.size() == 0) return line;

    //If the string has only one character
    if(line.size() == 1 && !std::isspace(line[0])) return line;

    //Left trim
    int first = 0;
    for(unsigned int i = 0; i < line.size(); ++i){
        if (!std::isspace(line[i])){
            first = i;
            break;
        }
    }

    //Right trim
    int last = 0;
    for(unsigned int i = line.size()-1; i >= 0; --i){
        if (!std::isspace(line[i])){
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

    //Trim the std::string
    line = line.substr(first, last-first);

    return line;
}

struct sockaddr_in to_addr(string hostname, int port)
{
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    struct hostent *h = gethostbyname(hostname.c_str());
    if(h == NULL){
        TCP_ERROR(\"bad hostname: \" + hostname, 1, addr);
    }else if(h->h_length <= 0){
        TCP_ERROR(\"gethostbyname() failed\", 2, addr);
    }
    char *ip = inet_ntoa(*(struct in_addr*)(h->h_addr_list[0]));
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port        = htons(port);
    return addr;
}

//Close the connection to a client.
void tcp_close(int fd)
{
    if(fd<0) return;
    close(fd); 
    FD_CLR(fd, &masterfds);
    client_ips.erase(fd);
    if(fd == tcp_connection) tcp_connection = -1;
}

//Close the connection to a client by IP.
void tcp_close(string ip)
{
    for (unordered_map<int, string>::const_iterator it = client_ips.begin(); it != client_ips.end(); ++it){
        if (it->second == ip) {
            tcp_close(it->first);
            break;
        }
    }
}

//Open persistent connection as client.
int tcp_connect(string host, int port)
{
    ZERO_ERROR();
    if(tcp_connection > 0)
        TCP_ERROR(\"connect() fail: connection open, CLOSE first\", -1, -1);

    int sock;
    if((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        TCP_ERROR(\"socket() failed\", 3, sock);

    struct sockaddr_in addr = to_addr(host, port);

    int e;
    if((e = connect(sock,(struct sockaddr*)&addr,sizeof(addr))) < 0)
        TCP_ERROR(\"connect() failed host:\"+host+\" port:\"+to_string(port), 4, e);

    if(sock < 0) return sock;

    tcp_connection = sock;
    return sock;
}

string tcp_send(int sock, string body)
{
    ZERO_ERROR();

    int sent = 0, total = 0;
    while(total < body.size()){
        if((sent = send(sock, body.c_str()+total, body.size()-total, MSG_DONTWAIT)) < 0)
            TCP_ERROR(\"send() call failed\", 13, \"\");
        total += sent;
    }

    string str;
    int bytes = 0, i = 0;
    char buf[RECV_BUF_SIZE];
    while((i=recv(sock,buf+bytes,RECV_BUF_SIZE-bytes,0))!=0){
        bytes+=i;
    }
    buf[bytes] = 0;
    str = buf;
    return str;
}

//Opens tcp connection, sends body, reads responses, then closes connection.
string tcp_send(string hostname, int port, string body)
{
    ZERO_ERROR();
    int tmpsock = -1, sock = tcp_connection;
    if(sock < 0){
        sock = tmpsock = tcp_connect(hostname, port);
        if(sock < 0) return \"\";
    }
    string str = tcp_send(sock, body);
    if(tmpsock>=0) tcp_close(tmpsock);
    return str;
}

string tcp_send(string body)
{
    if(tcp_connection<0) TCP_ERROR(\"no open connection\", -1, \"\");
    return tcp_send(tcp_connection, body);
}

//Starts TCP listening and returns server fd.
int tcp_listen(string hostname, int port)
{
    ZERO_ERROR();
    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sock < 0) TCP_ERROR(\"socket() failed\", 6, -1);

    int opt_value = 1;
    if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&opt_value, sizeof(int)) < 0)
        TCP_ERROR(\"setsockopt failed\", 7, -1);

    struct sockaddr_in my_addr = to_addr(hostname, port);
    if(::bind(sock, (struct sockaddr*)&my_addr, sizeof(my_addr)) < 0)
        TCP_ERROR(\"bind() failed host:\"+hostname+\" port:\"+to_string(port), 10, -1);

    FD_SET(sock, &masterfds);
	maxfd = sock;

    if(listen(sock, SOMAXCONN) < 0)
        TCP_ERROR(\"listen() failed host:\"+hostname+\" port:\"+to_string(port), 11, -1);

    return sock;
}

//Accepts new TCP client and returns client fd.
int tcp_accept()
{
    ZERO_ERROR();
    struct sockaddr_storage client;
    socklen_t len = sizeof(client);
    int client_fd = accept(server_fd, (struct sockaddr*)&client, &len);
    if(client_fd < 0) TCP_ERROR(\"accept() failed\", 12, -1);

    struct sockaddr_in *sin = (struct sockaddr_in *)&client;
    char ip[16];
    inet_ntop(AF_INET, sin, ip, 16);
    client_ips[client_fd] = ip;

    FD_SET(client_fd, &masterfds);
    if(client_fd > maxfd) maxfd = client_fd;

    return client_fd;
}

//Reads from fd and adds to tcp_input_buf.
void tcp_read(int fd)
{
    if(recv(fd, tcp_input_buf, RECV_BUF_SIZE, 0) <= 0){
        tcp_close(fd);
    }
}

void tcp_server(string host, ldpl_number port, ldpl_map<string> & var, void (*subpr)())
{
    FD_ZERO(&masterfds);
    FD_ZERO(&tempfds);
    memset(&tcp_input_buf, 0, RECV_BUF_SIZE);
    server_fd = tcp_listen(host, port);
    if(server_fd<0) return;
    while(1){
        tempfds = masterfds;
        int sel = select(maxfd + 1, &tempfds, NULL, NULL, NULL);
        if(sel<=0){ close(server_fd); break; }
        for(int i = 0; i <= maxfd; ++i){
            if(!FD_ISSET(i, &tempfds)) continue;
            if(server_fd == i) {
                tcp_accept();
            }else{
                last_fd = i;
                tcp_read(i);
                if(tcp_input_buf[0] != 0){
                    var[\"data\"] = tcp_input_buf;
                    var[\"ip\"] = client_ips[i];
                    subpr();
                }
                memset(&tcp_input_buf, 0, RECV_BUF_SIZE);
            }
        }
    }
}

//Replies to client. Must be called in LISTEN callback.
int tcp_reply(int fd, string msg)
{
    ZERO_ERROR();
    int sent = 0, bytes = 0;
    while(sent < msg.size()){
        if((bytes = send(fd, msg.c_str(), msg.size(), MSG_DONTWAIT)) < 0)
            TCP_ERROR(\"send() call failed\", 13, -1);
        sent += bytes;
    }
    return sent;
}
