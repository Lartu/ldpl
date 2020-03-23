#include <iostream>
#include <string>
#include "../../src/ldpl.h"

ldpl_text MY_NAME;

void HELLO()
{
    std::cout << "Heyo, worldy!" << std::endl;
}

void GREET()
{
    std::cout << "Salutations, " << MY_NAME << "!" << std::endl;
}