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

#include <cassert>
#include <iostream>
#include <limits>
#include <string>

#include <math.h>

struct caller {
    template <class T> caller(T fun) { fun(); }
};

# define CAT2(X, Y) X ## Y
# define CAT(X, Y) CAT2(X, Y)
# define TEST(NAME) caller CAT(__VAR, __LINE__) = []

bool approx( double const a, double const b )
{
    const double scale = 1.0;
    const double epsilon = 100 * std::numeric_limits<double>::epsilon();

    return fabs(a - b) < epsilon * (scale + (std::max)( fabs(a), fabs(b) ) );
}

TEST( basic_to )
{
    assert( "1.23 k" == to_engineering_string( 1230, 3, false ) );
    assert( "1.23 k" == to_engineering_string( 1230, 3, eng_prefixed ) );

    assert( "1.23e3" == to_engineering_string( 1230, 3, true ) );
    assert( "1.23e3" == to_engineering_string( 1230, 3, eng_exponential ) );

    assert( "1.23 kPa" == to_engineering_string( 1230, 3, false, "Pa" ) );
    assert( "1.23 kPa" == to_engineering_string( 1230, 3, eng_prefixed, "Pa" ) );

    assert( "1.23e3 Pa" == to_engineering_string( 1230, 3, true, "Pa"  ) );
    assert( "1.23e3 Pa" == to_engineering_string( 1230, 3, eng_exponential, "Pa" ) );
};

TEST( basic_from )
{
    assert( approx( 98.76e-3, from_engineering_string( "98.76 m" ) ) );
    assert( approx( 98.76e-3, from_engineering_string( "98.76 ml" ) ) );
};

TEST( prefixes )
{
    assert( approx( 1e-24, from_engineering_string( "1 y" ) ) );
    assert( approx( 1e-21, from_engineering_string( "1 z" ) ) );
    assert( approx( 1e-18, from_engineering_string( "1 a" ) ) );
    assert( approx( 1e-15, from_engineering_string( "1 f" ) ) );
    assert( approx( 1e-12, from_engineering_string( "1 p" ) ) );
    assert( approx( 1e-9 , from_engineering_string( "1 n" ) ) );
    assert( approx( 1e-6 , from_engineering_string( "1 u" ) ) );
    assert( approx( 1e-3 , from_engineering_string( "1 m" ) ) );
    assert( approx( 1    , from_engineering_string( "1 "  ) ) );
    assert( approx( 1    , from_engineering_string( "1"   ) ) );
    assert( approx( 1e+3 , from_engineering_string( "1 k" ) ) );
    assert( approx( 1e+6 , from_engineering_string( "1 M" ) ) );
    assert( approx( 1e+9 , from_engineering_string( "1 G" ) ) );
    assert( approx( 1e+12, from_engineering_string( "1 T" ) ) );
    assert( approx( 1e+15, from_engineering_string( "1 P" ) ) );
    assert( approx( 1e+18, from_engineering_string( "1 E" ) ) );
    assert( approx( 1e+21, from_engineering_string( "1 Z" ) ) );
    assert( approx( 1e+24, from_engineering_string( "1 Y" ) ) );
};

TEST( exponents_easy )
{
    assert( "1.23 y" == to_engineering_string( 1.23e-24, 3, eng_prefixed ) );
    assert( "1.23 z" == to_engineering_string( 1.23e-21, 3, eng_prefixed ) );
    assert( "1.23 a" == to_engineering_string( 1.23e-18, 3, eng_prefixed ) );
    assert( "1.23 f" == to_engineering_string( 1.23e-15, 3, eng_prefixed ) );
    assert( "1.23 p" == to_engineering_string( 1.23e-12, 3, eng_prefixed ) );
    assert( "1.23 n" == to_engineering_string( 1.23e-9 , 3, eng_prefixed ) );
    assert( "1.23 u" == to_engineering_string( 1.23e-6 , 3, eng_prefixed ) );
    assert( "1.23 m" == to_engineering_string( 1.23e-3 , 3, eng_prefixed ) );
    assert( "1.23 "  == to_engineering_string( 1.23    , 3, eng_prefixed ) );
    assert( "1.23 k" == to_engineering_string( 1.23e+3 , 3, eng_prefixed ) );
    assert( "1.23 M" == to_engineering_string( 1.23e+6 , 3, eng_prefixed ) );
    assert( "1.23 G" == to_engineering_string( 1.23e+9 , 3, eng_prefixed ) );
    assert( "1.23 T" == to_engineering_string( 1.23e+12, 3, eng_prefixed ) );
    assert( "1.23 P" == to_engineering_string( 1.23e+15, 3, eng_prefixed ) );
    assert( "1.23 E" == to_engineering_string( 1.23e+18, 3, eng_prefixed ) );
    assert( "1.23 Z" == to_engineering_string( 1.23e+21, 3, eng_prefixed ) );
    assert( "1.23 Y" == to_engineering_string( 1.23e+24, 3, eng_prefixed ) );
};

#define print(a, b) \
    if (a != b) \
        std::cout << "'" << a << "' == '" << b << "'" << std::endl;

TEST( exponents_hard )
{
    // minimally 3 digits:
    // Result e.g. "1000 z" in stead of "1 a":

    std::cout << "Warning(" << __LINE__ << "): Printing failing prefix tests:" << std::endl;

    print( "1.00 y", to_engineering_string( 1e-24, 1, eng_prefixed ) );
    print( "1.00 z", to_engineering_string( 1e-21, 1, eng_prefixed ) );
    print( "1.00 a", to_engineering_string( 1e-18, 1, eng_prefixed ) );
    print( "1.00 f", to_engineering_string( 1e-15, 1, eng_prefixed ) );
    print( "1.00 p", to_engineering_string( 1e-12, 1, eng_prefixed ) );
    print( "1.00 n", to_engineering_string( 1e-9 , 1, eng_prefixed ) );
    print( "1.00 u", to_engineering_string( 1e-6 , 1, eng_prefixed ) );
    print( "1.00 m", to_engineering_string( 1e-3 , 1, eng_prefixed ) );
    print( "1.00 " , to_engineering_string( 1    , 1, eng_prefixed ) );
    print( "1.00 k", to_engineering_string( 1e+3 , 1, eng_prefixed ) );
    print( "1.00 M", to_engineering_string( 1e+6 , 1, eng_prefixed ) );
    print( "1.00 G", to_engineering_string( 1e+9 , 1, eng_prefixed ) );
    print( "1.00 T", to_engineering_string( 1e+12, 1, eng_prefixed ) );
    print( "1.00 P", to_engineering_string( 1e+15, 1, eng_prefixed ) );
    print( "1.00 E", to_engineering_string( 1e+18, 1, eng_prefixed ) );
    print( "1.00 Z", to_engineering_string( 1e+21, 1, eng_prefixed ) );
    print( "1.00 Y", to_engineering_string( 1e+24, 1, eng_prefixed ) );
};

TEST( round_trip )
{
    assert( approx( 54.32, from_engineering_string( to_engineering_string( 54.32, 4, false ) ) ) );
    assert( approx( 54.32, from_engineering_string( to_engineering_string( 54.32, 4, true  ) ) ) );

    assert( approx( 543.2e2, from_engineering_string( to_engineering_string( 543.2e2, 4, false ) ) ) );
    assert( approx( 543.2e2, from_engineering_string( to_engineering_string( 543.2e2, 4, true  ) ) ) );
};

TEST( step )
{
    assert( "1.01 "  == step_engineering_string( "1.0"  , 3, eng_prefixed, eng_positive ) );
    assert( "1.01e0" == step_engineering_string( "1.0"  , 3, eng_exponential, eng_positive ) );

    assert( "1.01 k" == step_engineering_string( "1.0 k", 3, eng_prefixed, eng_positive ) );
    assert( "1.01e3" == step_engineering_string( "1.0 k", 3, eng_exponential, eng_positive ) );

    assert( "990 m"  == step_engineering_string( "1.0"  , 3, eng_prefixed, eng_negative ) );
    assert( "990e-3" == step_engineering_string( "1.0"  , 3, eng_exponential, eng_negative ) );

    assert( "990 k"  == step_engineering_string( "1.0 M", 3, eng_prefixed, eng_negative ) );
    assert( "990e3"  == step_engineering_string( "1.0 M", 3, eng_exponential, eng_negative ) );

    assert( "990 "   == step_engineering_string( "1.0 k", 3, eng_prefixed, eng_negative ) );
    assert( "990e0"  == step_engineering_string( "1.0 k", 3, eng_exponential, eng_negative ) );
};

int main()
{
}

// Requires C++11 lambda: VC2010+, g++ -std=c++11

// cl -nologo -W3 -EHsc -DENG_FORMAT_MICRO_GLYPH=\"u\" -I../src test_eng_format.cpp ../src/eng_format.cpp && test_eng_format
// g++ -Wall -Wextra -std=c++11 -DENG_FORMAT_MICRO_GLYPH=\"u\" -I../src -o test_eng_format.exe test_eng_format.cpp ../src/eng_format.cpp && test_eng_format
