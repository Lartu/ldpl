
<p align="center">
  <img src="https://github.com/Lartu/ldpl/blob/master/images/ldpl-4.0-logo.png">
  <br><br>
  <img src="https://img.shields.io/badge/stable_release-v5-blue.svg">
  <img src="https://img.shields.io/badge/license-apache_2.0-yellow">
  <img src="https://github.com/Lartu/ldpl/actions/workflows/clang-format-check.yml/badge.svg">
  <a href="t.me/ldpllang"><img src="https://img.shields.io/badge/-LDPL_on_Telegram-red?color=blue&logo=telegram&logoColor=white"></a>
</p>

[**LDPL**](https://www.ldpl-lang.org/) is a powerful, general-purpose compiled programming language designed
from the ground up to be excessively **expressive**, **readable**, **fast** and **easy** to learn.
It mimics plain English, in the likeness of the good parts of older programming languages like COBOL.
It even supports UTF-8 out of the box.

LDPL 5 is now considered **complete** and in [minimal evolution](https://tratt.net/laurie/blog/2023/how_big_should_a_programming_language_be.html) mode.
This means that language is supported, but its specification will intentionally be kept as unchanged as possible.

This repository contains the source code and [releases](https://github.com/Lartu/ldpl/releases) of the LDPL compiler.

### Example Code

```ruby
# Hello There Example
data: 
  name is text
  i is number
  
procedure: 
  display "Hello there, what's your name?"
  accept name
  for i from 0 to 10 step 1 do
    display "你好, " name "!" crlf
  repeat
```

This code greets the user and asks them to enter their name, then it greets them in Chinese, ten times. Easy as pie and super legible.
Check the [official website](https://www.ldpl-lang.org/) for more examples,
including a Brainf*ck interpreter and Bellman-Ford's Algorithm!

## LDPL Philosophy

LDPL is a language designed to be easy to understand and learn, and simple to write and use.
We believe that coding should be like that.
Compiling code should be effortless and straightforward: a single, flagless command should be enough to compile any source.
Every statement in the language should do one and only one thing, not depending on the context.
The compiler should deal with complex, low-level stuff like encoding, sockets, floating-point number comparison, etc., transparently to the user.
Hard stuff shouldn't be.

We understand that this philosophy may lead to longer source code, more verbose statements, and extra steps to reach an end,
but it should make coding in LDPL easier and more enjoyable than in other languages.

As one user once put it, *"Usually when I'm programming, I feel like I'm in a big fancy jet and there's a lot of turbulence
and it's not going well but then all of a sudden it's smooth air again and the drink cart comes along and I get a ginger
ale and it's great. But with LDPL, I feel like I'm a cub scout out in the woods with a box of matches and a hatchet and
my Scout's Handbook (the LDPL Docs) just exploring and figuring it out as I go. Whenever I run into a problem I just check
my handbook and, sure enough, there's a solution right there waiting for me!"*.

We want to make LDPL a language you'll love not because it lets you do many things in one line or because of how modern it is:
but because it's designed to stay by your side and tell you everything's is going to be okay even when things look rough.

## Installing LDPL

Clone this repository. Go into the cloned directory, open a terminal and run `make` and `make install`.
You can also optionally `make man` to re-build the man documentation (PHP is required for this).
This will install LDPL and the LDPL documentation (`man ldpl`) on your system.
LDPL requires only **C++11** to compile.

### 🍺 Homebrew

If you prefer [Homebrew](https://brew.sh), you can install LDPL by running `brew install ldpl`.


### 🐦 Snap

If you prefer [Snap](https://snapcraft.io/), you can install LDPL by running `snap install ldpl-lang`.


## Learning LDPL and Read the Docs

![Learning Dinosaur](https://github.com/Lartu/ldpl/blob/master/images/reference-logo.png)

If you want to learn how to code in LDPL, there's a little **tutorial** at https://learnxinyminutes.com/docs/ldpl/.
Also, be sure to check the examples on the official LDPL website.

[The **LDPL documentation** is available here](https://docs.ldpl-lang.org/).
The documentation is also uploaded to the [docs](docs) folder of this repository and can be read and modified from there.

The LDPL documentation can also be found on your system using `man ldpl` when you install LDPL by using `make install`.
The man page is also available in the [man](/man) folder of this repository.
If you download a precompiled LDPL binary, the man documentation is included as `ldpl.1`, but you will have to install it yourself.

## How to use this compiler

To use the compiler, you must have a C++ compiler already installed on your system and have mapped it to `c++` on your `PATH`.
The LDPL Compiler compiles LDPL code to C++ code, and thus this is a requirement for it to work.

Once the compiler is set up, write some LDPL source code, for example `source.ldpl`.
Then compile the source code using `ldpl source.ldpl`. The compiled, executable binary file will be saved as `source-bin`.
For more information on the compiler, run `ldpl -h` and check the [docs](https://docs.ldpl-lang.org/#the-ldpl-compiler).

### C++ extensions

LDPL supports extensions written in C++. Extensions are `.cpp`, `.o`, or `.a` files that can be imported into your program.

For a guide to writing and building C++ extensions, see the [LDPL official documentation](https://docs.ldpl-lang.org/cppext/).

## How can I contribute to LDPL?

![Contributing Dinosaur](https://github.com/Lartu/ldpl/blob/master/images/tutorial-logo.png)

There are many ways to contribute to the LDPL project. You can fix bugs, add issues, write examples, write software in LDPL, etc.
Check out the [contribution guide](https://www.ldpl-lang.org/contribute.html) for more information. Most anything is very welcome!
Even telling your friends about LDPL is a very easy and very useful contribution.

Contributors are expected to behave by the [LDPL Code of Conduct](https://www.ldpl-lang.org/conduct.html). TL;DR: be nice to everyone.

### Community

If you want to talk to the rest of us, join the LDPL community in the [LDPL Programming Language **Telegram** Group](https://t.me/ldpllang)!
There's also [r/LDPL](https://reddit.com/r/LDPL) on **Reddit**.
You are also welcome to open new LDPL channels anywhere else.

## Supporting the LDPL Project

There is [LDPL merchandise available](https://www.teepublic.com/user/lartu), due to popular request.
We've paired with TeePublic to bring you everything from shirts, to mousepads, to coffee mugs.
All money made from selling these goodies goes to funding the language, paying for the server that keeps the LDPL website online, etc.

Visit the LDPL [Donations and Merchandise](https://www.ldpl-lang.org/support.html) page for more information.

## Getting Help

If you have any questions regarding the LDPL project, you are welcome to submit an issue to this repository,
check the [LDPL website](https://www.ldpl-lang.org) or join the community channels stated in the previous section.
There are no dumb questions, just ask away.

## 📜 License

The LDPL Compiler is distributed under the Apache 2.0 License.
All LDPL Dinosaur logos were created by [Lartu](https://github.com/Lartu) and are released under a Creative Commons
Attribution 4.0 International (CC BY 4.0) license.
