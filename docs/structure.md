# LDPL Source Code Structure

## Case Sensitivity

LDPL is a **case-insensitive** language. This means that variables and statements
that consist of the same characters are understood to be the same variable
or statement regardless of their case. For example, in other languages the
variables `foo` and `FOO` would represent two different variables, but in
LDPL they are **the same one**. This same thing happens with statements. For
LDPL it's the same if you write `display` or `dIsPlAy` (but please don't do so).

## Comments
Comments in LDPL are denoted with a hash symbol ('#') and can be placed both
on their own line or at the end of a line that already contains a statement.
Everything after the hash is considered to be part of the comment and,
therefore, not executed.

    :::coffeescript
    data:
        # This won't be executed
        # This won't be either
    procedure:
        # This line won't be interpreted

## Data and Procedure Sections
LDPL was designed to be a rigidly structured programming
language and, as such, variable declarations and the remaining code procedures
are separated into two different, mutually exclusive sections within every
source file: the `data:` section and the `procedure:` section.

    :::coffeescript
    data:
        # Variables go here
    procedure:
        # The rest of the code goes here

Variable declarations should be placed within the **data** section, while the
rest of the code should be placed inside the **procedure** section. Further
sub-procedures should be placed also within the **procedure** section, inside their
own **sub-procedure** subsections.

The **data** section may be obviated if no variables are used.

If your project consists of multiple LDPL source files, each file must have its
own data and procedure sections.

### The Data Section

The **data** section is where global variables are declared (you can use them
anywhere in your program). If no variables are declared, the data section can
be skipped altogether.

The data section is defined and preceded by the `data:` keyword.
An empty data section looks like this:

    :::coffeescript
    data:
    
On every line within the data section (that is, on every line after the `data:`
keyword and before the `procedure` keyword) one and only one variable can be
declared.

The syntax for declaring a variable in LDPL is:

    :::coffeescript
    variable-name is data-type
    
**Variable naming schemes** and **data types** will be explained later on this
document.

A data section cannot contain anything but variable declarations, comments and
empty lines. An example data section may end up looking like this:

    :::coffeescript
    data: # this is an example data section
        foo is number
        bar is text map
        
        # foobar is my number list
        foobar is number list

### The Procedure Section

The **procedure** section is where all the code of a LDPL program that is not a
variable declaration is written. An LDPL program **must** contain a procedure
section, even if it's empty. Execution should and will fail otherwise.

Within the procedure section, every line can contain either a comment,
a statement, a statement and a comment or be empty. No two statements can be
written on the same line.

An example procedure section may end up looking like this:

    :::coffeescript
    procedure:
        store 5 in foo
        store "hi there" in bar:"hi"
        # Note that these are the variables
        # declared in the data section above.

Code within the procedure section is executed from top to bottom,
skipping sub-procedure declarations, unless they are explicitly called.

Available statements and sub-procedure declarations will be explained further
in the following sections of this document.

## Variable Data Types

LDPL natively supports the scalar **number** and **text** data types. It also
supports containers of said scalar types: **maps** and **lists**, combined in
any way.

Variables may be declared, as stated above, using the syntax:

    :::coffeescript
    variable-name is data-type
    
within the data section. The name of a data type is composed of either a scalar
data type name (**number** or **text**) or a scalar data type plus one or more
container type names. For example:

    :::coffeescript
    foo is number
    bar is text
    foobar is text list map
   
In the case above, **foobar** is a **map of lists of text values**.

### The Number Data Type

The **number** data type, as its name suggests, depicts numeric values.
It's recommended that it be represented internally as a binary64
double-precision floating-point format number as defined by the IEEE 754.

Both variables and numeric constants can be members of the number type.

Valid number literals must begin with a decimal value (for example 5 or 0.12,
.12 wouldn't be a valid number) and may be preceded by a minus sign for
negative numbers (-5, -567.912). Numbers may not be preceded by a plus sign
(+5 is not a valid number literal). The literal -0 is implicitly transformed
into 0.

    :::coffeescript
    -231897.123 # is an example number

### The Text Data Type

The **text** data type, as its name suggests, represents alphanumeric strings.
In the interest of supporting as many locales as possible, texts should be utf-8
encoded to be compatible with Unicode. A text maximum length for text values is
explicitly not defined and it should be limited only by the amount of available
memory on the system. Strings in LDPL are enclosed between two "quotes".

    :::coffeescript
    "This is an example string!"
    
LDPL strings may contain multiple escape sequences / control characters in
them. Each escape sequence counts as only one character. The available escape
sequences are:

 * `\a` = alert (bell)
 * `\b` = backspace
 * `\t` = horizontal tab
 * `\n` = newline / line feed
 * `\v` = vertical tab
 * `\f` = form feed
 * `\r` = carriage return
 * `\e` = non-standard GCC escape
 * `\0` = null byte
 * `\\` = \ character
 * `\"` = " character
    
For example, the string `"hello,\nworld"` will be displayed as

    :::text
    hello,
    world
    
when printed to the console.
    
### The List Data Type

The **list** data type is a collection of number or text values, or containers
of possibly more containers of said types. Values can be pushed to lists and
then be accessed and modified using the `:` operator. List indexes consist of
integer numbers. The first index of a list is index 0, and the rest count up to
the length of the list minus one.

Lists, as collections of number or text values (or collections of said types),
can only have one defined type at any given time: text or number. A single list
is not capable of storing both numeric and alphanumeric values.

    :::coffeescript
    data:
        foo is number list # This is a list
    
    procedure:
        push 10 to foo
        display foo:0 lf
        
!!!note
    The `display` statement prints values to the screen. While it will be
    explained in more detail later, the line 
    
        :::python
        `display foo:0 lf`
        
    prints
    the value of `foo:0` (the index `0` of the list `foo`) followed by
    a line break (`lf`).
    
In most other languages, you may have to declare a list and specify how many
elements or components it contains. In such languages, the declaration causes a
contiguous block of memory to be allocated for that many elements. LDPL lists,
however, are different: they are dynamic. This means that you can store as many
values as you want in a single list without fear of running out of place (of
course, this is limited by the memory allocated by your operating system for
your LDPL program).

Suppose you store the values `"hi"`, `"there"`, `"I love"` and
`"LDPL, it's great!"` in a `text list`, in that particular order. Then the
contents of the list and the **index** associated with each **element** will be

| Index | Element |
| :---: | :---: |
| 0 | `"hi"` |
| 1 | `"there"` |
| 2 | `"I love"` |
| 3 | `"LDPL, it's great!"` |

We have shown the pairs in order because their order is relevant: if you added
a new element to the list, it would be inserted after the last element, thus
being associated with index `4`.

To add values to a list, you must first push them to the list. For example,
if you want to add the numbers `10`, `20` and `30` to a `number list`, your
code should look like this:

    :::coffeescript
    data:
      myList is number list
    procedure:
      push 10 to myList # 10 is stored in index 0 of myList
      push 20 to myList # 20 is stored in index 1 of myList
      push 30 to myList # 30 is stored in index 2 of myList


Values in lists can be stored and accessed just like any other variable
(see the **store - in** statement for further details) using the `:` operator.
This operator indicates what index of the list we are writing to or reading
from. Here we declare a `number list` and store the values `5` and `-10.2` in
it, and then replace the number `5` by the number `890`:

    :::coffeescript
    data:
      myList is number list
    procedure:
      push 5 to myList
      # 5 is stored in index 0 of myList
      push -10.2 to myList
      # -10.2 is stored in index 1 of myList
      store 890 in myList:0
      # We store 890 in index 0 of myList, thus replacing the 5
      
!!!note
    The `push` statement adds a value at the end of a list. Also, the `store 
    - in` statement stores a value in a variable. These two statements will
    be explained in more detail in their respective sections of this document.

Please note that as a list is **variable** that's a collection of values, a
single index of a list is a variable in itself. This means that any subindex
of a list that resolves to a scalar value be used in any position where you
could use a variable of the same type of that value. So, if you have something
like this:

```coffeescript
store <number-variable or number> in <number-variable>
```

You could use a `number list` with a defined sub-index \(for example, in the
example above, `myList:0`\) where it says number-var, just like in
the `store - in` examples in the code extracts above.

In the **list statements** section you'll find a collection of statements that
can be used to work with lists.
        
### The Map Data Type

The **map** data type is a collection of number or text values. Maps
superficially resemble lists but with fundamental differences. The biggest one
is that any number or string in LDPL may be used as an array index, not just
consecutive integers. Also, values in maps have no order.

    :::python
    data:
        foo is number map # This is a map
    
    procedure:
        store 19 in foo:"hi there!"
        display foo:"hi there!" lf

Maps, as collections of number or text values (or collections of said types),
can only have one defined type at any given time: text or number. A single map
is not capable of storing both numeric and alphanumeric values.

Unlike lists, maps are **associative**. This means that each map is a
collection of pairs: a key and its corresponding element. For example,
you could have a `number map` with the following contents:

| Key | Element |
| :---: | :---: |
| 4 | 30 |
| 2 | 10 |
| "Hi there!" | -56.3 |
| "99ldplrocks89" | 0 |

We have shown the pairs in jumbled order because their order is irrelevant.
One advantage of maps is that new pairs can be added at any time. maps can be
sparse: they can have missing keys \(say for example you have keys 1 and 5, but
don't have keys 2, 3 and 4\). Another consequence of maps is that the keys
don't necessarily have to be positive integers. Any number, or even a string,
can be a key.

Values in maps can be stored and accessed just like any other variable
(see the **store - in** statement for further details) using the `:` operator.
This operator indicates what key of the map we are writing to or reading from.
Here we declare a `number map` and store the values `5` and `-10.2` in the
keys `1` and `5`, respectively.

    :::coffeescript
    data:
      myMap is number map
    procedure:
      store 5 in myMap:1 #Stores 5 in key 1 of myMap
      store -10.2 in myMap:5 #Stores -10.2 in key 5 of myMap

As stated before, map keys don't always have to be constant numbers.
They can also be number variables, text and text variables, or even sub-indexes
of lists or elements from other maps. For example:

    :::coffeescript
    data:
      myMap is number map
      myOtherMap is number map
      myVar is number

    procedure:
      store 17 in myVar
      store 1 in myMap:"hello"
      #Stores 1 in key "hello" of myMap
      store 7 in myMap:myVar
      #Stores 7 in a key equal to the current value of myVar
      store 3 in myMap:myOtherMap:4
      #Stores 3 in a key of equal value to the key of myMap with value equal to
      #key 4 of myOtherMap

In fact, when you use a number value as a subindex for a map, it is silently
casted into a text value. For example, `myMap:1` will be interpreted \(an thus,
the same\) as `myMap:"1"`.

Please note that as a map is **variable** that's a collection of values,
a single key of a map is a variable in itself. This means that any key of a
map that resolves to a scalar value can be used in any position where you could
use a variable of the same type of that value. So, if you have something like
this:

```coffeescript
store <number-var or number> in <number-var>
```

You could use a `number map` with a particular key where it says number-var,
just like in the `store - in` examples in the code extracts above
\(for example `myMap:"hello"`\).

As you'll see in the **Default Variable Values** section, you can access
undeclared keys of a map just like if they were declared.
See the following example:

```coffeescript
data:
  myMap is number map
procedure:
  display myMap:99
```

In the example above, `0` will be printed and no errors displayed during
compilation, even though the key `99` of `myMap` hasn't been explicitly
declared. This is because when you try to access an element that hasn't
been declared yet, LDPL declares it for you and initializes it to its type
default value.

It's important to note that this very feature is a double-edged weapon. While
you can use it to access uninitialized map keys, you cannot check if a value
exists in a map without initializing it if it wasn't there before. Statements
like **store key count of** and **store keys of** are provided as means to
overcome this situation.

In the **map statements** section you'll find a collection of statements that
can be used to work with maps.

!!!warning
    In older versions of LDPL, **map**s were called **vector**s. 
    Starting from LDPL 3.1.0 Diligent Dreadnoughtus, they have been renamed
    to reflect the real data structure they represent. While it might still be
    possible to call them vectors in code, and legacy code that declares maps
    as vectors is and will continue be supported, this nomenclature is
    **deprecated** and shouldn't be used anymore.
    
## Multicontainers

As stated before, _"The name of a data type is composed of either a scalar
data type name (**number** or **text**) or a scalar data type plus one or more
container type names."_ This means that you can declare variables like:

    :::coffeescript
    myVariable is text list list
    myOtherVariable is number map list map map list map
    pleaseStop is text list list map list list map map map list map map list

These variables are called **multicontainers**. Multicontainers should be read
from right to left in order to understand how are they set up. For example,
a `text list map` is a **map** that holds **lists** of **text** values. You
may access a value stored in a `text list map` like:

    :::coffeescript
    display foo:"hi":0
    
where `hi` is the key used to access a list stored in the map, and `0` the
index used to access a text value stored in the list that was stored in the
map.

If you want to push a list to a `list list` or a map to a `map list` you must
use the **push list to -** and **push map to -** statements, respectively:

    :::coffeescript
    data:
        listOfMaps is text map list
        listOfLists is text list list
    procedure:
        push map to listOfMaps
        store "Hello!" in toListOfLists:0:"hi!"
        # Pushes a map to listOfMaps and then stores
        # the value "Hello!" in the key "hi!" of the pushed map
        push list to listOfLists
        push "Hello!" toListOfLists:0
        # Pushes a map to listOfLists and then pushes
        # the value "Hello!" at index 0 of the pushed list

These will be explained in more detail in their respective sections.

## Default Type Values

In LDPL each variable is initialized with a value by default. This means
that when you declare a variable, it will, by default, hold this value
until it's changed.

 * **Number** variables are initialized with the value `0`. Each element of
 a **number map** is a number variable, and thus also initialized to `0`.
 * **Text** variables are initialized to the empty string `""`. The same goes
 for **text maps**, where each element it contains is also initialized to `""`.
 * **Lists** are initialized empty by default and trying to access a
 non-existing index will result in an error.
 * Keys of **Maps of Lists** (`list map`) are declared as empty lists of the
 scalar type of the multicontainer by default.
 
## Predeclared Variables

Some variables in LDPL are already declared for you without you having to
declare them. These variables are the **argv** text list, and the **errorcode**
and **errortext** variables.

### The *argv* list variable

Every LDPL program comes with the `argv` **text list** variable declared by
default.

If you pass command line arguments to your LDPL compiled program \(running, for
example, something like `myBinary argument1 argument2)`, the values stored
in the `argv` list \(_argument vector_\) will be the values of each argument
passed \(in this case, `"argument1"` will be stored in `argv:0` and
`"argument2"` in `argv:1`\).

!!!hint
    Given that `argv` is a **text list**, the values passed as arguments are
    always stored as **text**, even numbers.

Naturally, if no arguments are passed to the program, the `argv` list will be
empty.

### The *errorcode* and *errortext* text variables

Some LDPL operations may fail when executed. Maybe you tried loading a file
that wasn't there or getting the ASCII value of a multi-byte emoji. These
operations make use of the `errorcode` and `errortext` variables to
tell you if they ran successfully or not.

The `errorcode` and `errortext` variables come declared by default.
Some statements may modify their values to express their results.

The `errorcode` variable is a **number** variable. It will hold the value 0
if the statement ran successfully and any other number if it did not.

The `errortext` variable is a **text** variable that will be empty if the
statement ran successfully. If it did not, it will store a human readable
description of what went wrong.

The `errorcode` and `errortext` variables can be read and written like any other
LDPL variable.

!!!warning
    When handling error checks, please bear in mind that the content of the
    `errortext` variable may change in future releases of LDPL. The value
    stored in `errorcode`, however, will not change and so that's the value
    that should be used to check whether an operation ran successfully or not.

## Identifier Naming Schemes

Variables and sub-procedure names follow the same naming rules. Their names
can't be empty and may consist of any character with few exceptions \(listed
below\). Like statements, variable and sub-procedure names in LDPL are not case
sensitive.

* Variable and sub-procedure names cannot contain the character `:`, as it is
used for **map** and **list** accesses.
* Variable and sub-procedure names cannot contain the character `"`, as it is
used to delimit strings.
* Variable and sub-procedure names cannot contain spaces.
* Variable and sub-procedure names cannot be valid numbers \(they may contain
numbers, though\).
* Variable and sub-procedure names cannot contain the character `(` nor the
character `)` as these characters are used in mathematical expressions.
* Variables and sub-procedures cannot be called `CRLF`, as it is internally
turned into `"\r\n"`.
* Variables and sub-procedures cannot be called `LF`, as it is internally
turned into `"\r\n"`.
* Variables and sub-procedures cannot be called `+` nor `-` nor `*` nor `/` as
these characters are used in mathematical expressions.


!!!warning
    External Identifiers follow different naming rules. Please check the
    **External Identifier Naming Scheme** section for more information.

## LABEL naming scheme

Labels in LDPL can't be empty and may contain any character except spaces and `"`. LABELs can't be named `CRLF` nor `LF` for the same reasons explained in the section above.

## External Identifier Naming Scheme

Please refer to [this page](extensions/c++-extensions/external-identifier-naming-scheme.md).

## Declaring Functions

A **function** is a section within a source code that can be called and executed
from other parts of the script and that returns a value to its caller. Function
subsections must be declared within the procedure section of your code using
the `fun <function-name>` statement and end with an `end fun` statement.

You cannot declare a function within a function.

As functions are declared once execution reaches them, you cannot invoque a
function before it has been declared.

The full syntax for declaring a function is:

    fun <function-name> is <return-type>
        parameters:
            # Parameters go here
        data:
            # Local variable declarations go here
        procedure:
            # Code goes here
    end fun

The **parameters** and local **data** sub-sections are optional (more on these
later). If you decide to not include any of those sub-sections, you can skip
the **procedure** tag and just go ahead writing your code like this:

    fun myFunction is nothing
        display "Hello there!" lf
    end fun
    
In context, the full declaration of a function looks like this:

    data:
        # ...
    procedure:
        # ...
        fun <function-name> is <return-type>
            parameters:
                # Parameters go here
            data:
                # Local variable declarations go here
            procedure:
                # Code goes here
        end fun

You cannot declare two functions with the same name. Also, unctions must follow
the same naming guidelines variable names follow (explained below).

### The Parameters and Data Sub-Sections

Within the **parameters** and **data** sub-sections of a function you may only
declare variables just like in the global **data** section. The variables
defined here, however, can only be used inside the procedure sub-saction of the
same function they where declared in.

Variables between the parameters and local data sub-sections may not share
names.

If a variable declared within the parameters or data sub-sections of
a function has the same name of a global variable, when using that name in the
procedure section of the function, it will refer to the variable declared in
said function.

At the start of the function execution, all the variables declared in its
**data** section will be initialized with the default value of their type, and
each invocation of the function will have its own copy of the local variables.

Variables declared within the **parameters** section will be initialized to the
values passed when the function was called. LDPL is pass-by-copy for scalar
values but it passes container types by reference.

    fun printName is nothing
        parameters:
            name is text
        procedure:
            display "Hello there, " name "!" lf
    end fun

###

## Variable and Function Naming Guidelines

## Including More Source Files
