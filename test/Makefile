# Copyright (C) 2013 by Martin Moene
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

SOURCES  = test_eng_format.cpp ../src/eng_format.cpp
HEADERS  = ../src/eng_format.hpp

OBJECTS  = $(patsubst %.cpp, %.o, $(SOURCES))

CXXFLAGS = -Wall -Wextra -std=c++11 -Wno-missing-braces -DENG_FORMAT_MICRO_GLYPH=\"u\" -I../src

test_eng_format: $(OBJECTS) $(HEADERS)
	$(CXX) -o $@ $^

clean:
	rm -f $(OBJECTS) test_eng_format

check: test_eng_format
	./test_eng_format

.PHONY: test_eng_format clean check

# end of file
