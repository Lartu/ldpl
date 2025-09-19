
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
> Version 5.1, the previous release, has a number of known issues. Please build and use 5.2 whenever possible.
> 
> _Lartu (September 14th, 2025)_

## LDPL Philosophy

LDPL is designed to be easy to understand, learn, write, and use, because we believe coding should feel that way.
Compiling LDPL code is effortless and straightforward: a single, flagless command is enough to build any source.
Every statement in the language performs one, and only one, function, regardless of context.
The compiler takes care of the complex, low-level details such as encoding, sockets, floating-point comparisons, and more, transparently for the user.
LDPL makes difficult things feel easy.

We know this philosophy can lead to slightly longer source code, more verbose statements, and extra steps to reach a solution — but the goal is to make coding in LDPL simpler, clearer, and more enjoyable than in other languages.

>*"Usually when I'm programming, I feel like I'm in a big fancy jet and there's a lot of turbulence and it's not going well but then all of a sudden it's smooth air again and the drink cart comes along and I get a ginger ale and it's great. But with LDPL, I feel like I'm a cub scout out in the woods with a box of matches and a hatchet and my Scout's Handbook (the LDPL Docs) just exploring and figuring it out as I go. Whenever I run into a problem I just check my handbook and, sure enough, there's a solution right there waiting for me!"*

We want LDPL to be a language you enjoy using, not because it focuses on brevity or follows trends, but because it is reliable, approachable, and helps you get things done with confidence.
LDPL is a language that reassures you that everything will be okay!

## Installing LDPL

To install LDPL, simply clone this repository, navigate to the cloned directory, open a terminal, and run `make` followed by `make install`. That's it! 

LDPL requires only **C++11** to compile and run, and it works out of the box on Unix systems such as Linux and macOS.

![Contributing Dinosaur](https://github.com/Lartu/ldpl/blob/master/images/tutorial-logo.png)

## Learning LDPL

If you want to learn how to code in LDPL, the best way is to read the documentation and explore a few examples.

[The LDPL Documentation is available here](https://docs.ldpl-lang.org/).
It is also included in the [docs](docs) folder of this repository, where you can read and modify it directly.

![Learning Dinosaur](https://github.com/Lartu/ldpl/blob/master/images/reference-logo.png)

> [!IMPORTANT]
> The examples will be linked here soon — we are working on it!

## How to use this compiler

To use the compiler, you must have a C++ compiler installed on your system and ensure it is accessible as `c++` in your PATH.  
The LDPL compiler translates LDPL code into C++ code, which makes this a necessary requirement for it to function.

Once the compiler is set up, write some LDPL source code, for example `source.ldpl`.
Then compile the source code using `ldpl source.ldpl`.
The compiled executable binary will be saved as `source-bin`.

For more information about the compiler, run `ldpl -h` or refer to the [docs](https://docs.ldpl-lang.org/#the-ldpl-compiler).

## Contributing to LDPL

There are many ways to contribute to the LDPL project.
You can fix bugs, open issues, write examples, develop software in LDPL, and much more.
Check the [contribution guide](https://www.ldpl-lang.org/contribute.html) for details.

Most contributions are welcome, even simply telling your friends about LDPL is a valuable way to help.

Contributors are expected to follow the [LDPL Code of Conduct](https://www.ldpl-lang.org/conduct.html).
In short: be nice to everyone.

## Community

If you would like to chat with us, join the LDPL community in the [LDPL Programming Language Telegram Group](https://t.me/ldpllang)!  
There is also [r/LDPL](https://reddit.com/r/LDPL) on Reddit.  

You are always welcome to create new LDPL channels on any other platform.

## Getting Help

If you have any questions about the LDPL project, feel free to submit an issue in this repository, visit the [LDPL website](https://www.ldpl-lang.org), or join the community channels mentioned above.
Remember, there are no dumb questions — just ask!

## License

The LDPL Compiler is distributed under the Apache 2.0 License.
All LDPL Dinosaur artwork and logos were created by [Lartu](https://github.com/Lartu) and are released under the Creative Commons Attribution 4.0 International (CC BY 4.0) license.
