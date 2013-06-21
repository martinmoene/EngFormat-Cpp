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

#include <cmath>
#include <stdlib.h>

struct caller {
    template <class T> caller(T fun) { fun(); }
};

# define CAT2(X, Y) X ## Y
# define CAT(X, Y) CAT2(X, Y)
# define TEST(NAME) caller CAT(__VAR, __LINE__) = []

bool approx( double const a, double const b )
{
	double dbls[2] = { a, b };
	
	for(int i=0; i<2; ++i) {
		int classify = fpclassify(dbls[i]);
		if(classify != FP_NORMAL) {
			switch(classify) {
			case FP_SUBNORMAL:
				printf("YIKES: subnormal!\n");
				return false;
			case FP_ZERO:
				break;
			default:
				printf("YIKES: unknown!\n");
				return false;
			case FP_INFINITE:
				printf("YIKES: Infinite\n");
				return false;
			case FP_NAN:
				printf("YIKES: NaN\n");
				return false;
			}
		}
	}

#if 0
    const double scale = 1.0;
    const double epsilon = 100 * std::numeric_limits<double>::epsilon();

    return fabs(a - b) < epsilon * (scale + (std::max)( fabs(a), fabs(b) ) );
#endif
    double foo = a - b;
	return !isnormal(foo);
}

#define ASSERT1(x) assert(x)
#define ASSERT2(x, y) {\
		std::string s = y;\
		/* std::cout << "ASSERT STR is:" << s << std::endl; */\
		assert(x == s);\
	}

#define print(a, b) \
    if (a != b) \
        std::cout << "'" << a << "' == '" << b << "'" << std::endl;
#define to_engineering_string_units to_engineering_string

TEST (dfh )
{
    ASSERT2( "100e96", to_engineering_string( 1e98, 3, eng_exponential ) );
};

TEST( basic_to )
{
    ASSERT2( "1.23 k", to_engineering_string( 1230, 3, false ) );
    ASSERT2( "1.23 k", to_engineering_string( 1230, 3, eng_prefixed ) );

    ASSERT2( "1.23e3", to_engineering_string( 1230, 3, true ) );
    ASSERT2( "1.23e3", to_engineering_string( 1230, 3, eng_exponential ) );

    ASSERT2( "0.00", to_engineering_string( 0, 3, eng_prefixed ) );
    ASSERT2( "0.00e0", to_engineering_string( 0, 3, eng_exponential ) );

    ASSERT2( "1.23 kPa", to_engineering_string_units( 1230, 3, false, "Pa" ) );
    ASSERT2( "1.23 kPa", to_engineering_string_units( 1230, 3, eng_prefixed, "Pa" ) );

    ASSERT2( "1.23e3 Pa", to_engineering_string_units( 1230, 3, true, "Pa"  ) );
    ASSERT2( "1.23e3 Pa", to_engineering_string_units( 1230, 3, eng_exponential, "Pa" ) );

    ASSERT2( "100e96", to_engineering_string( 1e98, 3, eng_exponential ) );
    ASSERT2( "-10.0e-99", to_engineering_string( -1e-98, 3, eng_exponential ) );

    ASSERT2( "999.999 Y", to_engineering_string( 999.999e24, 6, eng_prefixed ) );
    ASSERT2( "-1.00000 y", to_engineering_string( -999.9999e-27, 6, eng_prefixed ) );
};

TEST( basic_from )
{
    ASSERT1( approx( 98.76e-3, from_engineering_string( "98.76 m" ) ) );
    ASSERT1( approx( 98.76e-3, from_engineering_string( "98.76 ml" ) ) );
};

TEST( proper_rounding )
{
    ASSERT2( "100 z", to_engineering_string( 99.951e-21, 3, eng_prefixed ) );	// need the final 1 to force number past .950
    ASSERT2( "99.9 z", to_engineering_string( 99.949e-21, 3, eng_prefixed ) );

    ASSERT2( "100.09 z", to_engineering_string( 100.0949e-21, 5, eng_prefixed ) );
    ASSERT2( "99.999 z", to_engineering_string( 99.99851e-21, 5, eng_prefixed ) );
};

TEST( bad_data )
{
    ASSERT2( "NaN", to_engineering_string( strtod("NaN", NULL), 3, eng_prefixed ) );
    ASSERT2( "INFINITE", to_engineering_string( strtod("INFINITY", NULL), 3, eng_exponential ) );

    ASSERT2( "100e96", to_engineering_string( 1e98, 3, eng_prefixed ) );		// too big for eng prefix
    ASSERT2( "-10.0e-99", to_engineering_string( -1e-98, 3, eng_prefixed ) );	// too big for eng prefix

#pragma warning "FIX ME!"
	//ASSERT1( isnan(  from_engineering_string( " " ) ) );
	//ASSERT1( isnan(  from_engineering_string( "Howdie" ) ) );
	//ASSERT1( isnan(  from_engineering_string( "1 Q" ) ) );
	//ASSERT1( fpclassify( from_engineering_string( "0" ) ) & FP_ZERO );
};

TEST( prefixes )
{
    ASSERT1( approx( 1e-24, from_engineering_string( "1 y" ) ) );
    ASSERT1( approx( 1e-21, from_engineering_string( "1 z" ) ) );
    ASSERT1( approx( 1e-18, from_engineering_string( "1 a" ) ) );
    ASSERT1( approx( 1e-15, from_engineering_string( "1 f" ) ) );
    ASSERT1( approx( 1e-12, from_engineering_string( "1 p" ) ) );
    ASSERT1( approx( 1e-9 , from_engineering_string( "1 n" ) ) );
    ASSERT1( approx( 1e-6 , from_engineering_string( "1 u" ) ) );
    ASSERT1( approx( 1e-3 , from_engineering_string( "1 m" ) ) );
    ASSERT1( approx( 1    , from_engineering_string( "1 "  ) ) );
    ASSERT1( approx( 1    , from_engineering_string( "1"   ) ) );
    ASSERT1( approx( 1e+3 , from_engineering_string( "1 k" ) ) );
    ASSERT1( approx( 1e+6 , from_engineering_string( "1 M" ) ) );
    ASSERT1( approx( 1e+9 , from_engineering_string( "1 G" ) ) );
    ASSERT1( approx( 1e+12, from_engineering_string( "1 T" ) ) );
    ASSERT1( approx( 1e+15, from_engineering_string( "1 P" ) ) );
    ASSERT1( approx( 1e+18, from_engineering_string( "1 E" ) ) );
    ASSERT1( approx( 1e+21, from_engineering_string( "1 Z" ) ) );
    ASSERT1( approx( 1e+24, from_engineering_string( "1 Y" ) ) );
};

TEST( exponents_easy )
{
    ASSERT2( "1.23 y", to_engineering_string( 1.23e-24, 3, eng_prefixed ) );
    ASSERT2( "1.23 z", to_engineering_string( 1.23e-21, 3, eng_prefixed ) );
    ASSERT2( "1.23 a", to_engineering_string( 1.23e-18, 3, eng_prefixed ) );
    ASSERT2( "1.23 f", to_engineering_string( 1.23e-15, 3, eng_prefixed ) );
    ASSERT2( "1.23 p", to_engineering_string( 1.23e-12, 3, eng_prefixed ) );
    ASSERT2( "1.23 n", to_engineering_string( 1.23e-9 , 3, eng_prefixed ) );
    ASSERT2( "1.23 u", to_engineering_string( 1.23e-6 , 3, eng_prefixed ) );
    ASSERT2( "1.23 m", to_engineering_string( 1.23e-3 , 3, eng_prefixed ) );
    ASSERT2( "1.23"  , to_engineering_string( 1.23    , 3, eng_prefixed ) );
    ASSERT2( "1.23 k", to_engineering_string( 1.23e+3 , 3, eng_prefixed ) );
    ASSERT2( "1.23 M", to_engineering_string( 1.23e+6 , 3, eng_prefixed ) );
    ASSERT2( "1.23 G", to_engineering_string( 1.23e+9 , 3, eng_prefixed ) );
    ASSERT2( "1.23 T", to_engineering_string( 1.23e+12, 3, eng_prefixed ) );
    ASSERT2( "1.23 P", to_engineering_string( 1.23e+15, 3, eng_prefixed ) );
    ASSERT2( "1.23 E", to_engineering_string( 1.23e+18, 3, eng_prefixed ) );
    ASSERT2( "1.23 Z", to_engineering_string( 1.23e+21, 3, eng_prefixed ) );
    ASSERT2( "1.23 Y", to_engineering_string( 1.23e+24, 3, eng_prefixed ) );
};

TEST( exponents_hard )
{
    // minimally 3 digits:
    // Result e.g. "1000 z" in stead of "1 a":

#ifdef __cplusplus
    std::cout << "Warning(" << __LINE__ << "): Printing failing prefix tests:" << std::endl;
#endif

    print( "1.00 y", to_engineering_string( 1e-24, 1, eng_prefixed ) );
    print( "1.00 z", to_engineering_string( 1e-21, 3, eng_prefixed ) );
    print( "1.00 a", to_engineering_string( 1e-18, 1, eng_prefixed ) );
    print( "1.00 f", to_engineering_string( 1e-15, 1, eng_prefixed ) );
    print( "1.00 p", to_engineering_string( 1e-12, 1, eng_prefixed ) );
    print( "1.00 n", to_engineering_string( 1e-9 , 1, eng_prefixed ) );
    print( "1.00 u", to_engineering_string( 1e-6 , 1, eng_prefixed ) );
    print( "1.00 m", to_engineering_string( 1e-3 , 1, eng_prefixed ) );
    print( "1.00"  , to_engineering_string( 1    , 1, eng_prefixed ) );
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
    ASSERT1( approx( 54.32, from_engineering_string( to_engineering_string( 54.32, 4, false ) ) ) );
    ASSERT1( approx( 54.32, from_engineering_string( to_engineering_string( 54.32, 4, true  ) ) ) );

    ASSERT1( approx( 543.2e2, from_engineering_string( to_engineering_string( 543.2e2, 4, false ) ) ) );
    ASSERT1( approx( 543.2e2, from_engineering_string( to_engineering_string( 543.2e2, 4, true  ) ) ) );
};

TEST( step )
{
    ASSERT2( "1.01" , step_engineering_string( "1.0"  , 3, eng_prefixed, true ) );
    ASSERT2( "1.01e0", step_engineering_string( "1.0"  , 3, eng_exponential, true ) );

    ASSERT2( "1.01 k", step_engineering_string( "1.0 k", 3, eng_prefixed, true ) );
    ASSERT2( "1.01e3", step_engineering_string( "1.0 k", 3, eng_exponential, true ) );

    ASSERT2( "990 m" , step_engineering_string( "1.0"  , 3, eng_prefixed, false ) );
    ASSERT2( "990e-3", step_engineering_string( "1.0"  , 3, eng_exponential, false ) );

    ASSERT2( "990 k" , step_engineering_string( "1.0 M", 3, eng_prefixed, false ) );
    ASSERT2( "990e3" , step_engineering_string( "1.0 M", 3, eng_exponential, false ) );

    ASSERT2( "990"  , step_engineering_string( "1.0 k", 3, eng_prefixed, false ) );
    ASSERT2( "990e0" , step_engineering_string( "1.0 k", 3, eng_exponential, false ) );
};

int main()
{
}

// Requires C++11 lambda: VC2010+, g++ -std=c++11

// cl -nologo -W3 -EHsc -DENG_FORMAT_MICRO_GLYPH=\"u\" -I../src test_eng_format.cpp ../src/eng_format.cpp && test_eng_format
// g++ -Wall -Wextra -std=c++11 -DENG_FORMAT_MICRO_GLYPH=\"u\" -I../src -o test_eng_format.exe test_eng_format.cpp ../src/eng_format.cpp && test_eng_format
