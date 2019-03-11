![LDPL](http://ldpl.lartu.net/ldpl-logo-new.png)

[LDPL](http://ldpl.lartu.net/) is a very simple programming language designed from the ground up to be excessively expressive, readable and easy to learn. It is mostly written in a series of statements that mimics plain English, in the likeness of *[the good parts of]* older languages like COBOL, with the desire that it can be understood by anybody. It's very portable and runs on a plethora of different architectures and operating systems including AMD-64 Linux and Windows, ARMv8 Linux and PowerPC Mac OS X. It even supports UTF-8 out of the box.

This repository contains the source code and releases of the LDPL interpreter.
Check http://ldpl.lartu.net/ for more information on the language, tutorials, examples and more!

## About the language

LDPL is standarized [here](http://ldpl.lartu.net/reference).
That's both the language documentation and the compiler writting standard.

## How to use this interpreter

First, download the latest release.
Then `make` LDPL in the `src` folder. LDPL requires only C++11 to compile.
Then write some LDPL source code, say `source.lsc`.
Then compile and run said source code using `ldpl source.lsc`.
Done! For more info on the interpreter run `ldpl -h`.
Example code can be found on [the LDPL website](http://ldpl.lartu.net).

## Interpreter options

 * You can import files to your LDPL compilation by using the `-i` flag. For example, say you have a library `mylib.ldpl` and a source file `mysource.ldpl`, you can compile both and include the library by running `ldpl -i=mylibrary.ldpl mysource.ldpl`. Multiple `-i=` can be used to import multiple files.
 * By using `-r` you can just compile the project and print LDPL's internal representation for that code.
 * `-v` and `--version` print out version info and release details.
 * `-h` and `--help` print this list of options.

## License

LDPL is distributed under the GNU General Public License 3.0.
