# LDPL 🦕

LDPL is a programming language designed to be very easy to read and understand. Also as a joke, as its syntax is based on that from COBOL.

This repository contains the source for the LDPL compiler. It takes LDPL source code and spits [NariVM](https://github.com/lartu/narivm) runnable code. So, in order to use it, you have to also install NariVM.

## About the language

LDPL is standarized [here](https://ldpl.lartu.net). That's both the language documentation and the compiler writting standard.

## How to use this compiler

First, `make ldpl` in the `src` folder. Then write some LDPL source code, say `source.lsc`. Then compile said source code, `ldpl source.lsc`. The compiled NariVM runnable code will be saved as `out.nvm`. Run the compiled file with Narivm, `nvm out.nvm`.
