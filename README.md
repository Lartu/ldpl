# The LDPL Programming Language

![Version](https://img.shields.io/badge/current_release-3.0.5_'Creative_Carnotaurus'-blue.svg)
![Version](https://img.shields.io/badge/development_version-4.0_'Diligent_Dreadnoughtus'-red.svg)
[![Build Status](https://travis-ci.org/Lartu/ldpl.svg?branch=master)](https://travis-ci.org/Lartu/ldpl)

![LDPL](https://github.com/Lartu/ldpl/blob/master/images/ldpl-logo.png)

[LDPL](https://www.ldpl-lang.org/) is a powerful compiled programming language designed from the ground up to be excessively **expressive**, **readable**, **fast** and **easy** to learn. It mimics plain English, in the likeness of *the good parts of* older programming languages like COBOL, with the desire that it can be understood by anybody. It's very portable and runs on a plethora of different architectures and operating systems including AMD-64 Linux and macOS, ARMv8 Linux, Android and both Intel and PowerPC OS X (*tested from Tiger 10.4 onwards*). It even supports UTF-8 out of the box.
Please note that Windows support has been dropped as of LDPL 4.0 'Diligent Dreadnoughtus'.

This repository contains the source code and releases of the LDPL compiler.
Check https://www.ldpl-lang.org/ for more information on the language, tutorials, examples and more!

### Example LDPL code

```coffeescript
data: 
  name is text     # Your name goes here! 

procedure: 
  # Simple Hello World
  display "Hello World" crlf "What's your name? " 
  accept name 
  display "你好, " name "! Welcome to LDPL!" crlf
```

This greets the user, ask them for their name and then prints *你好, &lt;name&gt;!* followed by a line-break. Easy as pie. Super legible. Check the [official website](https://www.ldpl-lang.org/) and the [examples](/examples) subfolder for more examples, including a Brainf*ck interpreter and Bellman-Ford's Algorithm!

## Installing LDPL

You can get yourself a precompiled LDPL binary or compile LDPL from source. Check the following table if you are not sure of what to do.

| **If you want to download a compiled binary** | **If you want to build LDPL yourself** |
| ---                                           | --- |
| Compiled binaries contain the latest stable release. Might not be up to date with the newest, bleeding edge features, but will work fine on most systems and are already compiled for you. | Building LDPL yourself gives you access to the latest features of the language. Development LDPL code tends to be stable, compile and work fine, but hidden bugs may still lurk in there! |
| ![Current Release Logo](https://github.com/Lartu/ldpl/blob/master/images/release-logos/creative-carnotaurus.png) | ![Current Development Version Logo](https://raw.githubusercontent.com/Lartu/ldpl/master/images/release-logos/diligent-dreadnoughtus-dev.png) |
| **How to:** [download the latest stable release available](https://github.com/Lartu/ldpl/releases). You should then move the binary to a folder on your PATH. | **How to:** first, clone this repository. Then `make` and `make install` LDPL in the `src` folder. This will install LDPL and the LDPL documentation (`man ldpl`) on your system. LDPL requires only C++11 to compile. |

**Note for Windows users:** compilation under Windows has been tested with [MinGW](http://www.mingw.org/wiki/Getting_Started) as installed on that guide. MinGW-w64 seems to have some problems compiling LDPL.
Also, it is advised to configure git autoclrf to use Unix-style end-of-line: `git config core.autocrlf input`. LDPL relies on an AWK script which will not work properly with DOS-style newlines (CLRF).

Once you have LDPL installed, check the LDPL reference to learn how to use the language. Information on how to compile LDPL scripts and a list of LDPL compatible editors is provided below.

## LDPL compatible editors

![VSC](https://www.ldpl-lang.org/images/vsc.png?v=1) ![VIM](https://www.ldpl-lang.org/images/vim.png) ![ATOM](https://www.ldpl-lang.org/images/atom.png) ![nano](https://www.ldpl-lang.org/images/nanologo.png)

We have highlighting and code formatting extensions available for [vim](https://github.com/araml/ldpl.vim) (thanks to [Araml](https://github.com/araml)), [Visual Studio Code](https://marketplace.visualstudio.com/items?itemName=dvkt.vscode-ldpl) (thanks to [ʇʞʌp](https://github.com/dvkt)), [Atom](https://atom.io/packages/language-ldpl) (thanks to [Damián Garro](https://github.com/dgarroDC)) and [nano](https://github.com/Lartu/ldpl/tree/master/highlighting/nano) (by [Lartu](http://github.com/lartu)).

You can use any other editor (without syntax highlighting) if you like. If know how to write syntax packages for other editors and would like to contribute to the LDPL project, a highlighting extension would be a very welcome contribution.

## LDPL documentation & reference

![Learning Dinosaur](https://github.com/Lartu/ldpl/blob/master/images/reference-logo.png)

[The **LDPL documentation** is available here](https://docs.ldpl-lang.org). Check that to learn yourself some LDPL (*for the greater good*)! The documentation is also uploaded to the [ldpl-docs](https://github.com/lartu/ldpl-docs) GitHub repository and can be read and forked from there.

The LDPL documentation can also be found on your system using `man ldpl` when you install LDPL by using `make install`. The man page is also
available in the [man](/man) folder of this repository.

## How to use this compiler

To use the compiler, you must have a C++ compiler already installed on your system and have mapped it to `c++`, found on your `PATH`. The LDPL Compiler compiles LDPL code to C++ code and thus this is a requirement for it to work.

Once the compiler is set up, go write some LDPL source code, say `source.lsc`.
Then compile the source code using `ldpl source.lsc`. The compiled, executable binary file will be saved as `source-bin`.
Done! For more info on the compiler run `ldpl -h`.

### Compiler options

 * The `-f` flag can be used to pass extra options to the compiler when building extensions. For example, `-f=-lSDL` could be used to link against SDL.
 * By using `-r` you can just compile the project and print the C++ representation for that code.
 * You can set the output file for the compiled binary with the `-o` flag. For example, if you want to name your program "dog", you could compile it with `ldpl -o=dog main.ldpl`.
 * On Linux and Windows platforms, LDPL builds static binaries by default. If you want to build non-static ones use the `-ns` flag.
 * The `-c` flag tells LDPL to accept source code from the standard input. 
 * You can import extensions to your LDPL compilation by using the `-i=` flag. Extensions can be imported by passing `.o`, `.a`, or `.cpp` files to this flag; see the Extensions section below.
 * `-v` and `--version` print out version info and release details.
 * `-h` and `--help` print this list of options.

### C++ extensions

LDPL supports extensions written in C++. Extensions are `.cpp`, `.o`, or `.a` files that can be imported into your program using the `-i=` compiler flag. For example `ldpl -i=lib.cpp source.ldpl`.

For a guide to writing and building extensions, see [LDPL's official documentation](https://docs.ldpl-lang.org/extensions/c++-extensions). For a simple example, see the [examples/cpp-extension](./examples/cpp-extension) directory.

## How can I contribute to LDPL?

You can add statements to the language (that's in fact really easy!), fix bugs, add issues, write examples, write some software in LDPL, etc. Check out the [contribution guide](/CONTRIBUTING.md) for more information. Anything is very welcome! Even telling your friends about LDPL is a very easy and very useful contribution!

![Contributing Dinosaur](https://github.com/Lartu/ldpl/blob/master/images/tutorial-logo.png)

Contributors are expected to behave by the [Dragon Tamers Code](/CODE_OF_CONDUCT.md). Tl;dr: be nice.

If you want to talk to the rest of us, you can find the LDPL community at [r/LDPL](https://reddit.com/r/LDPL) and via IRC on [irc.freenode.net](http://irc.freenode.net/), channel #ldpl. Also on Telegram via the [ldpllang](https://t.me/ldpllang) group. The IRC and Telegram channels are bridged. You are welcome to open new LDPL channels anywhere else if you like.

## Where can I get more help, if I need it?

You are welcome to submit an issue to this repository, check the [LDPL website](https://www.ldpl-lang.org) or join the community channels stated in the previous section. There are a few extra ways to get in touch with us stated on the website.

## License

This LDPL Compiler is distributed under the GNU General Public License 3.0. All LDPL Dinosaur logos where created by [Lartu](https://github.com/Lartu) and are released under a Creative Commons Attribution 4.0 International (CC BY 4.0) license.
