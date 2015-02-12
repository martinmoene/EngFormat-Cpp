// Copyright (C) 2005-2009 by Jukka Korpela
// Copyright (C) 2009-2013 by David Hoerl
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
#include <limits>
#include <sstream>

#include <float.h>
#include <math.h>
#include <stdlib.h>

/*
 * Note: using fabs() and other math functions in global namespace for
 * best compiler coverage.
 */

/*
 * Note: micro, �, may not work everywhere, so you can define a glyph yourself:
 */
#ifndef ENG_FORMAT_MICRO_GLYPH
# define ENG_FORMAT_MICRO_GLYPH "�"
#endif

/*
 * Note: if not using signed at the computation of prefix_end below,
 * VC2010 -Wall issues a warning about unsigned and addition overflow.
 * Hence the cast to signed int here.
 */
#define ENG_FORMAT_DIMENSION_OF(a) ( static_cast<int>( sizeof(a) / sizeof(0[a]) ) )

eng_prefixed_t eng_prefixed;
eng_exponential_t eng_exponential;

namespace
{

char const * const prefixes[/*exp*/][2][9] =
{
    {
        {   "",   "m",   ENG_FORMAT_MICRO_GLYPH
                            ,   "n",    "p",    "f",    "a",    "z",    "y", },
        {   "",   "k",   "M",   "G",    "T",    "P",    "E",    "Z",    "Y", },
    },
    {
        { "e0", "e-3", "e-6", "e-9", "e-12", "e-15", "e-18", "e-21", "e-24", },
        { "e0",  "e3",  "e6",  "e9",  "e12",  "e15",  "e18",  "e21",  "e24", },
    },
};

const int prefix_count = ENG_FORMAT_DIMENSION_OF( prefixes[false][false]  );

#if defined( _MSC_VER )

template <typename T>
long lrint( T const x )
{
    return static_cast<long>( x );
}

#endif

int sign( int const value )
{
    return value == 0 ? +1 : value / abs( value );
}

bool is_zero( double const value )
{
#if __cplusplus >= 201103L
    return FP_ZERO == fpclassify( value );
#else
    // deliberately compare literally:
    return 0.0 == value;
#endif
}

bool is_nan( double const value )
{
#if __cplusplus >= 201103L
    return isnan( value );
#else
    // deliberately return false for now:
    return false;
#endif
}

bool is_inf( double const value )
{
#if __cplusplus >= 201103L
    return isinf( value );
#else
    // deliberately return false for now:
    return false;
#endif
}

long degree_of( double const value )
{
    return is_zero( value ) ? 0 : lrint( floor( log10( fabs( value ) ) / 3) );
}

int precision( double const scaled, int const digits )
{
    // MSVC6 requires -2 * DBL_EPSILON;
    // g++ 4.8.1: ok with -1 * DBL_EPSILON

    return is_zero( scaled ) ? digits - 1 : digits - log10( fabs( scaled ) ) - 2 * DBL_EPSILON;
}

std::string prefix_or_exponent( bool const exponential, int const degree, bool const include_space )
{
    return std::string( !include_space || exponential || 0 == degree ? "" : " " ) + prefixes[ exponential ][ sign(degree) > 0 ][ abs( degree ) ];
}

std::string exponent( int const degree )
{
    std::ostringstream os;
    os << "e" << 3 * degree;
    return os.str();
}

/*
 * engineering to exponent notation conversion.
 */
std::string engineering_to_exponent( std::string text );

} // anonymous namespace

/**
 * convert real number to prefixed or exponential notation, optionally followed by a unit.
 */
std::string
to_engineering_string( double const value, int const digits, bool exponential, std::string const unit /*= ""*/, bool include_space /*= true*/ )
{
    if      ( is_nan( value ) ) return "NaN";
    else if ( is_inf( value ) ) return "INFINITE";

    const int degree = degree_of( value );

    std::string factor;

    if ( abs( degree ) < prefix_count )
    {
        factor = prefix_or_exponent( exponential, degree, include_space );
    }
    else
    {
        exponential = true;
        factor = exponent( degree );
    }

    std::ostringstream os;

    const double scaled = value * pow( 1000.0, -degree );

    const std::string space = include_space && ( 0 == degree || exponential ) && unit.length() ? " ":"";

    os << std::fixed << std::setprecision( precision( scaled, digits ) ) << scaled << factor << space << unit;

    return os.str();
}

/**
 * convert the output of to_engineering_string() into a double.
 */
double from_engineering_string( std::string const text )
{
    return strtod( engineering_to_exponent( text ).c_str(), NULL );
}

/**
 * step a value by the smallest possible increment.
 */
std::string step_engineering_string( std::string const text, int digits, bool const exponential, bool const positive )
{
    const double value = from_engineering_string( text );

    if ( digits < 3 )
    {
        digits = 3;
    }

    // correctly round to desired precision
    const int expof10 = is_zero(value) ? 0 : lrint( floor( log10( value ) ) );
    const int   power = expof10 + 1 - digits;

    const double  inc = pow( 10.0, power ) * ( positive ? +1 : -1 );
    const double  ret = value + inc;

    return to_engineering_string( ret, digits, exponential );
}

namespace
{

/*
 * "k" => "1e3"
 */
std::string prefix_to_exponent( std::string const pfx )
{
    for ( int i = 0; i < 2; ++i )
    {
        for( int k = 0; k < prefix_count; ++k )
        {
            if ( pfx == prefixes[0][i][k] )
            {
                return prefixes[1][i][k] ;
            }
        }
    }
    return "";
}

/*
 * Convert engineering presentation to presentation with exponent.
 *
 * The engineering presentation should not contain a unit, as the first letter
 * is interpreted as an SI prefix, e.g. "1 T" is 1e12, not 1 (Tesla).
 *
 * "1.23 M"   => 1.23e+6
 * "1.23 kPa" => 1.23e+3  (ok, but not recommended)
 * "1.23 Pa"  => 1.23e+12 (not what's intended!)
 */
std::string engineering_to_exponent( std::string const text )
{
    std::string::size_type pos = text.find( ' ' );

    if ( std::string::npos == pos )
    {
        return text;
    }

    const std::string magnitude = text.substr( 0, pos );
    const std::string prefix    = text.substr( pos + 1, 1 );

    return magnitude + prefix_to_exponent( prefix );
}

} // anonymous namespace

// end of file
