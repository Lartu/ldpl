# The LDPL Programming Language
![Stage](https://img.shields.io/badge/build-passing-green.svg) ![Stage](https://img.shields.io/badge/tests-passing-green.svg) ![Version](https://img.shields.io/badge/version-2.2.0-blue.svg)

![LDPL](http://ldpl.lartu.net/ldpl-logo-new.png)

[LDPL](http://ldpl.lartu.net/) is a very simple (but powerful) programming language designed from the ground up to be excessively expressive, readable, fast and easy to learn. It is mostly written in a series of statements that mimics plain English, in the likeness of *[the good parts of]* older languages like COBOL, with the desire that it can be understood by anybody. It's very portable and runs on a plethora of different architectures and operating systems including AMD-64 Linux and Windows, ARMv8 Linux and PowerPC Mac OS X. It even supports UTF-8 out of the box.

This repository contains the source code and releases of the LDPL compiler.
Check http://ldpl.lartu.net/ for more information on the language, tutorials, examples and more!

## About the language

LDPL is standarized [here](http://ldpl.lartu.net/reference).
That's both the language documentation and the compiler writting standard.

## How to use this compiler

To use the compiler, you must have a C++ compiler already installed on your system and have mapped it to `c++`, found on your `PATH`. The LDPL Compiler compiles LDPL code to C++ code and thus this is a requirement for it to work.

**If you want to download a compiled binary:** just go to the *Releases* part of this repository and download the newest one available.

**If you want to build LDPL yourself:** First, download the latest release.
Then `make` LDPL in the `src` folder. LDPL requires only C++11 to compile.
Then write some LDPL source code, say `source.lsc`.
Then compile ource code using `ldpl source.lsc`. The compiled, executable binary file will be saved as `ldpl.out`.
Done! For more info on the compiler run `ldpl -h`.
Example code can be found on [the LDPL website](http://ldpl.lartu.net).

**Note for Windows users:** compilation under Windows has been tested with [MinGW](http://www.mingw.org/wiki/Getting_Started) as installed on that guide. MinGW-w64 seems to have some problems compiling LDPL.

## Compiler options

 * You can import files to your LDPL compilation by using the `-i` flag. For example, say you have a library `mylib.ldpl` and a source file `mysource.ldpl`, you can compile both and include the library by running `ldpl -i=mylibrary.ldpl mysource.ldpl`. Multiple `-i=` can be used to import multiple files.
 * By using `-r` you can just compile the project and print the C++ representation for that code.
 * You can set the output file for the compiled binary with the `-o` flag. For example, if you want to name your program "dog", you could compile it with `ldpl -o=dog main.ldpl`.
 * `-v` and `--version` print out version info and release details.
 * `-h` and `--help` print this list of options.

## License

LDPL is distributed under the GNU General Public License 3.0. The LDPL Dragon logo is released under a Creative Commons Attribution 4.0 International (CC BY 4.0) license.
