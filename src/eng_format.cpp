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
#include <sstream>

#include <math.h>
#include <stdlib.h>

/*
 * Note: using fabs() and other math functions in global namespace for
 * best compiler coverage.
 */

/*
 * Note: micro, µ, may not work everywhere, so you can define a glyph yourself:
 */
#ifndef ENG_FORMAT_MICRO_GLYPH
# define ENG_FORMAT_MICRO_GLYPH "µ"
#endif

/*
 * Note: if not using signed at the computation of prefix_end below,
 * VC2010 -Wall issues a warning about unsigned and addition overflow.
 * Hence the cast to signed int here.
 */
#define ENG_FORMAT_DIMENSION_OF(a) ( static_cast<int>( sizeof(a) / sizeof(0[a]) ) )

eng_prefixed_t eng_prefixed;
eng_exponential_t eng_exponential;

/*
 * Smallest power of ten for which there is a prefix defined.
 * If the set of prefixes will be extended, change this constant
 * and update the table "prefix".
 */

namespace
{

char const * prefix[] =
{
    "y", "z", "a", "f",
    "p", "n", ENG_FORMAT_MICRO_GLYPH, "m",
    "", "k", "M", "G",
    "T", "P", "E", "Z",
    "Y",
};

char const * exponent[] =
{
    "e-24", "e-21", "e-18", "e-15",
    "e-12", "e-9" , "e-6" , "e-3" ,
    ""   , "e3"  , "e6"  , "e9"  ,
    "e12" , "e15" , "e18" , "e21" ,
    "e24" ,
};

const int prefix_count = ENG_FORMAT_DIMENSION_OF( prefix );
const int prefix_start = -24;
const int prefix_end   = prefix_start + 3 * ( prefix_count - 1 );

#if __cplusplus >= 201103L       // C++11
static_assert( ENG_FORMAT_DIMENSION_OF( prefix ) == ENG_FORMAT_DIMENSION_OF( exponent ), "table sizes must match" );
#endif

/*
 * engineering to exponent notation conversion.
 */
std::string eng2exp( std::string text );

} // anonymous namespace

#ifdef _MSC_VER

namespace
{

#if 1
template <typename T>
long lrint( T const x )
{
    return static_cast<long>( x );
}
#else
__inline long int
lrint ( double flt )
{
    int intgr ;

    _asm
    {
        fld flt
        fistp intgr
    } ;

    return intgr ;
}
#endif
} // anonymous namespace

#endif

/**
 * convert real number to prefixed or exponential notation, optionally followed by a unit.
 */
std::string
to_engineering_string( double value, int digits, bool const exponential, std::string const unit /*= ""*/ )
{
    if ( digits < 3 )
    {
        digits = 3;
    }

    char const * sign;

    if ( value < 0.0 )
    {
        sign = "-";
        value = fabs( value );
    }
    else
    {
        sign = "";
    }

    // correctly round to desired precision
    int expof10 = lrint( floor( log10( value ) ) );

    const int power = digits - 1 - expof10;
    value *= pow( 10.0, power );

    double display;
    const double fract = modf( value, &display );

    if ( fract >= 0.5 )
    {
        display += 1.0;
    }

    value = display * pow( 10.0, -power );  //  expof10 - digits + 1

    if ( expof10 > 0 )
    {
        expof10 = ( expof10 / 3 ) * 3;
    }
    else
    {
        expof10 = ( ( -expof10 + 3 ) / 3 ) * ( -3 );
    }

    value *= pow( 10.0, -expof10 );

    if ( value >= 1000.0 )
    {
        value /= 1000.0;
        expof10 += 3;
    }
    else
    {
        if( value >= 100.0 )
        {
            digits -= 2;
        }
        else
        {
            if( value >= 10.0 )
            {
                digits -= 1;
            }
        }
    }

    std::ostringstream os;

    if( exponential || ( expof10 < prefix_start ) || ( expof10 > prefix_end ) )
    {
        os << sign << std::fixed << std::setprecision( digits - 1 ) << value << "e" << expof10 << ( unit.length() ? " " : "" );
    }
    else
    {
        os << sign << std::fixed << std::setprecision( digits - 1 ) << value << " " << prefix[( expof10 - prefix_start ) / 3];
    }
    return os.str() + unit;
}

/**
 * convert the output of real2eng() into a double.
 */
double from_engineering_string( std::string const text )
{
    return strtod( eng2exp( text ).c_str(), NULL );
}

/**
 * step a value by the smallest possible increment.
 */
std::string step_engineering_string( std::string const text, int digits, bool const exponential, bool const positive )
{
    const double value = from_engineering_string( text );

    if( digits < 3 )
    {
        digits = 3;
    }

    // correctly round to desired precision
    const int expof10 = lrint( floor( log10( value ) ) );
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
std::string prefix2exponent( std::string const pfx )
{
    for( int i = 0; i < prefix_count; ++i )
    {
        if ( pfx == prefix[i] )
        {
            return exponent[i];
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
std::string eng2exp( std::string const text )
{
    std::string::size_type pos = text.find( ' ' );

    if ( std::string::npos == pos )
    {
        return text;
    }

    const std::string magnitude  = text.substr( 0, pos );
    const std::string prefix     = text.substr( pos + 1, 1 );

    return magnitude + prefix2exponent( prefix );
}

} // anonymous namespace

// end of file
