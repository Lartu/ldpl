## About C++ Extensions

**C++ extensions** are packages of code written in C++ that can interface with LDPL
code and be added to your LDPL projects.

Because LDPL programs compile down to C++, there is no need for a translation layer or bridge: extensions can be included directly into LDPL programs and manipulate, share, and access subprocedures and variables natively. All that's needed is a few naming conventions on the C++ side and the use of the `external` syntax for variables and subprocedures on the LDPL side. External variable and sub-procedure naming conventions are explained in detail in the **Naming Schemes** section of this documentation.

Extensions contain sub-procedures and variables that are considered to be _external_.
This means that they are not part of an LDPL source code and thus must be accessed and called in a different way to what you might be used to. **External sub-procedures** should be called using the the `call external` statement. While it is explained in greater detail in the **Control Flow Statements** section of this documentation, the statement works just like the normal `call` statement with the exception that it doesn't accept parameters.

```c++
// In C++
void CPPFUNCTION(){
    //...
}
```

```coffeescript
# In LDPL
call external cppFunction
```

**External variables** (declared in a C++ file) should be **re-declared** again in your LDPL data section with the same type they have in the C++ file, appending to their type the `extenal` keyword. This allows programmers to extend LDPL with new features or to wrap 3rd party libraries and re-use their functionality.

```c++
// In C++
ldpl_number MYNUMBER = 9;
```

```coffeescript
# In LDPL
data:
myNumber is external number
```

This will be explained in greater detail in the following sections.

## Writing C++ Extensions
    
If LDPL lacks some feature that C++ might offer, for example graphics or networking,
you might find yourself in the need of writing a C++ extension (provided an extension
for what you are looking for has not been already written, of course).

Extensions can create variables and functions that are accessible from LDPL through the `call external` and `external` data type keyword, as explained in the previous section. Typically all you need is a single `.cpp` file that you include from your LDPL source using the `extension` keyword (explained in the **Code Structure** section of this documentation), but you may also include `.o` files, `.a` files, or any combination of them all.

### Functions

To create a function in C++ that can be called from an LDPL program, you must follow four rules:

1. The function type must be `void(void)`, ex: `void MY_FUNC();`
2. The function name must conform to LDPL's external identifier naming conventions explained in the **Naming Schemes** section of this documentation.
3. The function must not take any parameters.
4. The function must not return any values.

Because LDPL does not _know_ the name of any variables or functions declared in non-LDPL files, it allows the programmer to call any function or variable, existing or not, by using the `external` syntax. If the variable or function you are trying to access does not exist, the C++ linker will throw a nasty error. Also, all C++ variable and function names must contain only `A-Z`, `0-9`, and the `_` character. Every character on the LDPL side will be converted to upper case, and non alpha-numeric characters will be converted to an underscore \(`_`\) when referencing the C++ side (again, as stated in the **Naming Schemes** section of this documentation).

**Example:**

For example, this function in a file called **add.cpp**

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

can be accessed from LDPL in the following way

```coffeescript
external "add.cpp"

procedure:
    call external prompt_add
```

### Variables

To create a variable in a C++ extension that can be accessed from LDPL code, you must follow two rules:

1. The variable's name must conform to the LDPL external identifier naming convention stated in the **Naming Schemes** section of this documentation.
2. The C++ type of the variable must match the type used by LDPL for the data type represented by that variable.

The first rule should be familiar from the previous section: all C++ variable and function names must contain only `A-Z`, `0-9`, and the `_` character. Everything else on the LDPL side will get converted to an underscore \(`_`\).

For the second rule, you may find a file called **ldpl_types.h** in the LDPL repository that contains definitions for all the build-in LDPL data types: `ldpl_number`, `ldpl_text`, `ldpl_list<T>` and `ldpl_map<T>`.

**Example:**

Declaring variables is easy on the C++ side:

```cpp
ldpl_text NAME;
ldpl_number AGE;
ldpl_text STREET_ADDRESS;
```

These will be available to an LDPL program when declared as external in its `data:` section:

```coffeescript
data:
    name is external text
    age is external number
    street_address is external text
```

### Accessing Variables in C++ Functions

Since LDPL and C++ are using the same variable when you use the `external` keyword (say, for example, `MY_VAR` in C++ and external `my-var` in LDPL), any changes you make to the content of said variables are shared. Use them just like you would use any regular variable, both in C++ and LDPL.

```cpp
ldpl_number A, B, SUM;
void ADD() 
{
    SUM = A + B;
}
```

```coffeescript
data:
    a is external number
    b is external number
    sum is external number

procedure:
    store 100 in a
    store 250 in b
    call external add
    display sum lf
```

Building and running this program will print `350`.

## Building C++ Extensions

Extensions are easy to build: when compiling your LDPL program, use the `extension` keyword (explained in detail in the **Code Structure** section of this documentation) to add `.cpp` files, `.o` files, or `.a` files to your LDPL project. They will be included in your program and become available using the `external` statements.

If your C++ extension files require extra flags to be passed to the C++ compiler in order to compile \(for example, `-lSDL` when working with SDL\) you can use the `flag` statement (explained in detail in the **Code Structure** section of this documentation) to pass flags to the C++ compiler.

    :::coffeescript
    external "otherFile.cpp"
    flag "-fpermisive"
    flag "-lSDL2"
    data:
        #...
    procedure:
        #...

## "Hello World" C++ Example

File **simple.cpp**:
```cpp
#include <iostream>
void SIMPLE(){
        std::cout << "Very simple!" << std::endl;
}
```

File **simple.ldpl**:
```coffeescript
extension "simple.cpp"

procedure:
call external simple

```

Console:

```bash
$ ldpl simple.ldpl
LDPL: Compiling...
* File(s) compiled successfully.
* Saved as simple-bin
$ ./simple-bin
Very simple!
```

## External Sub-procedures

Sometimes when writting C++ Extensions you'll find yourself in the need of declaring a function in C++ but coding it in LDPL. This is the opposite of writing C++ functions and calling them from LDPL, it's writing LDPL sub-procedures and calling them from C++.

These C++ calleable sub-procedures are called **external sub-procedures**, as they can be called from an external medium.

In order to declare an external sub-procedure you must first forward-declare it in your C++ source code. Say, for example, that you want to declare a sub-procedure called `helloWorld`. In your C++ you should write the following line:

```cpp
void HELLOWORLD();
```

Note that external sub-procedures **cannot receive any kind of parameters** and must be declared as `void`. You may then call the external sub-procedure from C++ code like this:

```cpp
int myCPPFunction(){
  HELLOWORLD();
  return 1;
}
```

Once that's taken care of, you can declare your external sub-procedure as any other sub-procedure in LDPL by prepending the identifier `external` to the sub-procedure declaration:

```coffeescript
external sub-procedure myExternalSub
    #...
end sub-procedure
```

or just

```coffeescript
external sub myExternalSub
    #...
end sub
```

These sub-procedures can be called from LDPL like you would call any other sub-procedure, but their names must follow the external identifier naming scheme detailed in the **Naming Schemes** section of this documentation as any other C++ interfacing identifier.


## External Variables

Variables defined in extensions can be accessed by prefacing their data type declaration with the `external` keyword. This must occur in the **data** section of your LDPL code. Once an external variable is declared, it can be used just like any other LDPL variable.

**Syntax:**

```text
<variable> is external <data type>
```

**Example:**

```coffeescript
data:
    rl-prompt is external text
    window.size is external number
```


