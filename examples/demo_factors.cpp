// Copyright (C) 2013 by Martin Moene
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "eng_format.hpp"
#include <iostream>
#include <math.h>

int main()
{
    for ( int i = -27; i <= 27; ++i )
    {
        const double x = pow( 10.0, i );
        std::cout << x << " '" << to_engineering_string( x, 3, eng_prefixed   , "V" ) << "'" << std::endl;
        std::cout << x << " '" << to_engineering_string( x, 3, eng_exponential, "V" ) << "'"<< std::endl;
    }

    return 0; // VC6
}

// cl -nologo -W3 -EHsc -I../src demo_factors.cpp ../src/eng_format.cpp && demo_factors.exe
// g++ -Wall -I../src -o demo_factors.exe demo_factors.cpp ../src/eng_format.cpp && demo_factors.exe
