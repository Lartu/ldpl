![LDPL](http://ldpl.lartu.net/ldpl-logo-new.png)

[LDPL](http://ldpl.lartu.net/) is a programming language designed to be very easy to read and understand. Also as a joke, as its syntax is based on that from COBOL.

This repository contains the source for the LDPL interpreter.

## How it works

The interpreter takes a source file (or more) of LDPL code. It parses it and compiles it into [NariVM](https://github.com/lartu/narivm) intermediate code. Then NariVM -included as a library- runs said intermediate code.

## About the language

LDPL is standarized [here](http://ldpl.lartu.net/reference). That's both the language documentation and the compiler writting standard.

## How to use this compiler

First, download the latest release. Then `make` LDPL in the `src` folder. Then write some LDPL source code, say `source.lsc`. Then compile and run said source code using `ldpl source.lsc`. Done! For more info on the interpreter run `ldpl -h`. Example code can be found on [the LDPL website](http://ldpl.lartu.net).

## License

LDPL is distributed under the GNU General Public License 3.0.
