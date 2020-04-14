Welcome To The Y.A.R.S. Developer Resource
==========================================

_This project is still in heavy development. Many parts of this project are 
still subject to complete rewrites and drastic API changes.  What is present 
is working and usable, if you do want to start experimenting with this 
library and its concepts._

## Overview

Y.A.R.S., Yet Another RPG System, is a _skill-based game system_.  While it 
is possible to use Y.A.R.S. for table-top pen-and-paper games, the math 
formulas and data tracking are tailored more towards a computer 
implementation.

* Y.A.R.S. is a "Single Header Library" inspired by [STB 
repository](https://github.com/nothings/stb). The benefit of a Single Header 
Library is that all you need to do is include the header file and you have 
access to the entire library, nothing else to add or link. While most Single 
Header Libraries are a collection of C functions, Y.A.R.S. is a single class 
with a small handful of helper methods in the global namespace.

<hr>

## Why Y.A.R.S.?

Probably the best reason to use Y.A.R.S. instead of any of the other popular 
RPG systems is that you don't have to worry about any Prestige Lawyers of the 
Coast coming after you.  There is also the complete lack of licensing fees.

Looking for a more technical reason? Y.A.R.S. is completely data-driven.
There are no predefined classes, races, attributes, or skill sets.  This 
means Y.A.R.S. can be used for everything from Cyberpunk to Fantasy, Modern 
to Sci-Fi.  The only thing that needs to be done is create a set of Skills 
and Items, plus the Characters to use them,  Then tell Y.A.R.S. who does what 
and to whom, and Y.A.R.S. will work out the rest.

<hr>

## How To Integrate Into Your Project

Using Y.A.R.S. in your project is done in two parts: Implementation and 
Interface.  Here you will see an overview of how to integrate into your 
projects. For more details and options refer to the 
[documentation](html/index.html).

Also, Y.A.R.S. requires a compiler that supports C++20.
<br><br>


### Implementation

Some where in your project, you will need to have a source code file that 
will contain the Y.A.R.S. implementation.  In this file, before including the 
Y.A.R.S. header file, add the define `ZAKERO_YARS_IMPLEMENTATION`.  For
example:

    #define ZAKERO_YARS_IMPLEMENTATION
    #include "path/to/Zakero_Yars.h"

The Y.A.R.S. implementation should only be done in a single location.  If 
done more than once, the linker will probably complain about multiple 
definitions of the Yars object.
<br><br>


### Interface

To use the Yars object in your project, only the Zakero_Yars.h file needs to be 
included.  No other Y.A.R.S. defines are require.

    #include "path/to/Zakero_Yars.h"

<hr>

## How to contribute

Firstly, if you are not viewing this from the main website, 
[yars.zakero.com](https://yars.zakero.com), then you are at a site where the 
code is mirrored.  Patches are welcome and commit access to the official repo 
may be granted.

To work with the main repo, you will need to use 
[fossil](https://fossil-scm.org). Fossil is much easier to use than git and is
more capable. Try it, you may like it.

Fossil: Clone

    fossil clone https://yars.zakero.com Zakero_Yars.fossil

Fossil: Open the repo

    mkdir Zakero_Yars
    cd Zakero_Yars
    fossil open /path/to/Zakero_Yars.fossil
<br>

### License

Any and all code contributions will be placed under the current license.  See 
the [License](LICENSE.txt) file for details.
<br><br>


### Directory Structure

The Directory structure for Y.A.R.S. is very simple.

- `html/` All of the HTML documentation
  <br><br>
- `src/` The location of the Zakero_Yars.h file
  <br><br>
- `test/` All of the test programs are located here
<br><br>


### Code Style

At this time there are no tool configurations to standardize on.  Until a 
code-style has been defined, do your best to follow the existing style.

Currently evaluating:

- __clang-format__:
  Not working. Does not support indenting after access modifiers in class 
  definitions. Some line wrapping does not work as desired but might be fixable 
  by adjusting the penalty values.

- __uncrustify__:
  Not enough documentation to explain all of the options.  Experimenting was 
  painfully slow, to the point of giving up.
<br><br>


### Unit Tests

All tests can be found in the `test/` directory.  These tests use the 
[Catch2](https://github.com/catchorg/Catch2) testing framework.  All of the 
tests are stand-alone and can be compiled and run individually.
`test/Zakero_Yars.cpp` is the exception, this test is designed to the 
_top-level_ test that incorporates all the other tests.

There is no CMake/CTest support at the moment but instructions for compiling 
the tests are located at the top of the each of the files.
