### Hello there!

Welcome to the LDPL contributors guide, thank you for being here! This guide will tell you all you need to know in order to contribute to the LDPL project.

### Who can contribute to LDPL?

Anyone can contribute to the LDPL project! As stated on the README.md file, we accept all kinds of contributions anyone can make. From adding statements to the language (that's in fact really easy!) to fixing bugs, adding issues, writing examples, documentation writing some software in LDPL, etc. Anything you consider will make the project grow and better, you are welcome to contribute it. Even a simple drawing or telling your friends about LDPL!

### What do I need to know to contribute to LDPL?

Please take a moment to familiarize yourself with the [Code Of Conduct](/CODE_OF_CONDUCT) (you are expected and required to behave by this code) and the [LDPL reference](https://docs.ldpl-lang.org/) (not really a *must* per se, but that's where all the language is documented).

C++ knowledge is a must if you want to contribute to the compiler code, and LDPL knowledge is a must if you want to write examples or LDPL software. Drawing is not really a must if you want to submit a drawing, but you may get better results if you know how to draw. We'll love your art anyway, though.

### Will you accept my pull request?

We tend to accept most pull requests, unless they break the language in some way or another. In that case, we'll discuss with you what can be done to have it accepted. We wouldn't like your hard made contribution to go to waste!

### Steps for creating good issues or pull requests.

Please take a moment to understand the LDPL compiler source code if you are submiting a new feature to it. It's made of just three files: `ldpl.cpp` where the compiler is mostly written, `ldpl.h` with variable and function definitions (and an occasional struct) and `ldpl_lib.cpp` that is a library that is included in every binary compiled with the LDPL compiler, that includes definitions for many functions used within the language.

LDPL is one big `if` statement, with one `if` for every statement of the language. Check the ones that are already written if you want to submit new statements, is really easy. **Test your new statements** before submiting them, please!

If you are writing documentation, try to make it as clear as possible. It's nice when you don't know nothing about a certain technology and you find a tutorial that explains everything from the ground up in a way you can understand without reading pages and pages of man documentation and shady html files. You love the guy who wrote that tutorial. Be that guy.

If you are writing LDPL examples, please comment them. Comment every line if you deem it necessary. But make them as clear as possible.

If you are writing LDPL software, please try to make your code clear as well. It's your code, though, we won't bite you if you don't want to make it nice.

If you are submiting issues, PLEASE be as clear as possible. Tell us where the error happened, tell us what were you doing, what code triggered it, etc. It just doesn't work when someone posts "please fix this function it doesn't work thx bye".

If you are submiting a drawing, feel free to take any artistic license you like. "Look mom, I drew the LDPL dinosaur!".

If you are submiting LDPL fanfiction, please ask yourself why am I doing this. Then kindly create a pull request.

### Links to external documentation, mailing lists, or a code of conduct.

- Official LDPL Website: [https://www.ldpl-lang.org/](https://www.ldpl-lang.org/)
- LDPL Reference: [https://docs.ldpl-lang.org/](https://docs.ldpl-lang.org/)
- LDPL Reddit Community: [https://reddit.com/r/ldpl](https://reddit.com/r/ldpl)
- IRC Channel: #ldpl at [irc.freenode.net](http://irc.freenode.net)
- [Dinosaur Tamers Code](/CODE_OF_CONDUCT)
- You can also send me an email at *mdelrio (at) dc.uba.ar*.
