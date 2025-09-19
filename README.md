
<p align="center">
  <img src="https://github.com/Lartu/ldpl/blob/master/images/ldpl-4.0-logo-white-stroke.png">
  <br><br>
  <img src="https://img.shields.io/badge/last_release-5.1.0_'Groovy_Gualicho'-green.svg">
  <img src="https://img.shields.io/badge/license-Apache_2.0-orange">
  <!--<img src="https://github.com/Lartu/ldpl/actions/workflows/clang-format-check.yml/badge.svg">-->
  <a href="t.me/ldpllang"><img src="https://img.shields.io/badge/-LDPL_on_Telegram-red?color=blue&logo=telegram&logoColor=white"></a>
</p>

[**LDPL**](https://www.ldpl-lang.org/) is a powerful, general-purpose compiled programming language built
to be **expressive**, **readable**, **fast**, and **easy to learn**. Inspired by the best parts of older languages such as COBOL,
it uses plain-English syntax and supports Unicode natively!

This repository contains the [source code](https://github.com/Lartu/ldpl/tree/master/src)
and [releases](https://github.com/Lartu/ldpl/releases) of the LDPL compiler, along with
the full documentation and the complete LDPL website.

```ruby
# Hello There Example
-- DATA --
name IS TEXT
i IS NUMBER
  
-- PROCEDURE --
DISPLAY "Hello there, what's your name?"
ACCEPT name
# Greet the user in Chinese ten times.
FOR i FROM 0 TO 10 DO
    PRINT "你好, " name "!"
REPEAT
```

Easy as pie and highly readable, LDPL supports multiple data types and programming patterns.
It can easily interface with C and C++ code and libraries, supports arbitrarily large integer arithmetic,
and makes both procedural and parallel programming straightforward.
LDPL is a full-fledged language that has been battle-tested in production systems.

> [!IMPORTANT]
> LDPL 5.2, the current development version, is stable and up to date. The rest of this repository, however, still needs a bit of tidying up.
> I am actively working on it, and you can expect things to improve over the coming days!
>
> **What needs to be done**
> - Improve and update the documentation; ship it with the project and drop the ReadTheDocs dependency.
> - Remove MkDocs, Snapcraft, and any other unused build/publish tooling.
> - Update all README files.
> - Consolidate the documentation and the website into this repository so everything lives in one place.
> 
> _Lartu (September 14th, 2025)_

## LDPL Philosophy

LDPL is a language designed to be easy to understand, learn, write, and use. We believe coding should be like that. Compiling code should be effortless and straightforward: a single, flagless command should be enough to compile any source. Every statement in the language should perform one, and only one, function, independent of the context. The compiler should handle complex, low-level tasks like encoding, sockets, floating-point number comparison, etc., transparently to the user. Difficult things shouldn’t feel difficult.

We understand that this philosophy may result in longer source code, more verbose statements, and additional steps to reach a solution, but it aims to make coding in LDPL easier and more enjoyable than in other languages.

As one user once said:

>*"Usually when I'm programming, I feel like I'm in a big fancy jet and there's a lot of turbulence and it's not going well but then all of a sudden it's smooth air again and the drink cart comes along and I get a ginger ale and it's great. But with LDPL, I feel like I'm a cub scout out in the woods with a box of matches and a hatchet and my Scout's Handbook (the LDPL Docs) just exploring and figuring it out as I go. Whenever I run into a problem I just check my handbook and, sure enough, there's a solution right there waiting for me!"*

We want LDPL to be a language you’ll love — not because it lets you do many things in one line or because it’s modern, but because it’s designed to stay by your side and reassure you that everything will be okay, even when things look rough.

## Installing LDPL

To install LDPL, clone this repository, navigate to the cloned directory, open a terminal, and run `make` followed by `make install`. That's it!

You can also optionally run `make man` to re-build the man documentation (PHP is required for this).
This process will install LDPL and its documentation (`man ldpl`) on your system.
LDPL only requires **C++11** to compile and run.

### Alternative Installation Methods

Bear in mind that these other installation methods may install an outdated version of LDPL, as they are maintained by external contributors.
Make sure that the version you are about to install matches the last LDPL release.

* **🍺 Homebrew**: If you prefer [Homebrew](https://brew.sh), you can install LDPL by running `brew install ldpl`.
* **🐦 Snap**: If you prefer [Snap](https://snapcraft.io/), you can install LDPL by running `snap install ldpl-lang`.


## Learning LDPL and Read the Docs

![Learning Dinosaur](https://github.com/Lartu/ldpl/blob/master/images/reference-logo.png)

If you want to learn how to code in LDPL, there is a brief tutorial available at [https://learnxinyminutes.com/docs/ldpl](https://learnxinyminutes.com/docs/ldpl). Additionally, be sure to check out the examples on the official LDPL website.

[The **LDPL Documentation** is available here](https://docs.ldpl-lang.org/).
The documentation is also uploaded to the [docs](docs) folder of this repository and can be read and modified from there.

The LDPL documentation can also be found on your system using `man ldpl` when you install LDPL using `make install`.
The man page is also available in the [man](/man) folder of this repository.

## How to use this compiler

To use the compiler, you must have a C++ compiler installed on your system and ensure it is accessible as `c++` in your PATH.
The LDPL compiler translates LDPL code into C++ code, making this a necessary requirement for it to function.

Once the compiler is set up, write some LDPL source code — for example, `source.ldpl`.
Then, compile the source code using `ldpl source.ldpl`. The compiled executable binary will be saved as `source-bin`.
For more information about the compiler, run `ldpl -h` or refer to the [docs](https://docs.ldpl-lang.org/#the-ldpl-compiler).

### C++ extensions

LDPL supports extensions written in C++. Extensions are `.cpp`, `.o`, or `.a` files that can be imported into your program.

For a guide on writing and building C++ extensions, refer to the [LDPL Docs](https://docs.ldpl-lang.org/cppext/).

## How can I contribute to LDPL?

![Contributing Dinosaur](https://github.com/Lartu/ldpl/blob/master/images/tutorial-logo.png)

There are many ways to contribute to the LDPL project.
You can fix bugs, open issues, write examples, develop software in LDPL, and more.
Check the [contribution guide](https://www.ldpl-lang.org/contribute.html) for details.
Almost any contribution is welcome — even simply telling your friends about LDPL is an easy and valuable way to help.

Contributors are expected to adhere to the [LDPL Code of Conduct](https://www.ldpl-lang.org/conduct.html). In short: be nice to everyone.

### Community

If you’d like to chat with us, join the LDPL community in the [LDPL Programming Language **Telegram** Group](https://t.me/ldpllang)!
There's also [r/LDPL](https://reddit.com/r/LDPL) on **Reddit**.

You are also welcome to create new LDPL channels on any other platform.

## Merchandise

Due to popular demand, [LDPL merchandise is available](https://www.teepublic.com/user/lartu). We’ve partnered with TeePublic to offer a variety of items, from shirts and mousepads to coffee mugs.

## Getting Help

f you have any questions about the LDPL project, feel free to submit an issue in this repository, visit the [LDPL website](https://www.ldpl-lang.org) or join the community channels mentioned in the previous section. Remember, there are no dumb questions — just ask!

## 📜 License

The LDPL Compiler is distributed under the Apache 2.0 License.
All LDPL Dinosaur logos were created by [Lartu](https://github.com/Lartu) and are released under the Creative Commons Attribution 4.0 International (CC BY 4.0) license.
