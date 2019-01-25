#ifndef CPPTRIM
#define CPPTRIM
#include <string>
#include <cctype>

//Removes all trailing and ending whitespace from a string
void trim(std::string & line){
    //If the std::string is empty
    if(line.size() == 0) return;
    
    //If the string has only one character
    if(line.size() == 1 && !std::isspace(line[0])) return;
    
    //Left trim
    int first = 0;
    for(int i = 0; i < line.size(); ++i){
        if (!std::isspace(line[i])){
            first = i;
            break;
        }
    }
    
    //Right trim
    int last = 0;
    for(int i = line.size()-1; i >= 0; --i){
        if (!std::isspace(line[i])){
            last = i+1;
            break;
        }
    }
    
    //Trim the std::string
    line = line.substr(first, last-first);
} 
#endif
