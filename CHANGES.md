Changes
=======

0.2 &ndash; 12 February 2015
----------------------------
- Add separator parameter to to_engineering_string()
- Failing test: test_eng_format.cpp:115: from_engineering_string() needs adaptation 
  to support empty separator, e.g. "1k"

0.1 &ndash; 1 July 2013
-----------------------
- Start version numbering
- Failing test: test_eng_format.cpp:145: formats as '-1000.000e-27', should be "-1.00000 y"
- Failing test: test_eng_format.cpp:158: formats as '100.0 z', should be "100 z"
