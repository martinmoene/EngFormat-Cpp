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

// Set to 1 for debugging info
#if 0
#define LOG(...) printf(__VA_ARGS__)
#else
#define LOG(...)
#endif

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
    "e0"   , "e3"  , "e6"  , "e9"  ,
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
std::string engineering_to_exponential( std::string text );

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
	bool is_signed = signbit(value);
	const char *sign = is_signed ? "-" : "";
	
	int classify = fpclassify(value);
	if(classify != FP_NORMAL) {
		switch(classify) {
		case FP_SUBNORMAL:
		case FP_ZERO:
		default:
		{
			char *s;
			if(exponential) {
				asprintf(&s, "%s%.*fe0", sign, digits-1, 0.0);
			} else {
				asprintf(&s, "%s%.*f", sign, digits-1, 0.0);
			}
			return s;
		}
		case FP_INFINITE:
			return strdup("INFINITE");
			break;
		case FP_NAN:
			return strdup("NaN");
			break;
		}
	}
	if(is_signed) {
		value = fabs(value);
	}

	if(digits < 3) {
		digits = 3;
	} else
	if(digits > 9) {
		digits = 9;
	}
	LOG("\n\n");

	// correctly round to desired precision
	long expof10 = lrint( floor( log10(value) ) );
	if(expof10 > 0) {
		expof10 = (expof10/3)*3;
		LOG("GT: expof10=%ld\n", expof10);
	} else {
		expof10 = ((-expof10+3)/3)*(-3);
		LOG("LTE: expof10=%ld\n", expof10);
	}

	value *= pow(10.0, -expof10);
	LOG("value=%lf\n", value);

	long lintgr, lfract;
	{
		double intgr, fract;
		
		fract = modf(value, &intgr);
		lintgr = lrint(intgr);
		LOG("compute: %ld . %lf\n", lintgr, fract);
	
		if(lintgr >= 1000) {
			digits -= 3;				// fractional digits
		} else
		if(lintgr >= 100) {
			LOG("lvalue > 100 (%ld)\n", lintgr);
			digits -= 2;				// fractional digits
		} else
		if(lintgr >= 10) {
			LOG("is > 10 (%ld)\n", lintgr);
			digits -= 1;				// fractional digits
		} else
		if(lintgr >= 1) {
			LOG("is > 1 (%ld)\n", lintgr);
		} else {
			LOG("YIKES: < 1\n");
			//assert(!"Impossible to get < 1 unless the fractional part is 1!");
			digits += 1;				// fractional digits
		}
		
		// how much to multiple the fraction to get it to round the one-off value
		double fractMult = pow(10.0, (int)digits - 1);
		long lfractMult = lrint(fractMult);
		LOG("digits=%d fractMult=%lf lfractMult=%ld\n", digits, fractMult, lfractMult);

		// round the fraction to the correct number of places
		fract *= fractMult;
		lfract = lrint(fract);
		LOG("lintgr=%ld lfract=%ld fract=%lf\n", lintgr, lfract, fract);

		// did the rounding the fractional component cause an increase in the integral value?
		if(lfract >= lfractMult) {
			LOG("ROUND\n");
			lfract -= lfractMult;			// remove overflow value

			long nlintgr = lintgr + 1;
			if( (lintgr < 1000 && nlintgr >= 1000) || (lintgr < 100 && nlintgr >= 100) || (lintgr < 10 && nlintgr >= 10) || (lintgr < 1 && nlintgr >= 1) ) {
				LOG("WRAP\n");
				lfract /= 10;
				fractMult /= 10;
				digits -= 1;
			}
			lintgr = nlintgr;				// rounded up, so increase integral part
			LOG("lintgr=%ld lfract=%ld\n", lintgr, lfract);
		}

		if(lintgr >= 1000) {
			LOG(">=1000\n");
			expof10 += 3;
			digits += 3;

			long fullVal = lrint(lintgr*fractMult) + lfract;
			long fullMult = lrintf(1000.0 * fractMult);
			LOG("fullVal=%ld fullMult=%ld\n", fullVal, fullMult);

			lintgr = fullVal / fullMult;
			lfract = fullVal - (lintgr * fullMult);
			LOG("lintgr=%ld lfract=%ld\n", lintgr, lfract);
		}
	}

	--digits;
	const char *decimal_str = digits ? "." : "";

#if 0
	char *result;
	if(exponential || (expof10 < prefix_start) || (expof10 > prefix_end)) {
		LOG("RESULT 1: digits=%d\n", digits);
		asprintf(&result, "%s%ld%s%0.*lde%ld", sign, lintgr, decimal_str, digits, lfract, expof10);
	} else {
		LOG("RESULT 2: digits=%d\n", digits);
		const char *s = prefix[(expof10-prefix_start)/3];
		asprintf(&result, "%s%ld%s%0.*ld%s%s", sign, lintgr, decimal_str, digits, lfract, *s ? " " : "", s);
	}
#endif
    std::ostringstream os;
	os.fill( '0' );
	std::streamsize width = os.width();

    if( exponential || ( expof10 < prefix_start ) || ( expof10 > prefix_end ) )
    {
        os << sign << lintgr << decimal_str;
		if(digits) {
			os.width( digits );
			os << lfract;
			os.width( width );
		}
		os << "e" << expof10 << ( unit.length() ? " " : "" );
        //os << sign << std::fixed << std::setprecision( digits - 1 ) << value << "e" << expof10 << ( unit.length() ? " " : "" );
    }
    else
    {
		const char *s = prefix[( expof10 - prefix_start ) / 3];

        os << sign << lintgr << decimal_str;
		if(digits) {
			os.width( digits );
			os << lfract;
			os.width( width );
		}
        os << (*s ? " " : "") << s;
		
        //os << sign << std::fixed << std::setprecision( digits - 1 ) << value << " " << prefix[( expof10 - prefix_start ) / 3];
    }
    return os.str() + unit;
}

/**
 * convert the output of real2eng() into a double.
 */
double from_engineering_string( std::string const text )
{
    return strtod( engineering_to_exponential( text ).c_str(), NULL );
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
    const long expof10 = lrint( floor( log10( value ) ) );
    const long  power = expof10 + 1 - digits;

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
 * "1.23 M" => "1.23e+006"
 * omits everything after prefix.
 */
std::string engineering_to_exponential( std::string const text )
{
    std::string::size_type pos = text.find( ' ' );

    if ( std::string::npos == pos )
    {
        return text;
    }

    return text.substr( 0, pos ) + prefix2exponent( text.substr( pos + 1, 1 ) );
}

} // anonymous namespace

// end of file
