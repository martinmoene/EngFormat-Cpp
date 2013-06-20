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

#include <iomanip>
#include <iostream>
#include <limits>

#include <math.h>

#if defined _MSC_VER && _MSC_VER <= 1200 // VC6
namespace std {
   double max( double const a, double const b ) { return a >= b ? a : b; }
}
#endif

bool approx( double const a, double const b )
{
    const double scale = 1.0;
    const double epsilon = 100 * std::numeric_limits<double>::epsilon();

    return fabs(a - b) < epsilon * (scale + (std::max)( fabs(a), fabs(b) ) );
}

int main()
{
    std::cout <<
       "to_engineering_string( 1e-23, 3, eng_prefixed ): '" <<
        to_engineering_string( 1e-23, 3, eng_prefixed ) << "'\n" <<

       "to_engineering_string( 1234, 3, eng_prefixed, \"Pa\" ): '" <<
        to_engineering_string( 1234, 3, eng_prefixed,  "Pa"  ) << "'\n" <<

       "step_engineering_string(\"1.00 k\", 3, eng_prefixed, eng_positive ): '" <<
        step_engineering_string( "1.00 k" , 3, eng_prefixed, eng_positive ) << "'\n" << std::endl;

    for ( int i = -24; i <= +24; ++i )
    {
        const double x = +1.00 * pow(10.0,i);

        const std::string text_inp = to_engineering_string( x, 3, eng_prefixed );
        const double             y = from_engineering_string( text_inp );
        const std::string text_out = to_engineering_string( y, 3, eng_prefixed );

        const std::string result = approx( x, y) ? "OK":"ERROR";

        std::cout <<
            result << "\t" <<
            x      << "       \t" <<
            y      << "       \t" <<
            "'"    << text_inp  << "'     \t" <<
            "'"    << text_out  << "'" << std::endl;
    }

    return 0; // VC6
}

// VC6, VC8, VC2010, g++4.8.1

// cl -nologo -W3 -EHsc -DENG_FORMAT_MICRO_GLYPH=\"u\" -I../src demo_eng_format.cpp ../src/eng_format.cpp && demo_eng_format
// g++ -Wall -std=c++03 -DENG_FORMAT_MICRO_GLYPH=\"u\" -I../src -o demo_eng_format.exe demo_eng_format.cpp ../src/eng_format.cpp && demo_eng_format
// g++ -Wall -std=c++11 -DENG_FORMAT_MICRO_GLYPH=\"u\" -I../src -o demo_eng_format.exe demo_eng_format.cpp ../src/eng_format.cpp && demo_eng_format
