# The LDPL Programming Language
![Stage](https://img.shields.io/badge/build-passing-green.svg) ![Stage](https://img.shields.io/badge/tests-passing-green.svg) ![Version](https://img.shields.io/badge/version-2.2.0-blue.svg)

![LDPL](http://ldpl.lartu.net/ldpl-logo-new.png)

[LDPL](http://ldpl.lartu.net/) is a very simple (but powerful) programming language designed from the ground up to be excessively expressive, readable, fast and easy to learn. It is mostly written in a series of statements that mimics plain English, in the likeness of *[the good parts of]* older languages like COBOL, with the desire that it can be understood by anybody. It's very portable and runs on a plethora of different architectures and operating systems including AMD-64 Linux and Windows, ARMv8 Linux and PowerPC Mac OS X. It even supports UTF-8 out of the box.

This repository contains the source code and releases of the LDPL compiler.
Check http://ldpl.lartu.net/ for more information on the language, tutorials, examples and more!

## About the language

LDPL is standarized [here](http://ldpl.lartu.net/reference).
That's both the language documentation and the compiler writting standard.

## Why should I use LDPL?

Because it looks nice, it's readable, it's fun, it's like a little toy to toy around with! Have you ever found a symbol within code that you didn't know what it did? Maybe an asterisk, a semicolon or even a pair of brackets? We have, too. The average programming language looks like this: 

```
+[-[<<[+[--->]-[<<<]]]>>>-]>-.---.>..>.<<<<-.<+.>>>>>.>.<<.<-.
```

Can you read that? We can't either. That's because regular programming languages weren't designed with readability in mind. **LDPL was**. Compare the example above with this and tell us which one would you rather use: 

```
procedure:
display "Hello World" crlf 
```

That's right. Say bye bye to cryptic glyphs. Say hello to **LDPL**. 

## How to use this compiler

To use the compiler, you must have a C++ compiler already installed on your system and have mapped it to `c++`, found on your `PATH`. The LDPL Compiler compiles LDPL code to C++ code and thus this is a requirement for it to work.

![Compiling Dragon](https://ldpl.lartu.net/reference/ldpl-reference.png)

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

## How can I contribute to LDPL?

You can add statements to the language (that's in fact really easy!), fix bugs, add issues, write examples, write some software in LDPL, etc. Check CONTRIBUTING.md (*to be linked in a minute, please wait!*) for more information. Anything is welcome, very, very welcome!

![Contributing Dragon](https://ldpl.lartu.net/tutorial-ldpl.png)

Contributors are expected to behave by the **Dragon Tamers' Code of Conduct** (*to be linked in a minute, please wait!*).

If you want to talk to the rest of us, you can find the LDPL community at r/LDPL and via IRC on irc.freenode.net, channel #ldpl. Also on Telegram at https://t.me/ldpllang. The IRC and Telegram channels are bridged. You are welcome to open new LDPL channels anywhere else if you like.

## Where can I get more help, if I need it?

You are welcome to submit an issue to this repository, check the [LDPL website](http://ldpl.lartu.net) or join the community channels stated in the previous section. There are a few extra ways to get in touch with us stated on the website.

## License

LDPL is distributed under the GNU General Public License 3.0. The LDPL Dragon logo is released under a Creative Commons Attribution 4.0 International (CC BY 4.0) license.
