#include <iostream>
#include <string>

extern std::string VAR_MYNAME;

void SUBPR_HELLO()
{
    std::cout << "Heyo, worldy!" << std::endl;
}

void SUBPR_GREET()
{
    std::cout << "Salutations, " << VAR_MYNAME << "!" << std::endl;
}