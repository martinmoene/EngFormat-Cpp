// Copyright (C) 2013 by Martin Moene
// Copyright (C) 2013 by David Hoerl
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

#include "lest.hpp"
#include "eng_format.hpp"

#include <cmath>
#include <iostream>
#include <limits>
#include <string>

std::string to_string( std::string  const & text ) { return text; };
std::string to_string( char const * const   text ) { return text; };

bool approx( double const a, double const b )
{
#if 0
    const double scale = 1.0;
    const double epsilon = 100 * std::numeric_limits<double>::epsilon();

    return fabs(a - b) < epsilon * (scale + (std::max)( fabs(a), fabs(b) ) );
#else
    double dbls[2] = { a, b };

    for ( int i = 0; i < 2; ++i )
    {
        int classify = std::fpclassify( dbls[i] );

        if ( classify != FP_NORMAL )
        {
            switch( classify )
            {
            case FP_SUBNORMAL:
                std::cout << "FP Error: subnormal!" << std::endl;
                return false;
            case FP_ZERO:
                break;
            case FP_INFINITE:
                std::cout << "FP Error: Infinite!" << std::endl;
                return false;
            case FP_NAN:
                std::cout << "FP Error: NaN!" << std::endl;
                return false;
            default:
                std::cout << "FP Error: unknown!" << std::endl;
                return false;
            }
        }
    }

    return ! std::isnormal( a - b );
#endif
}

const lest::test specification[] =
{
    CASE( "number converts well to string without prefix" )
    {
        EXPECT( "123" == to_engineering_string( 123, 3, false ) );
        EXPECT( "123" == to_engineering_string( 123, 3, eng_prefixed ) );
    },

    CASE( "number converts well to string using prefix" )
    {
        EXPECT( "1.23 k" == to_engineering_string( 1230, 3, false ) );
        EXPECT( "1.23 k" == to_engineering_string( 1230, 3, eng_prefixed ) );
    },

    CASE( "number converts well to string using prefix without spaces" )
    {
        EXPECT( "1.23k" == to_engineering_string( 1230, 3, false, "", "" ) );
        EXPECT( "1.23k" == to_engineering_string( 1230, 3, eng_prefixed, "", "" ) );
    },

    CASE( "number converts well to string using exponent" )
    {
        EXPECT( "1.23e3" == to_engineering_string( 1230, 3, true ) );
        EXPECT( "1.23e3" == to_engineering_string( 1230, 3, eng_exponential ) );
    },

    CASE( "number below 1000 converts well to string using exponent \"e0\"" )
    {
        EXPECT( "123e0" == to_engineering_string( 123, 3, true ) );
        EXPECT( "123e0" == to_engineering_string( 123, 3, eng_exponential ) );
    },

    CASE( "string using prefix converts well to number" )
    {
        EXPECT( lest::approx( 98.76e-3 ) == from_engineering_string( "98.76 m" ) );
        
        EXPECT( approx( 98.76e-3, from_engineering_string( "98.76 m" ) ) );
        EXPECT( approx( 98.76e-3, from_engineering_string( "98.76 ml" ) ) );
    },

    CASE( "string using prefix and empty separator converts well to number" "[not-implemented]" )
    {
        EXPECT( lest::approx( 98.76e-3 ) == from_engineering_string( "98.76m" ) );

        EXPECT( approx( 98.76e-3, from_engineering_string( "98.76m" ) ) );
        EXPECT( approx( 98.76e-3, from_engineering_string( "98.76ml" ) ) );
    },

    CASE( "string using exponent converts well to number" )
    {
        EXPECT( approx( 98.76e-3, from_engineering_string( "98.76e-3" ) ) );
        EXPECT( approx( 98.76e-3, from_engineering_string( "98.76e-3 l" ) ) );
    },

    CASE( "unit appropriately follows number converted to string" )
    {
#ifdef TEST_C_VERSION_BY_DH
        EXPECT( "1.23 kPa"  == to_engineering_string_units( 1230, 3, false, "Pa" ) );
        EXPECT( "1.23 kPa"  == to_engineering_string_units( 1230, 3, eng_prefixed, "Pa" ) );

        EXPECT( "1.23e3 Pa" == to_engineering_string_units( 1230, 3, true, "Pa"  ) );
        EXPECT( "1.23e3 Pa" == to_engineering_string_units( 1230, 3, eng_exponential, "Pa" ) );
#else
        EXPECT( "1.23 kPa"  == to_engineering_string( 1230, 3, false, "Pa" ) );
        EXPECT( "1.23 kPa"  == to_engineering_string( 1230, 3, eng_prefixed, "Pa" ) );

        EXPECT( "1.23e3 Pa" == to_engineering_string( 1230, 3, true, "Pa"  ) );
        EXPECT( "1.23e3 Pa" == to_engineering_string( 1230, 3, eng_exponential, "Pa" ) );
#endif
    },

    CASE( "zero converts well to string" )
    {
        EXPECT( "0.00"      == to_engineering_string( 0, 3, eng_prefixed ) );
        EXPECT( "0.00e0"    == to_engineering_string( 0, 3, eng_exponential ) );
    },

    CASE( "extreme converts well to string" )
    {
        EXPECT( "100e96"    == to_engineering_string( 1e98, 3, eng_exponential ) );
        EXPECT( "-10.0e-99" == to_engineering_string( -1e-98, 3, eng_exponential ) );

        EXPECT( "999.999 Y" == to_engineering_string( 999.999e24, 6, eng_prefixed ) );
        EXPECT("-1.00000 y" == to_engineering_string( -999.9999e-27, 6, eng_prefixed ) );
    },

    CASE( "prefix-exceeding value converts well to string" )
    {
        EXPECT( "100e96"    == to_engineering_string( 1e98, 3, eng_prefixed ) );
        EXPECT( "-10.0e-99" == to_engineering_string( -1e-98, 3, eng_prefixed ) );
    },

    CASE( "value rounds properly to string" )
    {
        // need the final 1 to force number past .950:
        EXPECT( "100 z"    == to_engineering_string( 99.951e-21, 3, eng_prefixed ) );
        EXPECT( "99.9 z"   == to_engineering_string( 99.949e-21, 3, eng_prefixed ) );

        EXPECT( "100.09 z" == to_engineering_string( 100.0949e-21, 5, eng_prefixed ) );
        EXPECT( "99.999 z" == to_engineering_string( 99.99851e-21, 5, eng_prefixed ) );
    },

    CASE( "bad floating point number converts accordingly to string" )
    {
        EXPECT( "NaN"      == to_engineering_string( NAN     , 3, eng_prefixed ) );
        EXPECT( "INFINITE" == to_engineering_string( INFINITY, 3, eng_exponential ) );

#pragma warning "FIX ME!"
        //EXPECT( isnan(  from_engineering_string( " " ) ) );
        //EXPECT( isnan(  from_engineering_string( "Howdie" ) ) );
        //EXPECT( isnan(  from_engineering_string( "1 Q" ) ) );
        //EXPECT( fpclassify( from_engineering_string( "0" ) ) & FP_ZERO );
    },

    CASE( "string using prefix converts well to number (y-Y)" )
    {
        EXPECT( approx( 1e-24, from_engineering_string( "1 y" ) ) );
        EXPECT( approx( 1e-21, from_engineering_string( "1 z" ) ) );
        EXPECT( approx( 1e-18, from_engineering_string( "1 a" ) ) );
        EXPECT( approx( 1e-15, from_engineering_string( "1 f" ) ) );
        EXPECT( approx( 1e-12, from_engineering_string( "1 p" ) ) );
        EXPECT( approx( 1e-9 , from_engineering_string( "1 n" ) ) );
        EXPECT( approx( 1e-6 , from_engineering_string( "1 u" ) ) );
        EXPECT( approx( 1e-3 , from_engineering_string( "1 m" ) ) );
        EXPECT( approx( 1    , from_engineering_string( "1 "  ) ) );
        EXPECT( approx( 1    , from_engineering_string( "1"   ) ) );
        EXPECT( approx( 1e+3 , from_engineering_string( "1 k" ) ) );
        EXPECT( approx( 1e+6 , from_engineering_string( "1 M" ) ) );
        EXPECT( approx( 1e+9 , from_engineering_string( "1 G" ) ) );
        EXPECT( approx( 1e+12, from_engineering_string( "1 T" ) ) );
        EXPECT( approx( 1e+15, from_engineering_string( "1 P" ) ) );
        EXPECT( approx( 1e+18, from_engineering_string( "1 E" ) ) );
        EXPECT( approx( 1e+21, from_engineering_string( "1 Z" ) ) );
        EXPECT( approx( 1e+24, from_engineering_string( "1 Y" ) ) );
    },

    CASE( "value with exponent converts well to string using prefix (easy)" )
    {
        EXPECT( "1.23 y" == to_engineering_string( 1.23e-24, 3, eng_prefixed ) );
        EXPECT( "1.23 z" == to_engineering_string( 1.23e-21, 3, eng_prefixed ) );
        EXPECT( "1.23 a" == to_engineering_string( 1.23e-18, 3, eng_prefixed ) );
        EXPECT( "1.23 f" == to_engineering_string( 1.23e-15, 3, eng_prefixed ) );
        EXPECT( "1.23 p" == to_engineering_string( 1.23e-12, 3, eng_prefixed ) );
        EXPECT( "1.23 n" == to_engineering_string( 1.23e-9 , 3, eng_prefixed ) );
        EXPECT( "1.23 u" == to_engineering_string( 1.23e-6 , 3, eng_prefixed ) );
        EXPECT( "1.23 m" == to_engineering_string( 1.23e-3 , 3, eng_prefixed ) );
        EXPECT( "1.23"   == to_engineering_string( 1.23    , 3, eng_prefixed ) );
        EXPECT( "1.23 k" == to_engineering_string( 1.23e+3 , 3, eng_prefixed ) );
        EXPECT( "1.23 M" == to_engineering_string( 1.23e+6 , 3, eng_prefixed ) );
        EXPECT( "1.23 G" == to_engineering_string( 1.23e+9 , 3, eng_prefixed ) );
        EXPECT( "1.23 T" == to_engineering_string( 1.23e+12, 3, eng_prefixed ) );
        EXPECT( "1.23 P" == to_engineering_string( 1.23e+15, 3, eng_prefixed ) );
        EXPECT( "1.23 E" == to_engineering_string( 1.23e+18, 3, eng_prefixed ) );
        EXPECT( "1.23 Z" == to_engineering_string( 1.23e+21, 3, eng_prefixed ) );
        EXPECT( "1.23 Y" == to_engineering_string( 1.23e+24, 3, eng_prefixed ) );
    },

    CASE( "value with exponent converts well to string with prefix (hard)" )
    {
        EXPECT( "1.00 y" == to_engineering_string( 1e-24, 3, eng_prefixed ) );
        EXPECT( "1.00 z" == to_engineering_string( 1e-21, 3, eng_prefixed ) );
        EXPECT( "1.00 a" == to_engineering_string( 1e-18, 3, eng_prefixed ) );
        EXPECT( "1.00 f" == to_engineering_string( 1e-15, 3, eng_prefixed ) );
        EXPECT( "1.00 p" == to_engineering_string( 1e-12, 3, eng_prefixed ) );
        EXPECT( "1.00 n" == to_engineering_string( 1e-9 , 3, eng_prefixed ) );
        EXPECT( "1.00 u" == to_engineering_string( 1e-6 , 3, eng_prefixed ) );
        EXPECT( "1.00 m" == to_engineering_string( 1e-3 , 3, eng_prefixed ) );
        EXPECT( "1.00"   == to_engineering_string( 1    , 3, eng_prefixed ) );
        EXPECT( "1.00 k" == to_engineering_string( 1e+3 , 3, eng_prefixed ) );
        EXPECT( "1.00 M" == to_engineering_string( 1e+6 , 3, eng_prefixed ) );
        EXPECT( "1.00 G" == to_engineering_string( 1e+9 , 3, eng_prefixed ) );
        EXPECT( "1.00 T" == to_engineering_string( 1e+12, 3, eng_prefixed ) );
        EXPECT( "1.00 P" == to_engineering_string( 1e+15, 3, eng_prefixed ) );
        EXPECT( "1.00 E" == to_engineering_string( 1e+18, 3, eng_prefixed ) );
        EXPECT( "1.00 Z" == to_engineering_string( 1e+21, 3, eng_prefixed ) );
        EXPECT( "1.00 Y" == to_engineering_string( 1e+24, 3, eng_prefixed ) );
    },

    CASE( "round-trip conversion on number without exponent succeeds" )
    {
        EXPECT( approx( 54.32, from_engineering_string( to_engineering_string( 54.32, 4, false ) ) ) );
        EXPECT( approx( 54.32, from_engineering_string( to_engineering_string( 54.32, 4, true  ) ) ) );
    },

    CASE( "round-trip conversion on number with exponent succeeds" )
    {
        EXPECT( approx( 543.2e2, from_engineering_string( to_engineering_string( 543.2e2, 4, false ) ) ) );
        EXPECT( approx( 543.2e2, from_engineering_string( to_engineering_string( 543.2e2, 4, true  ) ) ) );
    },

    CASE( "step up succeeds for string without prefix or exponent" )
    {
        EXPECT( "1.01"   == step_engineering_string( "1.0"   , 3, eng_prefixed, eng_increment ) );
        EXPECT( "1.01e0" == step_engineering_string( "1.0"   , 3, eng_exponential, eng_increment ) );
    },

    CASE( "step down succeeds for string without prefix or exponent" )
    {
        EXPECT( "1.00"   == step_engineering_string( "1.01"  , 3, eng_prefixed, eng_decrement ) );
        EXPECT( "1.00e0" == step_engineering_string( "1.01"  , 3, eng_exponential, eng_decrement ) );
    },

    CASE( "step up succeeds for string with zero value" )
    {
        EXPECT( "10.0 m"  == step_engineering_string( "0.0"  , 3, eng_prefixed, eng_increment ) );
        EXPECT( "10.0e-3" == step_engineering_string( "0.0"  , 3, eng_exponential, eng_increment ) );
    },

    CASE( "step up succeeds for string with prefix" )
    {
        EXPECT( "1.01 k" == step_engineering_string( "1.0 k" , 3, eng_prefixed, eng_increment ) );
        EXPECT( "1.01e3" == step_engineering_string( "1.0 k" , 3, eng_exponential, eng_increment ) );
    },

    CASE( "step down succeeds for string with prefix" )
    {
        EXPECT( "1.00 k" == step_engineering_string( "1.01 k", 3, eng_prefixed, eng_decrement ) );
        EXPECT( "1.00e3" == step_engineering_string( "1.01 k", 3, eng_exponential, eng_decrement ) );
    },

    CASE( "step up succeeds for string with exponent" )
    {
        EXPECT( "1.01 k" == step_engineering_string( "1.0e3" , 3, eng_prefixed, eng_increment ) );
        EXPECT( "1.01e3" == step_engineering_string( "1.0e3" , 3, eng_exponential, eng_increment ) );
    },

    CASE( "step down succeeds for string with exponent" )
    {
        EXPECT( "1.00 k" == step_engineering_string( "1.01e3", 3, eng_prefixed, eng_decrement ) );
        EXPECT( "1.00e3" == step_engineering_string( "1.01e3", 3, eng_exponential, eng_decrement ) );
    },

    CASE( "step up to prefix 'k' succeeds for string without prefix" )
    {
        EXPECT( "1.00 k" == step_engineering_string( "999"   , 3, eng_prefixed, eng_increment ) );
    },

    CASE( "step down to loose prefix succeeds for string with prefix 'k'" )
    {
        EXPECT( "999.0"  == step_engineering_string( "1.000 k", 4, eng_prefixed, eng_decrement ) );
    },

    CASE( "step up to loose prefix succeeds for string with prefix 'm'" )
    {
        EXPECT( "1.00"  == step_engineering_string( "999 m"  , 3, eng_prefixed, eng_increment ) );
    },

    CASE( "step down to use prefix or exponent succeeds for string without prefix" )
    {
        EXPECT( "990 m"  == step_engineering_string( "1.0"   , 3, eng_prefixed, eng_decrement ) );
        EXPECT( "990e-3" == step_engineering_string( "1.0"   , 3, eng_exponential, eng_decrement ) );
    },

    CASE( "step up to next prefix or exponent succeeds for string with prefix" )
    {
        EXPECT( "1.00 G" == step_engineering_string( "999 M" , 3, eng_prefixed, eng_increment ) );
        EXPECT( "1.00e9" == step_engineering_string( "999 M" , 3, eng_exponential, eng_increment ) );
    },

    CASE( "step down to next prefix or exponent succeeds for string with prefix" )
    {
        EXPECT( "990 k" == step_engineering_string( "1.0 M"  , 3, eng_prefixed, eng_decrement ) );
        EXPECT( "990e3" == step_engineering_string( "1.0 M"  , 3, eng_exponential, eng_decrement ) );
    },
};

int main( int argc, char* argv[] )
{
    return lest::run( specification, argc, argv );
}

// lest test framework requires C++11 lambda: VC2013 preview, g++ -std=c++11

// compilation control to test C routine to_engineering_string_units(): -DTEST_C_VERSION_BY_DH


// cl -nologo -W3 -EHsc -DENG_FORMAT_MICRO_GLYPH=\"u\" -I../src test_eng_format.cpp ../src/eng_format.cpp && test_eng_format
// g++ -Wall -Wextra -std=c++11 -Wno-missing-braces -DENG_FORMAT_MICRO_GLYPH=\"u\" -I../src -o test_eng_format.exe test_eng_format.cpp ../src/eng_format.cpp && test_eng_format
