/*
Copy and Paste the following into your terminal of choice.
Must be in the "test/" directory.

- Compile and Run
g++ -std=c++2a -Wall -Werror -o Yars *.cpp && ./Yars

// Does not compile with clang++
// These errors will be fixed over time
// clang++ -std=c++2a -Wall Werror -o Yars *.cpp && ./Yars

- Code Coverage
g++ -fprofile-arcs -ftest-coverage -fPIC -O0 -std=c++2a -Wall -Werror -o Yars *.cpp && ./Yars
gcovr -j 8 -r .. --html --html-details -o Yars-Code-Coverage.html
*/

#define ZAKERO_YARS_IMPLEMENTATION
#define	ZAKERO_YARS_ENABLE_DEBUG
#define	ZAKERO_YARS_ENABLE_ID_SHUFFLE
#define	ZAKERO_YARS_ENABLE_SAFE_MODE
#define	ZAKERO_YARS_ENABLE_EXPERIMENTAL
//#define ZAKERO_YARS_ENABLE_TO_STRING
#include "../src/Zakero_Yars.h"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

CATCH_REGISTER_TAG_ALIAS("[@attr]", "[attribute]");
CATCH_REGISTER_TAG_ALIAS("[@char]", "[character]");
