# LDPL 🦕

LDPL is a programming language designed to be very easy to read and understand. Also as a joke, as its syntax is based on that from COBOL.

This repository contains the source for the LDPL interpreter.

## How it works

The interpreter takes a source file (or more) of LDPL code. It parses it and compiles it into [NariVM](https://github.com/lartu/narivm) intermediate code. Then NariVM -included as a library- runs said intermediate code.

## About the language

LDPL is standarized [here](https://ldpl.lartu.net). That's both the language documentation and the compiler writting standard.

## How to use this compiler

First, `make ldpl` in the `src` folder. Then write some LDPL source code, say `source.lsc`. Then compile and run said source code using `ldpl source.lsc`. Done!

## License

LDPL is distributed under the GNU General Public License 3.0.
