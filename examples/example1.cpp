#include "eng_format.hpp"
#include <iostream>

int main()
{
    std::cout << to_engineering_string( 1234, 3, eng_prefixed, "Pa") << std::endl;
}

// cl -nologo -W3 -EHsc -I../src example1.cpp ../src/eng_format.cpp && example1
// g++ -Wall -Wextra -I../src -o example1.exe example1.cpp ../src/eng_format.cpp && example1
