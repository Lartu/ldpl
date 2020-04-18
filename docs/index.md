# LDPL Docs

## Introduction

The **LDPL Community** has compiled this document with the desire to teach
and standardize the **LDPL** programming language. This document contains the
specification for the **LDPL** programming language, as well as explanations,
instructions and examples for every feature included in it.

![LDPL](https://raw.githubusercontent.com/Lartu/ldpl/master/images/reference-logo.png)

Feedback, corrections and suggestions are welcome, both on the main 
[LDPL repository](https://github.com/Lartu/ldpl)
or by e-mail to [martin@ldpl-lang.org](mailto:martin@ldpl-lang.org).
You can join the LDPL community at [r/ldpl](https://reddit.com/r/ldpl)
or via IRC on irc.freenode.net channel #ldpl.

The source code for this documentation is available in the LDPL repository.

## About LDPL

**LDPL** is a powerful compiled programming language designed from the
ground up to be expressive, readable, fast and easy to learn. It mimics plain
English, in the likeness of the good parts of older programming languages like
COBOL, with the desire that it can be understood by anybody. LDPL was designed
to run on Unix systems, including AMD-64 Linux, macOS, ARMv8 Linux, Android
Termux and both Intel and PowerPC OS X (tested from Tiger 10.4 onwards).
It even supports UTF-8 out of the box.
    
    :::coffeescript
    # LDPL 'Hello World' example
    
    data:
    name is text # Your name will go here.
    
    procedure:
    display "Hello World!" lf "What's your name? "
    accept name
    display "你好, " name ", welcome to LDPL!" lf

LDPL also aims to suppress unreadable code and redundancy by only having one
way of doing anything. What a command does should never overlap with the
purpose of another command and, as such, every LDPL command does one and only
one thing. Every line is a step further towards the completion of an algorithm,
no more, no less.

## The LDPL Compiler

To use LDPL, you should first download or compile the LDPL compiler.
For more information on how to do that, read the
[*How to install LDPL* section](https://github.com/Lartu/ldpl#-how-to-install-ldpl)
on the LDPL Readme.

To use the compiler, you must have a C++ compiler already installed on your
system and have mapped it to `c++`, found on your PATH. The LDPL Compiler
compiles LDPL code to C++ code and thus this is a requirement for it to work.

Once the compiler is set up, go write some LDPL source code, say `source.ldpl`.
Then compile the source code using `ldpl source.ldpl`. The compiled, executable
binary file will be saved as `source-bin`. Done!

### Compiler Switches

To use the LDPL compiler, the syntax is as follows:

    ldpl [-i='<included file>']... <source file>|-c
         [-o='<output name>'|-r] [-f='<c++ flag>']... [-n]
    ldpl [-v|-h]

The `-f` flag can be used to pass extra options to the `c++` compiler when. For
example, `-f=-lSDL` could be used to link against SDL. The `flag` statement can
also be used as well (`flag "-lSDL"`) and its use is recommended. More on this
later.

By using `-r` you can just compile your project and print the C++ representation
for that code.

You can set the filename for the compiled binary with the `-o` flag. For
example, if you want to name your program "dog", you could compile it with
`ldpl -o=dog main.ldpl`.

On Linux platforms, LDPL builds static binaries by default. If you want to
build non-static ones use the `-ns` flag.

The `-c` flag tells LDPL to accept source code from the standard input.

 
You can import extra files and extensions to your LDPL compilation by using the
`-i=` flag. Extensions can be imported by passing `.o`, `.a`, or `.cpp` files
to this flag; see the Extensions section for more information. The use of the
`INCLUDE` statement is preferred.

`-v` and `--version` print out version info and release details.

`-h` and `--help` print this list of options.

## File Extensions

The preferred file extension for LDPL source files is **'.ldpl'**.
The extension **'.lsc'** (LDPL Source File) should also be accepted in case
the preferred extension couldn't be used for any reason.

!!! tip
    File extensions are important: they help editors to recognize what language
    your source code is written in and they tell the LDPL compiler how to treat
    your files.
    
## License

This LDPL Compiler is distributed under the Apache 2.0 License.
All LDPL Dinosaur logos were created by [Lartu](https://lartu.net) and are
released under a Creative Commons Attribution 4.0 International (CC BY 4.0)
license. This documentation is released under the Apache 2.0 License.


