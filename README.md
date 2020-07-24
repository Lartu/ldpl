<p align="center">
  <img src="https://github.com/Lartu/ldpl/blob/master/images/ldpl-4.0-logo.png">
  <br><br>
  <img src="https://img.shields.io/badge/stable_release-4.4.3-blue.svg?style=flat-square">
  <img src="https://img.shields.io/badge/dev_version-4.5.0-purple.svg?style=flat-square">
  <a href="https://travis-ci.org/Lartu/ldpl"><img src="https://img.shields.io/travis/Lartu/ldpl/master?style=flat-square"></a>
  <img src="https://img.shields.io/badge/license-apache_2.0-yellow?style=flat-square">
  <a href='https://www.freelists.org/list/ldpl'><img src="https://img.shields.io/badge/Mailing List-ldpl-lightgray.svg?style=flat-square"></a>
  <a href="https://www.freenode.net"><img src="https://img.shields.io/badge/irc-%23ldpl-orange?style=flat-square"></a>
</p>

[**LDPL**](https://www.ldpl-lang.org/) is a powerful compiled programming language designed from the ground up to be excessively **expressive**, **readable**, **fast** and **easy** to learn. It mimics plain English, in the likeness of the good parts of older programming languages like COBOL, with the desire that it can be understood by anybody. LDPL was designed to run on Unix systems, including AMD-64 Linux, macOS, ARMv8 Linux, Android Termux and both Intel and PowerPC OS X (*tested from Tiger 10.4 onwards*). It even supports UTF-8 out of the box.

This repository contains the source code and [releases](https://github.com/Lartu/ldpl/releases) of the LDPL compiler and other useful goodies.

### Index
 * [Introduction](https://github.com/Lartu/ldpl#readme)
   * [LDPL Code Example](https://github.com/Lartu/ldpl#-example-ldpl-code)
   * [What can I use LDPL for?](https://github.com/Lartu/ldpl#-what-can-i-use-ldpl-for)
 * [LDPL Philosophy](https://github.com/Lartu/ldpl#-ldpl-philosophy)
 * [How to Install LDPL](https://github.com/Lartu/ldpl#-how-to-install-ldpl)
 * [Learning LDPL & Documentation](https://github.com/Lartu/ldpl#-learning-ldpl-and-read-the-docs)
 * [How to use the LDPL Compiler](https://github.com/Lartu/ldpl#-how-to-use-this-compiler)
   * [Compiler Options](https://github.com/Lartu/ldpl#-compiler-options)
   * [C++ Extensions](https://github.com/Lartu/ldpl#-c-extensions)
 * [Libraries](https://github.com/Lartu/ldpl#-libraries)
   * [The LDPL Standard Library](https://github.com/Lartu/ldpl#-the-ldpl-standard-library)
   * [Other Libraries](https://github.com/Lartu/ldpl#-other-ldpl-libraries)
 * [LDPL Compatible Editors](https://github.com/Lartu/ldpl#-ldpl-compatible-editors)
 * [Contributing to the LDPL Project](https://github.com/Lartu/ldpl#-how-can-i-contribute-to-ldpl)
 * [Supporting the LDPL Project](https://github.com/Lartu/ldpl#-supporting-the-ldpl-project)
 * [Getting Help](https://github.com/Lartu/ldpl#-getting-help)
 * [License](https://github.com/Lartu/ldpl#-license)

### 📒 Example LDPL code

```coffeescript
# Hello There Example
data: 
  name is text
  
procedure: 
  display "Hello there, what's your name?"
  accept name
  display "你好, " name "!" crlf
```

This code greets the user and asks them to enter their name, then it greets them in Chinese. Easy as pie and super legible. Check the [official website](https://www.ldpl-lang.org/) and the [examples](/examples) subfolder for more examples, including a Brainf*ck interpreter and Bellman-Ford's Algorithm!

### 🎏 What can I use LDPL for?

LDPL can be used to write software just like any other language you know. You can open, edit and save files, do math,
parse and modify text, etc. Some of these tasks are made easier by the fact that LDPL natively knows how to work with
UTF-8 strings. By using the [available libraries](https://github.com/Lartu/ldpl#-some-other-ldpl-libraries), LDPL becomes
a language *specially* well suited for writing **Telegram Bots**, **IRC Bots**, **Text UI Applications** and **Simple Network Applications**.
Code written in LDPL is easy to read, runs fast and can easily interface with C++ code,
so you can do anything you want, really.

## 📯 LDPL Philosophy

LDPL is a language designed to be easy to understand and simple to write. Designed to be easy to learn and simple to use.
We believe that coding should be easy and simple. Nothing should become a frustrating chore. Compiling source code should be easy and
simple: a single, flagless command should be enough to compile any source code. Every statement in the language should do 
one and one thing only, not multiple things depending on the context. Complex, low level stuff like encoding, sockets, 
floating point number comparison, etc., should be dealt by the compiler, transparently to the user. Hard stuff should be
made easier.

We understand that this philosphy may lead to longer source code, more verbose statements and extra steps to reach an end,
but we also believe that it should make coding in LDPL easier and more enjoyable than in other languages.

As one user once put it, *"Usually when I'm programming, I feel like I'm in a big fancy jet and there's a lot of turbulence and it's not going well but then all of a sudden it's smooth air again and the drink cart comes along and I get a ginger ale and it's great. But with LDPL, I feel like I'm a cub scout out in the woods with a box of matches and a hatchet and my Scout's Handbook (the LDPL Docs) just exploring and figuring it out as I go. Whenever I run into a problem I just check my handbook and, sure enough, there's a solution right there waiting for me!"*.

We want to make LDPL a language you'll love not because it lets you do many things in one line, nor because of the way
it runs, nor because of how modern it is: but because it's designed to stay by your side and tell you everything's gonna
be okay even when times get dark.

## 💾 How to install LDPL

First, clone this repository. Then `make` and `make install`. You can also optionally `make man` to re-build the documentation (PHP is required for this). This will install LDPL and the LDPL documentation (`man ldpl`) on your system. LDPL requires only C++11 to compile. LDPL will install some libraries to `/usr/local/lib/ldpl`. If you want to install them in another location, modify the `LDPLLIBLOCATION` variable of the (makefile)[makefile].

Once you have LDPL installed on your system, check the LDPL reference to learn how to use the language. Information on how to compile LDPL scripts and a list of LDPL compatible editors is provided in the sections below.

### 🍺 Homebrew

If you prefer [Homebrew](https://brew.sh), you can install LDPL by running `brew install ldpl`.

## 📚 Learning LDPL and Read the Docs

![Learning Dinosaur](https://github.com/Lartu/ldpl/blob/master/images/reference-logo.png)

If you want to learn how to code in LDPL, there's a little **tutorial** at https://learnxinyminutes.com/docs/ldpl/. Also be sure to check the [examples](./examples)!

[The **LDPL documentation** is available here](https://docs.ldpl-lang.org/). Check that to learn yourself some LDPL (*for the greater good!*). The documentation is also uploaded to the [docs](docs) folder of this repository and can be read and modified from there.

The LDPL documentation can also be found on your system using `man ldpl` when you install LDPL by using `make install` or the LDPL Install Wizard. The man page is also available in the [man](/man) folder of this repository. If you download a precompiled LDPL binary, the man documentation is included as `ldpl.1`, but you have to install it yourself.

## 💻 How to use this compiler

To use the compiler, you must have a C++ compiler already installed on your system and have mapped it to `c++`, found on your `PATH`. The LDPL Compiler compiles LDPL code to C++ code and thus this is a requirement for it to work.

Once the compiler is set up, write some LDPL source code, for example `source.ldpl`.
Then compile the source code using `ldpl source.ldpl`. The compiled, executable binary file will be saved as `source-bin`.
Done! For more information on the compiler run `ldpl -h` and check the [docs](https://docs.ldpl-lang.org/#the-ldpl-compiler).

### 🔧 Compiler options

 * The `-f` flag can be used to pass extra options to the compiler when building extensions. For example, `-f=-lSDL` could be used to link against SDL. The `flag` statement can also be used as well (`flag "-lSDL"`) and its use is recommended.
 * By using `-r` you can just compile the project and print the C++ representation for that code.
 * You can set the output file for the compiled binary with the `-o` flag. For example, if you want to name your program "dog", you could compile it with `ldpl -o=dog main.ldpl`.
 * On Linux platforms, LDPL builds static binaries by default. If you want to build non-static ones use the `-ns` flag.
 * The `-c` flag tells LDPL to accept source code from the standard input. 
 * You can import extensions to your LDPL compilation by using the `-i=` flag. Extensions can be imported by passing `.o`, `.a`, or `.cpp` files to this flag; see the Extensions section below.
 * `-v` and `--version` print out version info and release details.
 * `-h` and `--help` print this list of options.
 
 For more information, check the [docs](https://docs.ldpl-lang.org/#compiler-switches).

### 📦 C++ extensions

LDPL supports extensions written in C++. Extensions are `.cpp`, `.o`, or `.a` files that can be imported into your program using the `EXTENSION` statement. For example `extension "myLibrary.cpp"`.

For a guide to writing and building C++ extensions, see the [LDPL official documentation](https://docs.ldpl-lang.org/cppext/). For a simple example, see the [examples/cpp-extension](./examples/cpp-extension) directory.

## 🏛 Libraries
Libraries can be included using

```coffeescript
include "library_name" # for example: include "std-math"
```

before the `DATA` and `PROCEDURE` sections of your source file.

### 📖 The LDPL Standard Library

A number of useful statements and subroutines are available in the [**LDPL Standard Library**](https://github.com/Lartu/ldpl-std).

### 🦕 Other LDPL Libraries

- [**The LDPL IRC Bot Library**](https://github.com/Lartu/ldpl-irc-bot): the LDPL IRC Bot Library lets you write LDPL programs that connect to IRC servers, join channels, send messages and more in the simplest
way possible.
- [**The LDPL Ncurses Library**](https://github.com/Lartu/ldpl-ncurses): an ncurses wrapper for LDPL designed for creating terminal applications that require more complex text user-interfaces than simple line-feeding.
- [**The LDPL Network Server Library**](https://github.com/Lartu/ldpl-net-server/): the LDPL Network Server Library is an library for creating socket based servers in LDPL. It aims to make it very easy to develop, test and deploy network servers, with as little work as possible.
- [**The LDPL Socket Library**](https://github.com/xvxx/ldpl-socket): the LDPL socket Library allows you to open, close, write to, and read from network sockets in LDPL.
- [**The LDPL Telegram Bot Library**](https://github.com/dgarroDC/ltb): this simple LDPL library lets you create a Telegram bot that can receive and send text messages.
- [**The LDPL URL Resolver Library**](https://github.com/Lartu/ldpl-url-resolver): a super easy to use library for LDPL that helps you resolve URLs into their respective IP addresses.

## 📝 LDPL Compatible Editors

![Gedit](https://www.ldpl-lang.org/images/gedit.png) ![VSC](https://www.ldpl-lang.org/images/vsc.png?v=1) ![VIM](https://www.ldpl-lang.org/images/vim.png) ![ATOM](https://www.ldpl-lang.org/images/atom.png) ![nano](https://www.ldpl-lang.org/images/nanologo.png)

We have highlighting and code formatting extensions available for [**Gedit / Pluma**](https://github.com/Lartu/ldpl/tree/master/highlighting/gedit) (by [Lartu](http://github.com/lartu)), [**Visual Studio Code**](https://marketplace.visualstudio.com/items?itemName=Lartu.ldpl-vsc-extension) (thanks to [xvxx](https://github.com/xvxx) and [Lartu](http://github.com/lartu)), [**nano**](https://github.com/Lartu/ldpl/tree/master/highlighting/nano) (by [Lartu](http://github.com/lartu)), [**Atom**](https://atom.io/packages/language-ldpl) (thanks to [Damián Garro](https://github.com/dgarroDC)) and [**Vim**](https://github.com/araml/ldpl.vim) (thanks to [Araml](https://github.com/araml)).

You can use any other editor (without syntax highlighting) if you like. If know how to write syntax packages for other editors and would like to contribute to the LDPL project, a highlighting extension would be a very welcome contribution.

## 👨🏼‍💻 How can I contribute to LDPL?

![Contributing Dinosaur](https://github.com/Lartu/ldpl/blob/master/images/tutorial-logo.png)

There are many ways to contribute to the LDPL project. You can fix bugs, add issues, write examples, write software in LDPL, add statements to the [Standard Library](https://github.com/Lartu/ldpl-std), etc. Check out the [contribution guide](https://www.ldpl-lang.org/contribute.html) for more information. Anything is very welcome! Even telling your friends about LDPL is a very easy and very useful contribution.

Contributors are expected to behave by the [LDPL Code of Conduct](https://www.ldpl-lang.org/conduct.html). TL;DR: be nice to everyone.

### 🦕🦕🦕 Community

If you want to talk to the rest of us, join the [LDPL Mailing List](https://www.freelists.org/list/ldpl)! You can also find the LDPL community at [r/LDPL](https://reddit.com/r/LDPL), via IRC on [irc.freenode.net](http://irc.freenode.net/), channel *\#ldpl*, and also on Telegram via the [ldpllang](https://t.me/ldpllang) group. The IRC and Telegram channels are bridged. You are welcome to open new LDPL channels anywhere else if you like.

## 👔 Supporting the LDPL Project

The LDPL Programming Language is a non-profit project mantained by people like you from all over the world. If you like what we do, please consider [donating](https://www.ldpl-lang.org/support.html) to the LDPL project.

There's also [LDPL merchandise available](https://www.teepublic.com/user/lartu), due to popular request! We've paired with TeePublic to bring you everything from shirts, to mousepads, to coffee mugs. All money made from selling these goodies goes to funding the language, paying for the server that keeps the LDPL website online, etc.

Visit the LDPL [Donations and Merchandise](https://www.ldpl-lang.org/support.html) page for more information.

## 🔎 Getting Help

If you have any questions regarding the LDPL project you are welcome to submit an issue to this repository, check the [LDPL website](https://www.ldpl-lang.org) or join the community channels stated in the previous section. There are no dumb questions, just ask away.

## 📜 License

The LDPL Compiler is distributed under the Apache 2.0 License. All LDPL Dinosaur logos where created by [Lartu](https://github.com/Lartu) and are released under a Creative Commons Attribution 4.0 International (CC BY 4.0) license.
