# The LDPL Programming Language
![Stage](https://img.shields.io/badge/build-passing-green.svg)
![Stage](https://img.shields.io/badge/tests-passing-green.svg)
![Version](https://img.shields.io/badge/current_release-3.0.3_'Active_Argentinosaurus'-blue.svg)
![Version](https://img.shields.io/badge/development_version-3.0.4-red.svg)

![LDPL](https://www.ldpl-lang.org/ldpl-logo-new.png)

[LDPL](https://www.ldpl-lang.org/) is a very simple (but powerful) programming language designed from the ground up to be excessively expressive, readable, fast and easy to learn. It is mostly written in a series of statements that mimics plain English, in the likeness of *[the good parts of]* older languages like COBOL, with the desire that it can be understood by anybody. It's very portable and runs on a plethora of different architectures and operating systems including AMD-64 Linux and Windows, ARMv8 Linux, Android and both Intel and PowerPC Mac OS X (*tested from Tiger 10.4 onwards*). It even supports UTF-8 out of the box.

This repository contains the source code and releases of the LDPL compiler.
Check https://www.ldpl-lang.org/ for more information on the language, tutorials, examples and more!

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

## Getting started with LDPL

You should first download or compile the LDPL compiler:

 - **If you want to download a compiled binary:** just go to the *Releases* part of this repository and download the newest one available.

 - **If you want to build LDPL yourself:** first, clone this repository.
Then `make` LDPL in the `src` folder. LDPL requires only C++11 to compile.

**Note for Windows users:** compilation under Windows has been tested with [MinGW](http://www.mingw.org/wiki/Getting_Started) as installed on that guide. MinGW-w64 seems to have some problems compiling LDPL.

LDPL is standarized and documented [here](https://ldpl.gitbook.io). Check that to learn yourself some LDPL (*for the greater good*)!

Examples are available in the [Examples](/examples) subfolder.

## LDPL compatible editors

![VSC](https://www.ldpl-lang.org/vsc.png) ![VIM](https://www.ldpl-lang.org/vim.png) ![ATOM](https://www.ldpl-lang.org/atom.png)

We have highlighting and code formatting extensions available for [vim](https://github.com/araml/ldpl.vim) (thanks to [Araml](https://github.com/araml)), [Visual Studio Code](https://marketplace.visualstudio.com/items?itemName=dvkt.vscode-ldpl) (thanks to [ʇʞʌp](https://github.com/dvkt)) and [Atom](https://atom.io/packages/language-ldpl) (thanks to [Damián Garro](https://github.com/dgarroDC)).

You can use any other editor (without syntax highlighting) if you like. If know how to write syntax packages for other editors and would like to contribute to the LDPL project, a highlighting extension would be a very welcome contribution.

## How to use this compiler

To use the compiler, you must have a C++ compiler already installed on your system and have mapped it to `c++`, found on your `PATH`. The LDPL Compiler compiles LDPL code to C++ code and thus this is a requirement for it to work.

![Compiling Dragon](https://www.ldpl-lang.org/reference/ldpl-reference.png)

Once the compiler is set up, go write some LDPL source code, say `source.lsc`.
Then compile the source code using `ldpl source.lsc`. The compiled, executable binary file will be saved as `source-bin`.
Done! For more info on the compiler run `ldpl -h`.
Example code can be found on [the LDPL website](https://www.ldpl-lang.org).

## Compiler options

 * You can import files to your LDPL compilation by using the `-i` flag. For example, say you have a library `mylib.ldpl` and a source file `mysource.ldpl`, you can compile both and include the library by running `ldpl -i=mylibrary.ldpl mysource.ldpl`. Multiple `-i=` can be used to import multiple files. Extensions can be imported by passing `.o`, `.a`, or `.cpp` files to this flag; see the Extensions section below.
 * The `-f` flag can be used to pass extra options to the compiler when building extensions. For example, `-f=-lSDL` could be used to link against SDL.
 * By using `-r` you can just compile the project and print the C++ representation for that code.
 * You can set the output file for the compiled binary with the `-o` flag. For example, if you want to name your program "dog", you could compile it with `ldpl -o=dog main.ldpl`.
 * On Linux and Windows platforms, LDPL builds static binaries by default. If you want to build non-static ones use the `-ns` flag.
 * `-v` and `--version` print out version info and release details.
 * `-h` and `--help` print this list of options.

## C++ extensions

LDPL supports extensions written in C++. Extensions are `.cpp`, `.o`, or `.a` files that can be imported into your program using the `-i=` compiler flag. 

For a guide to writing and building extensions, see [LDPL's official documentation](https://ldpl.gitbook.io/reference/extensions/c++-extensions). For a simple example, see the [examples/cpp-extension](./examples/cpp-extension) directory.

## How can I contribute to LDPL?

You can add statements to the language (that's in fact really easy!), fix bugs, add issues, write examples, write some software in LDPL, etc. Check out the [contribution guide](/CONTRIBUTING.md) for more information. Anything is very welcome! Even telling your friends about LDPL is a very easy and very useful contribution!

![Contributing Dragon](https://www.ldpl-lang.org/tutorial-ldpl.png)

Contributors are expected to behave by the [Dragon Tamers Code](/CODE_OF_CONDUCT.md).

If you want to talk to the rest of us, you can find the LDPL community at [r/LDPL](https://reddit.com/r/LDPL) and via IRC on [irc.freenode.net](http://irc.freenode.net/), channel #ldpl. Also on Telegram via the [ldpllang](https://t.me/ldpllang) group. The IRC and Telegram channels are bridged. You are welcome to open new LDPL channels anywhere else if you like.

## Where can I get more help, if I need it?

You are welcome to submit an issue to this repository, check the [LDPL website](https://www.ldpl-lang.org) or join the community channels stated in the previous section. There are a few extra ways to get in touch with us stated on the website.

## Is it a dinosaur or a dragon?

Both I guess.

## License

LDPL is distributed under the GNU General Public License 3.0. The LDPL Dragon logo is released under a Creative Commons Attribution 4.0 International (CC BY 4.0) license.
