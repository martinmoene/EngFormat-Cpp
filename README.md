EngFormat-Cpp
=============

C++ based Engineering Notation Formatter

The C++ engineering formatter presented here is partly based on [code found on stackoverflow](http://stackoverflow.com/a/15734251/437272) and partly based on the code by Jukka Korpela and David Hoerl as available from the project [EngineeringNotationFormatter](https://github.com/dhoerl/EngineeringNotationFormatter) [1] on GitHub.

Example usage
-------------
```Cpp
#include "eng_format.hpp"
#include <iostream>

int main()
{
    std::cout << to_engineering_string( 1234, 3, eng_prefixed, "Pa") << std::endl;
}
```

Compile and run:
```
prompt>g++ -Wall -Wextra -I../src -o example1.exe example1.cpp ../src/eng_format.cpp && example1
1.23 kPa
```

Basic C++ interface
-------------------
```Cpp
/**
 * convert a double to the specified number of digits in SI (prefix) or
 * exponential notation, optionally followed by a unit.
 */
std::string
to_engineering_string( double value, int digits, bool exponential, std::string unit = "" );

/**
 * convert the output of to_engineering_string() into a double.
 */
double
from_engineering_string( std::string text );

/**
 * step a value by the smallest possible increment.
 */
std::string
step_engineering_string( std::string text, int digits, bool exponential, bool increment );
```

Expanded C++ interface
----------------------

```Cpp
extern struct eng_prefixed_t {} eng_prefixed;
extern struct eng_exponential_t {} eng_exponential;

const bool eng_increment = true;
const bool eng_decrement = false;

/**
 * convert a double to the specified number of digits in SI (prefix) notation,
 * optionally followed by a unit.
 */
std::string
to_engineering_string( double value, int digits, eng_prefixed_t, std::string unit = "" );

/**
 * convert a double to the specified number of digits in exponential notation,
 * optionally followed by a unit.
 */
std::string
to_engineering_string( double value, int digits, eng_exponential_t, std::string unit = "" );

/**
 * step a value by the smallest possible increment, using SI notation.
 */
std::string
step_engineering_string( std::string text, int digits, eng_prefixed_t, bool increment );

/**
 * step a value by the smallest possible increment, using exponential notation.
 */
std::string
step_engineering_string( std::string text, int digits, eng_exponential_t, bool increment );
```

Notes and References
--------------------

[1] David Hoerl. [EngineeringNotationFormatter](https://github.com/dhoerl/EngineeringNotationFormatter). iOS Project demoing C-based Engineering Notation Formatter.

[2] Mark Harris. C/C++ Programming Tips, [Fast Float -> Int casts](http://wwwx.cs.unc.edu/~sud/tips/Programming_Tips.html)
