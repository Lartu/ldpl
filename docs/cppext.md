!!!Warning
    This section might not be up-to-date. It has yet to be checked.

## About C++ Extensions

Extensions contain sub-procedures and variables that are written in another language and usable in LDPL through the `call external` statement and `extenal` data type keyword. This allows programmers to extend LDPL with new features or to wrap 3rd party libraries and re-use their functionality. 

LDPL supports extensions written in C++. Because LDPL programs compile down to C++, there is no need for a translation layer or bridge: extensions can be included directly into LDPL programs and manipulate, share, and access subprocedures and variables natively. All that's needed is a few naming conventions on the C++ side and the use of the EXTERNAL syntax for variables and subprocedures on the LDPL side.

## Writing C++ Extensions

Extensions can create variables and functions that are accessible from LDPL through the [CALL EXTERNAL](../call-external.md) statement and [EXTERNAL](../external-variables.md) data type keyword. Typically all you need is a single `.cpp` file that you give the `ldpl` compiler when building your program, but you can also use `.o` files, `.a` files, or any combination of them all.

Extensions interact with LDPL in two main way: defining functions and declaring variables.

### Functions

To create a function in C++ that can be called from an LDPL program, you must follow two rules:

1. The function's type must be `void(void)`, ex: `void MY_FUNC();`
2. The function's name must conform to LDPL's [Extension Naming Convention](external-identifier-naming-scheme.md).

Because LDPL functions don't accept arguments or return values, to be callable from LDPL your C++ functions musn't either.

And because LDPL doesn't "know" the names of your functions and instead allows the programmer to call them using the `EXTERNAL` syntax, all C++ variable and subprocedure names must contain only `A-Z`, `0-9`, and the `_` character. Everything else on the LDPL side will get converted to an underscore \(`_`\) when referencing the C++ side.

**Example:**

For example, this function:

```cpp
void PROMPT_ADD() 
{
  int a, b, sum;
  cout << "1st number: ";
  cin >> a;
  cout << "2nd number: ";
  cin >> b;
  cout << "sum: " << sum << end; 
}
```

Once defined and built into LDPL, can be called using:

```text
CALL EXTERNAL prompt-add
```

### Variables

To create or reference a variable in a C++ extension that is shared with an LDPL program, you must follow two rules:

1. The variable's name must conform to LDPL's [Extension Naming Convention](external-identifier-naming-scheme.md).
2. The C++ type of the variable must match LDPL's internal type usage.

The first rule should be familiar from the functions section: all C++ variable and subprocedure names must contain only `A-Z`, `0-9`, and the `_` character. Everything else on the LDPL side will get converted to an underscore \(`_`\).

For the second, here's the mapping between types:

| LDPL Data Type | C++ Type |
| :--- | :--- |
| `TEXT` | `std::string` |
| `NUMBER` | `ldpl_number` |
| `TEXT MAP` | `ldpl_vector<std::string>` |
| `NUMBER MAP` | `ldpl_vector<ldpl_number>` |
| `TEXT LIST` | `ldpl_list<std::string>` |
| `NUMBER LIST` | `ldpl_list<ldpl_number>` |

The definitions for these data types can be found in the [LDPL C++ Types](../ldpl-c++-types.md) section.

**Example:**

Declaring `TEXT`and `NUMBER` variables is easy on the C++ side:

```cpp
std::string NAME;
ldpl_number AGE;
std::string STREET_ADDRESS;
```

These will be available to an LDPL program to declare in its `DATA:` section:

```text
DATA:
name IS EXTERNAL TEXT
age IS EXTERNAL NUMBER
street-address IS EXTERNAL TEXT
```

### Accessing Variables in Functions

Since LDPL and C++ are using the same variable when you use the `EXTERNAL` keyword, any changes you make to the variable's content are shared. Just use them like you would use regular C++ variables.

```cpp
ldpl_number A, B, SUM;
void ADD() 
{
    SUM = A + B;
}
```

```text
DATA:
A IS EXTERNAL NUMBER
B IS EXTERNAL NUMBER
SUM IS EXTERNAL NUMBER

PROCEDURE:
STORE 100 IN A
STORE 250 IN B
CALL EXTERNAL add
DISPLAY SUM CRLF
```

Building and running this program will print `350`.

### LDPL MAPs

The `MAP` types are a bit trickier - they are defined as `ldpl_vector<T>`, so you'll want to declare this prototype in your `.cpp` or `.h` file to use it in your extension:

```cpp
#ifndef ldpl_vector
template<typename T>
struct ldpl_vector {
    T& operator [] (const std::string& i);
    T& operator [] (double i);
};
#endif
```

Now you can use MAPs in LDPL:

```text
DATA:
Names IS EXTERNAL TEXT MAP
```

And in C++:

```cpp
ldpl_vector<std::string> NAMES;

// later...
NAMES[0] = "Pauline"
NAMES[1] = "just Paul"
```

!!!hint
    The internal datatype for LDPL MAPs is called ldpl\_vector because in LDPL releases prior to LDPL 3.1.0 Diligent Dreadnoughtus, MAPs were called VECTORs.

## Building C++ Extensions

Extensions are easy to build: when compiling your LDPL program, use the `-i=` flag to pass in `.cpp` files, `.o` files, or `.a` files to the LDPL compiler. They'll get included in your program and be available using the `EXTERNAL` statements.

For example, if your LDPL source file is called `mySource.ldpl` and you want to include `otherFile.cpp` to your project, you just do

```text
$ ldpl -i=otherFile.cpp mySource.ldpl
```

Starting from LDPL 4.0, you can also use the `EXTENSION` statement to include extensions to your project, just like you'd use the INCLUDE statement to include LDPL source files:

```coffeescript
EXTENSION "myExtension.cpp"

procedure:
  call external someFunction
```

The location where extensions are searched when using the `EXTENSION` statement is relative to the file that includes them.

If your C++ extension files require extra flags to be passed to the C++ compiler in order to compile \(for example, `-lSDL` when working with SDL\) you can use the `-f=` flag to pass those flags to it. Following the SDL example, you could do

```text
$ ldpl -i=mySDLSource.cpp mySource.ldpl -f=-lSDL
```

and the C++ compiler will be executed with the `-lSDL` flag.

```text
$ ldpl -i=<extension file> code.ldpl
```

Starting from LDPL 4.0, you can also use the `FLAG` statement to pass flags to the C++ compiler:

```coffeescript
EXTENSION "myExtension.cpp"
FLAG "-fpermissive" # This is the same as passing -f=-fpermissive to the LDPL compiler.
FLAG "-lSDL2" # This is the same as passing -f=-lSDL2 to the LDPL compiler.

procedure:
  call external someFunction
```

## "Hello World" C++ Example

File **simple.cpp**:
```cpp
#include <iostream>
void SIMPLE(){
        std::cout << "Very simple!" << std::endl;
}
```

File **simple.ldpl**:
```text
PROCEDURE:
CALL EXTERNAL simple

```

Console:

```bash
$ ldpl -i=simple.cpp simple.ldpl
LDPL: Compiling...
* File(s) compiled successfully.
* Saved as simple-bin
$ ./simple-bin
Very simple!
```

## EXTERNAL SUB-PROCEDUREs

Sometimes when writting C++ Extensions you'll find yourself in the need of declaring a function in C++ but coding it in LDPL. This is the opposite of writing C++ functions and calling them from LDPL, it's writing LDPL SUB-PROCEDUREs and calling them from C++.

These C++ calleable SUB-PROCEDUREs are called EXTERNAL SUB-PROCEDUREs, as they can be called from an EXTERNAL medium.

In order to declare an EXTERNAL SUB-PROCEDURE you must first declare it in your C++ source code. Say, for example, that you want to declare a SUB-PROCEDURE called `helloWorld`. In your C++ you should write the following line:

```cpp
void HELLOWORLD();
```

Note that EXTERNAL SUB-PROCEDUREs cannot receive any kind of parameters and must be declared as `void`. You may then call the EXTERNAL SUB-PROCEDURE from C++ code like:

```cpp
int myCPPFunction(){
  HELLOWORLD();
  return 1;
}
```

Once that's taken care of, you can declare your EXTERNAL SUB-PROCEDURE as any other SUB-PROCEDURE in LDPL by preppending the identifier EXTERNAL to the SUB-PROCEDURE declaration:

```coffeescript
EXTERNAL SUB-PROCEDURE helloWorld #or EXTERNAL SUB
  #Code here...
END SUB-PROCEDURE
```

These SUB-PROCEDUREs can be called from LDPL as any other SUB-PROCEDURE, but their names must follow the naming scheme explained in the **Naming Schemes** section as any other C++ interfacing component.


## EXTERNAL Variables

Variables defined in extensions can be accessed by prefacing their data type declaration with the `EXTERNAL` keyword. This must occur in the DATA section of an LDPL program. Once an external variable is declared, it can be used just like any other LDPL variable.

**Syntax:**

```text
<variable> IS EXTERNAL <data type>
```

**Example:**

```text
DATA:
RL-PROMPT IS EXTERNAL TEXT
WINDOW.SIZE IS EXTERNAL NUMBER
```

## LDPL Data Types in C++

A .h file containing type definitions for LDPL Data Types can be found [here](https://github.com/Lartu/ldpl-irc-bot/blob/master/ldpl-irc-bot/ldpl-types.h).


