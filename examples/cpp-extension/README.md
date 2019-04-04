# LDPL C++ Extension Hello World

This branch contains experimental support for extensions to LDPL written in the C++ programming language. Only Mac & Linux are supported at this time. 

## Quickly

If you want to skip all the words, cd into this directory (`examples/cpp-extension`) and run this make command: 

    make test

That will try to build `ldpl` with support for C++ extensions, then build and run the example extension in `examples/cpp-extension`. 

## Less Quickly 

LDPL Extensions consist of two files: a header (.h) and a static archive (.a). The header file should define any LDPL variables used by your extension as well as any LDPL subprocedures defined by your extension. The archive should contain your subprocedures' code.

Here's an example header file to define two subprocedures: one that just prints "hello world" and one that greets you by name. The `myname` variable can be accessed by LDPL programs.

## Code 

hello.h

    #include <string>

    extern string VAR_MYNAME;
    void SUBPR_HELLO();
    void SUBPR_GREET();

hello.cpp

    #include <iostream>
    #include "hello.h"

    void SUBPR_HELLO()
    {
        std::cout << "Heyo, worldy!" << std::endl;
    }

    void SUBPR_GREET()
    {
        std::cout << "Salutations, " << VAR_MYNAME << "!" << std::endl;
    }

hello.lsc

    PROCEDURE:

    CALL HELLO
    DISPLAY "What's your name, friend? "
    ACCEPT MYNAME
    CALL GREET

## Building it 

We don't need to write the LDPL file at the same time as the extension, of course - that's how we use the extension. The extension itself will probably be some library wrapping some other library to add superpowers to our programs.

To turn the above example into a usable extension, we compile the C++ into object files and then turn those object files into an archive. In our case, we only have one `.o` file:

    c++ -c hello.cpp -std=gnu++11 -o hello.o
	ar -cvq hello.a hello.o

Once we've got our archive, we compile our LDPL code in the same directory as the extension's header and archive files:

    ldpl -i=hello.h hello.lsc

If it compiles, you should have a `hello-bin`:

    ./hello-bin

The `.h` and `.a` files need to have the same name, be in the same directory, and be compiled for the correct architecture. 

## Name Mangling

Variable and subprocedure names must currently be defined using LDPL's internal mangled representation. This is because there is no translation layer between the extension and your compiled LDPL code. It doesn't make a big difference in the above example, but it means - becomes r_U and . becomes r_V, etc, creating function names like SUBPR_RLr_VACCEPT. See https://git.io/fjLnV for an example of using macros to help deal with this in your extension's code.
