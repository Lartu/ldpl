## The Data Section

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
        
## Variable Data Types

LDPL natively supports the scalar **number** and **text** data types. It also
supports containers of said scalar types: **maps** and **lists**, combined in
any way.

Variables may be declared, as stated above, using the syntax:

    :::coffeescript
    variable-name is data-type
    
within the data section. The name of a data type is composed of either a scalar
data type name (**number** or **text**) or a container type plus one or more
scalar or container type names. For example:

    :::coffeescript
    foo is number
    bar is text
    foobar is map of lists of text
   
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

In the **list statements** section, you'll find a collection of statements that
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
undeclared keys of a map, just like if they were declared.
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

In the **map statements** section, you'll find a collection of statements that
can be used to work with maps.

!!!warning
    In older versions of LDPL, **map**s were called **vector**s. 
    Starting from LDPL 3.1.0 Diligent Dreadnoughtus, they have been renamed
    to reflect the real data structure they represent. While it might still be
    possible to call them vectors in code, and legacy code that declares maps
    as vectors is and will continue to be supported, this nomenclature is
    **deprecated** and shouldn't be used anymore.
    
## Multicontainers

As stated before, _"The name of a data type is composed of either a scalar
data type name (**number** or **text**) or a container type plus one or more
scalar or container type names."_ This means that you can declare variables like:

    :::coffeescript
    myVariable is list of lists of text
    myOtherVariable is map of lists of maps of maps of lists of maps of numbers
    pleaseStop is list of maps of maps of lists of maps of maps of maps of lists of lists of maps of lists of lists of text

These variables are called **multicontainers**. Multicontainers hold other 
containers, which can hold more containers or scalar values to an arbitrary
depth. For example, a `map of lists of text` is a **map** that holds **lists** 
of **text** values. You may access a value stored in a `map of lists of text` 
like so:

    :::coffeescript
    display foo:"hi":0
    
where `hi` is the key used to access a list stored in the map, and `0` the
index used to access a text value stored in the list that was stored in the
map.

If you want to push a list to a `list of lists` or a map to a `list of maps` you 
must use the `push list to _` and `push map to _` statements, respectively:

    :::coffeescript
    data:
        listOfMaps is list of maps of texts
        listOfLists is list of lists of texts
    procedure:
        push map to listOfMaps
        store "Hello!" in listOfMaps:0:"hi!"
        # Pushes a map to listOfMaps and then stores
        # the value "Hello!" in the key "hi!" of the pushed map
        push list to listOfLists
        push "Hello!" listOfLists:0
        # Pushes a map to listOfLists and then pushes
        # the value "Hello!" at index 0 of the pushed list

These will be explained in more detail in their respective sections.

The LDPL compiler supports plural data-type names, as shown above. One can
use `numbers` instead of `number`, `texts` instead of `text`, 
`lists` instead of `lists` and `maps`
instead of `map`. A `list of list of number` is the same as a `list of lists of numbers`.

!!!Note
    In LDPL 4.3, multicontainers were introduced with a right-to-left syntax.
    A `map of list of text` was defined as `text list map`. This syntax is
    still supported.

## Default Type Values

In LDPL, each variable is initialized with a value by default. This means
that when you declare a variable, it will, by default, hold this value
until it's changed.

 * **Number** variables are initialized with the value `0`. Each element of
 a **number map** is a number variable, and thus also initialized to `0`.
 * **Text** variables are initialized to the empty string `""`. The same goes
 for **text maps**, where each element it contains is also initialized to `""`.
 * **Lists** are initialized empty by default and trying to access a
 non-existing index will result in an error.
 * Keys of **Maps of Lists** (`map of list`) are declared as empty lists of the
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

### The *errorcode* and *errortext* variables

Some LDPL operations may fail when executed. Maybe you tried loading a file
that wasn't there or getting the ASCII value of a multi-byte emoji. These
operations make use of the `errorcode` and `errortext` variables to
tell you if they ran successfully or not.

The `errorcode` and `errortext` variables come declared by default.
Some statements may modify their values to express their results.

The `errorcode` variable is a **number** variable. It will hold the value 0
if the statement ran successfully, and any other number if it did not.

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
