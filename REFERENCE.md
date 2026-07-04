# LDPL Language Reference

**LDPL** (Lartu's Definitive Programming Language) is a case‑insensitive, compiled programming language with a plain‑English syntax. An LDPL source file is compiled into C++ and then into a native executable using the `ldpl` compiler, yielding fast, portable programs. Source files normally use the `.ldpl` extension, though compiled source files may use `.lsc`. This guide describes the syntax and semantics of LDPL.

## Compiling LDPL Programs

To run LDPL code you must first install the LDPL compiler and have a C++ compiler (invoked as `c++`) available in your environment. The simplest way to build a program is:

```
ldpl source.ldpl
```

This command compiles `source.ldpl` and produces an executable named `source-bin` in the same directory. Use the following options to control compilation:

- `-o=<name>` – sets the name of the output binary. For example, `ldpl -o=hello hello.ldpl` compiles `hello.ldpl` and creates an executable called `hello`.
- `-i=<file>` – adds extra files to the compilation. These may be additional LDPL source files or compiled objects (`.o`), static libraries (`.a`) or C++ files. The `INCLUDE` statement is generally preferred for LDPL sources.
- `-f=<flag>` – passes an extra option to the C++ compiler. For instance, `-f=-lSDL2` links against the SDL2 library. You can specify this multiple times. In most cases it is preferable to use the `FLAG` statement inside your LDPL code to supply linker flags.
- `-r` – compiles the LDPL source and prints the generated C++ code to standard output. This does not produce a binary.
- `-ns` – on Linux targets LDPL builds static binaries by default. Use `-ns` to generate a dynamically linked (non‑static) binary.
- `-n` – suppresses the “compile successfully” message emitted at the end of compilation.
- `-c` – read LDPL source code from the standard input instead of from a file.
- `-v` or `--version` – prints the compiler version and exits.
- `-h` or `--help` – displays a summary of available compiler options.

## Program Structure

An LDPL program is organised into declarative and executable sections. Only one *DATA* and one *PROCEDURE* section may appear in a program. Text after a `#` on a line is a comment and is ignored by the compiler.

- **Data section:** Begins with `DATA:` or `-- DATA --`. Each line declares one variable and its type. External variables may be marked with the keyword `EXTERNAL` (see “C++ Extensions”).
- **Procedure section:** Begins with `PROCEDURE` or `-- PROCEDURE --`. Statements in this section execute in order. Code appearing before any sub‑procedure executes at program start. You may define sub‑procedures within this section.
- **Sub‑procedures:** A sub‑procedure is defined using `SUB‑PROCEDURE Name` (or the short form `SUB Name`) and ends with `END SUB‑PROCEDURE` (or `END SUB`). Each sub‑procedure may optionally contain a `PARAMETERS:` section listing parameter names and a `LOCAL DATA:` section declaring local variables; you may also use the alternate header forms `-- PARAMETERS --` and `-- LOCAL DATA --`. Sub‑procedures cannot be nested inside other sub‑procedures or within control flow blocks.
- **Include files:** Use `INCLUDE "file.ldpl"` to copy the contents of another LDPL file into the current one. The `include` statement must appear before the *DATA* section of the including file. Each LDPL source file must declare its own *DATA* and *PROCEDURE* sections (or their synonyms `-- DATA --` and `-- PROCEDURE --`). The *DATA* section may be omitted if no variables are declared.
- **C++ extensions:** The `EXTENSION "file.cpp"` statement (see “C++ Extensions”) includes C++ source files before compilation. Use `FLAG "-lSDL2"` to pass additional flags to the C++ compiler; you may prefix a flag with an operating system name (e.g., `FLAG windows "-lSDL2"`) to apply it only on that platform.

## Data Types and Variables

LDPL variables are declared in the data or local data sections with the syntax `Name is Type`. Names are case‑insensitive and may not contain spaces, colons (`:`), double quotes, parentheses, plus or minus signs, or be valid numbers. The compiler accepts both singular and plural type names—`number` and `numbers` are interchangeable, as are `text` and `texts`, `list` and `lists`, and `map` and `maps`. Nested types are written from the outermost container inward (e.g., a `list of map of text` is a list whose elements are maps from text to text). An older right‑to‑left syntax (for example, `text list map`) is still supported but the left‑to‑right form is recommended. Only one variable may be declared per line. The primary types are:

- **Number:** Arbitrary‑precision integer and floating‑point numeric type. A number variable starts at `0`.
- **Text:** UTF‑8 encoded string. A text variable starts as an empty string.
- **List:** Dynamic, ordered container. Declare a list as `list of number`, `list of text`, or nested containers such as `list of list of number` and `list of map of text`. A list begins empty.
- **Map:** Associative container (dictionary) mapping keys to values. Declare a map as `map of number`, `map of text`, or nested forms. Map keys may be numbers or text.

Nested types are written from the outermost container inward (e.g., a `list of map of text` is a list whose elements are maps from text to text). Only one variable may be declared per line. Collections are initially empty. LDPL provides the following predefined variables:

- `argv` – a list containing command‑line arguments.
- `errorcode` (number) – stores the result of file and system operations; `0` indicates success.
- `errortext` (text) – stores a human‑readable description of the last system error.
- `crlf` and `lf` – constants representing newline sequences (carriage‑return+line‑feed or line‑feed only).

## Sub‑procedures and Calls

A sub‑procedure encapsulates reusable code. Declare a sub‑procedure with `SUB‑PROCEDURE Name` and end it with `END SUB‑PROCEDURE`. After declaring the sub‑procedure name, you may include a `PARAMETERS:` section listing names of parameters and a `LOCAL DATA:` section for local variables. Parameters are passed by reference, so changes to parameters inside the sub‑procedure affect the caller’s variables.

The `PARAMETERS` and `LOCAL DATA` sections are optional. Parameter names and local variable names must be unique: you cannot declare the same name twice within a sub‑procedure. Variables declared in a sub‑procedure shadow global variables of the same name; within the sub‑procedure, all references to that name use the local variable. Each call to a sub‑procedure receives its own copy of the local variables, allowing recursion. You may call a sub‑procedure before it is defined, but every sub‑procedure used must be defined somewhere in your program before compilation finishes. A sub‑procedure that declares parameters must be called with `WITH` followed by the same number of arguments in the same order; a sub‑procedure with no parameters must be called without `WITH`.

Call a sub‑procedure using `CALL Name` or `CALL SUB‑PROCEDURE Name`. If the sub‑procedure expects parameters, append `WITH` followed by variables to pass. For example:

```
call addValues with a b sum
```

This call passes variables `a`, `b` and `sum` by reference to the sub‑procedure `addValues`. LDPL checks that parameter types match. The `RETURN` statement exits the current sub‑procedure. Use `EXIT` to terminate the entire program immediately.

To call a C++ function defined in an extension, use `CALL EXTERNAL FunctionName`. The name must follow the external naming rules described in the C++ Extensions section.

## Control Flow

LDPL provides several statements for controlling program execution.

### Assignment

- `STORE <expression> IN <variable>` – evaluates an expression and assigns the result to a variable. Expressions may be numeric or text. The reverse form `IN <variable> STORE <expression>` is equivalent.

### Conditional Statements

- `IF <condition> THEN` … `END IF` – execute the enclosed statements when the condition is true. Conditions may compare numbers or text using `=` (equal), `<>` (not equal), `>`, `>=`, `<` and `<=`. A membership condition uses the `IN` operator: if the right‑hand side is a list, `a IN b` is true when the scalar value `a` is contained in the list; if the right‑hand side is a map, `a IN b` tests whether `a` is one of the map’s keys. Conditions may be combined using `AND`, `OR` and parentheses.
- `ELSE IF <condition> THEN` – optional; tested if previous `IF` or `ELSE IF` blocks did not execute.
- `ELSE` – optional; executed when no preceding condition was true.
- `END IF` – ends an `IF` block.

### Loops

- **While loop:** `WHILE <condition> DO` … `REPEAT` executes the body while the condition remains true. Use `FOREVER DO` … `REPEAT` for an infinite loop.
- **For loop:** `FOR <numberVar> FROM <start> TO <end> DO` … `REPEAT` iterates the number variable from the start value to the end value, incrementing by 1 each time. Append `STEP <step>` to specify a custom increment (positive or negative). Add the word `INCLUSIVE` after `TO <end>` if the loop should include the end value. The start, end and step expressions are evaluated once at loop start.
- **For‑each loop:** `FOR EACH <itemVar> IN <collection> DO` … `REPEAT` iterates over every element in a list or every key in a map. When iterating over a map, the iteration variable must be of type `text` and will receive each key.
- **BREAK:** exits the nearest enclosing `WHILE`, `FOR` or `FOR EACH` loop. Using `BREAK` outside a loop is a compile‑time error.
- **CONTINUE:** skips the remainder of the current iteration and proceeds to the next iteration of the nearest loop. Using `CONTINUE` outside a loop is a compile‑time error.

### Labels and Goto

- `LABEL <name>` – defines a target label. Labels may appear inside the procedure section or within sub‑procedures.
- `GOTO <name>` – jumps to the specified label. Goto may only jump within the same sub‑procedure or the main program body.
- `EXIT` – terminates the entire program.

### Custom Statements

You can define your own statement syntax using `CREATE STATEMENT "pattern" EXECUTING <subprocedure>`. The *pattern* is a quoted string that may contain `$` symbols as placeholders. Each placeholder corresponds to a parameter of the sub‑procedure. When the compiler encounters the pattern in your code, it calls the specified sub‑procedure with the appropriate arguments.

## Parallel Execution and Concurrency

LDPL supports basic multithreading and mutual exclusion.

- `CALL PARALLEL <SubName>` – starts the named sub‑procedure in a new detached thread. Parallel calls cannot accept parameters and return immediately. Use parallel calls to run background tasks while the main thread continues.
- `AWAIT LOCK <LockName>` – acquires a named mutex. If another thread holds the same lock, the statement blocks until the lock becomes available.
- `UNLOCK <LockName>` – releases the named mutex previously acquired with `AWAIT LOCK`. Locks are identified by arbitrary text names and persist for the lifetime of the program.

## Arithmetic

LDPL provides both expression evaluation and imperative arithmetic commands. In the following descriptions, *number expressions* may be numeric variables, literals or the result of other arithmetic operations.

- **Expression evaluation:** `IN <numberVar> SOLVE <expression>` evaluates a mathematical expression containing `+`, `-`, `*` and `/` with parentheses and stores the result in `<numberVar>`. Expressions are evaluated in left‑to‑right order respecting parentheses.
- **Addition:** `ADD <a> AND <b> IN <c>` adds `a` and `b` and stores the sum in `c`. The reverse form `IN <c> ADD <a> AND <b>` is equivalent.
- **Subtraction:** `SUBTRACT <a> FROM <b> IN <c>` computes `b - a` and stores the result. Reverse form: `IN <c> SUBTRACT <a> FROM <b>`.
- **Multiplication:** `MULTIPLY <a> BY <b> IN <c>` stores `a × b` in `c`. Reverse form: `IN <c> MULTIPLY <a> BY <b>`.
- **Division:** `DIVIDE <a> BY <b> IN <c>` stores `a ÷ b` in `c`. Reverse form: `IN <c> DIVIDE <a> BY <b>`. Division by zero triggers a runtime error.
- **Modulo:** `MODULO <a> BY <b> IN <c>` stores the remainder of `a` divided by `b`. Reverse form: `IN <c> MODULO <a> BY <b>`.
- **Exponentiation:** `RAISE <a> TO <b> IN <c>` stores `ab` in `c`. Reverse form: `IN <c> RAISE <a> TO <b>`.
- **Logarithm:** `LOG <a> IN <c>` stores the natural logarithm of `a` in `c`. Reverse form: `IN <c> LOG <a>`. The argument must be positive.
- **Trigonometry:** `SIN <a> IN <c>`, `COS <a> IN <c>` and `TAN <a> IN <c>` store the sine, cosine and tangent of `a` (in radians) in `c`. Reverse forms: `IN <c> SIN <a>`, etc.
- **Random number:** `GET RANDOM IN <c>` stores a pseudo‑random number in the range [0, 1) (greater than or equal to 0 and strictly less than 1) in `<c>`. Reverse form: `IN <c> GET RANDOM`.
- **Increment and decrement:** `INCREMENT <numberVar>` adds 1 to a number variable; `DECREMENT <numberVar>` subtracts 1.
- **Floor and ceiling:** `FLOOR <x>` replaces the value of `x` with the largest integer ≤ `x`; `CEIL <x>` replaces `x` with the smallest integer ≥ `x`. To store the result in a different variable, use `FLOOR <x> IN <y>`, `CEIL <x> IN <y>` or the reverse forms `IN <y> FLOOR <x>` and `IN <y> CEIL <x>`.

## Time Operations

LDPL can interact with the system clock.

- `WAIT <n> MILLISECONDS`, `WAIT <n>` or `SLEEP <n>` – pauses execution for `n` milliseconds. `n` may be a number variable or expression.
- `GET YEAR IN <numberVar>` stores the current year (e.g., 2026). Reverse form: `IN <numberVar> GET YEAR`.
- `GET MONTH IN <numberVar>` stores the current month (1–12). Reverse form: `IN <numberVar> GET MONTH`.
- `GET DAY IN <numberVar>` stores the day of the month (1–31). Reverse form: `IN <numberVar> GET DAY`.
- `GET HOUR IN <numberVar>`, `GET MINUTES IN <numberVar>`, `GET SECONDS IN <numberVar>` – store the current hour (0–23), minutes (0–59) and seconds (0–59). Each has a reverse form beginning with `IN <numberVar> GET …`.
- `GET EPOCH IN <numberVar>` – stores the number of seconds since 1 January 1970 (Unix epoch). Reverse form: `IN <numberVar> GET EPOCH`.
- `GET ELAPSED MILLISECONDS IN <numberVar>` – stores the number of milliseconds elapsed since the program started. Reverse form: `IN <numberVar> GET ELAPSED MILLISECONDS`.

## Text Operations

Text variables support many operations. Unless stated otherwise, reverse forms are permitted by placing `IN <destination>` at the start of the statement.

- **Joining:** - `JOIN <expr1> AND <expr2> IN <textVar>` concatenates two values (numbers are converted to text) and stores the result. - `IN <textVar> JOIN <expr1> <expr2> …` concatenates any number of expressions or text literals and stores the result in `<textVar>`. Useful when joining more than two items.
- **Substring extraction:** `SUBSTRING <textExpr> FROM <start> LENGTH <len> IN <textVar>` copies a slice of a text expression into `<textVar>`. The `start` index is zero‑based and `len` is the number of characters. The reversed form begins with `IN <textVar> SUBSTRING …`.
- **Index search:** `GET INDEX OF <needle> FROM <haystack> IN <numberVar>` stores the zero‑based index of the first occurrence of `needle` in `haystack`, or `‑1` if it is not found. Reverse form: `IN <numberVar> GET INDEX OF <needle> FROM <haystack>`.
- **Counting:** `COUNT <needle> FROM <haystack> IN <numberVar>` counts how many times `needle` appears in `haystack`. Reverse form: `IN <numberVar> COUNT <needle> FROM <haystack>`.
- **Splitting:** `SPLIT <textExpr> BY <delimiter> IN <listVar>` splits the text expression at each occurrence of the delimiter and stores the pieces in a list of text. If the delimiter is an empty string (`""`), the text is split into individual characters. Reverse form: `IN <listVar> SPLIT <textExpr> BY <delimiter>`.
- **Length:** `GET LENGTH OF <textExpr> IN <numberVar>` stores the number of characters (UTF‑8 codepoints) in the text expression. Reverse form: `IN <numberVar> GET LENGTH OF <textExpr>`.
- **Byte count:** `GET BYTE COUNT OF <textExpr> IN <numberVar>` stores the number of bytes in the underlying string. Reverse form: `IN <numberVar> GET BYTE COUNT OF <textExpr>`.
- **Character retrieval:** `GET CHARACTER AT <index> FROM <textExpr> IN <textVar>` stores the character at a zero‑based index into `<textVar>`. Reverse form: `IN <textVar> GET CHARACTER AT <index> FROM <textExpr>`.
- **ASCII conversions:** - `GET ASCII CHARACTER <numberExpr> IN <textVar>` converts a number (0–255) into its single‑character string. - `GET CHARACTER CODE OF <textExpr> IN <numberVar>` stores the numeric code of the first character of `<textExpr>`. Reverse form: `IN <numberVar> GET CHARACTER CODE OF <textExpr>`.
- **Case conversion:** `CONVERT <textExpr> TO UPPERCASE IN <textVar>` and `CONVERT <textExpr> TO LOWERCASE IN <textVar>` convert all characters to upper or lower case. Reverse forms begin with `IN <textVar> CONVERT …`.
- **Trimming:** `TRIM <textExpr> IN <textVar>` removes leading and trailing whitespace. Reverse form: `IN <textVar> TRIM <textExpr>`.
- **Replacement:** `REPLACE <old> FROM <original> WITH <new> IN <textVar>` copies `original` into `<textVar>`, replacing every occurrence of `old` with `new`. Reverse form: `IN <textVar> REPLACE <old> FROM <original> WITH <new>`.
- **Multiline strings:** To embed a block of text literally without escaping quotes, use `STORE QUOTE IN <textVar>`, write the desired lines verbatim on subsequent lines, and then close the quote with `END QUOTE`. The optional form `STORE TRIMMED QUOTE IN <textVar>` trims leading and trailing whitespace from each line. Both statements have reverse forms beginning with `IN <textVar> STORE QUOTE` or `IN <textVar> STORE TRIMMED QUOTE`.

## List Operations

Lists are ordered, growable collections. Elements are accessed by zero‑based index (using the `:` operator) and may be modified with the following statements:

- `PUSH <value> TO <list>` – appends a value of the list’s element type to the end of `<list>`. To create nested containers, use `PUSH MAP TO <mapList>` to append a new empty map to a list of maps or `PUSH LIST TO <listList>` to append a new empty list to a list of lists.
- `GET LENGTH OF <list> IN <numberVar>` – stores the number of elements in the list. Reverse form: `IN <numberVar> GET LENGTH OF <list>`.
- `DELETE LAST ELEMENT OF <list>` – removes the last element if the list is not empty.
- `REMOVE ELEMENT AT <index> FROM <list>` – removes the element at the given index if it is within bounds.
- `CLEAR <collection>` – empties a list or map without deleting the collection itself.
- `COPY <sourceCollection> TO <destinationCollection>` – overwrites `<destinationCollection>` with a copy of `<sourceCollection>`. Both collections must have the same type.

## Map Operations

Maps associate keys with values. Use the `:` operator to index a map: `STORE <value> IN <map>:<key>` assigns a value; `STORE <map>:<key> IN <var>` retrieves a value. Maps support the following statements:

- `GET KEY COUNT OF <map> IN <numberVar>` – stores the number of keys in the map. Reverse form: `IN <numberVar> GET KEY COUNT OF <map>`.
- `GET KEYS OF <map> IN <listVar>` – copies all keys of `<map>` into a list of text. Reverse form: `IN <listVar> GET KEYS OF <map>`.
- `CLEAR <map>` – removes all key–value pairs from the map.
- `COPY <sourceMap> TO <destinationMap>` – overwrites `<destinationMap>` with a copy of `<sourceMap>`. Both maps must have the same key and value types.

## Input and Output

LDPL programs interact with the user and the filesystem using the statements below. All file and command operations update the built‑in variables `errorcode` and `errortext`, which should be checked to determine whether an operation succeeded. A non‑zero `errorcode` indicates failure and `errortext` contains a brief description.

- **Display output:** - `DISPLAY <value> …` writes one or more text or number values to the standard output without a newline. Separate multiple values with spaces. - `PRINT <value> …` writes values followed by a newline. It is equivalent to `DISPLAY` followed by outputting the newline constant `crlf`.
- **User input:** - `ACCEPT <variable>` reads a line from standard input and stores it in a variable. If the variable is a number and the input cannot be parsed as a number, the statement prints an error and prompts again. - `ACCEPT <textVar> UNTIL EOF` reads from standard input until end‑of‑file and stores all input in a text variable.
- **Executing system commands:** - `EXECUTE <textExpr>` runs the given command string on the host system. The standard output and exit code are discarded. - `EXECUTE <textExpr> AND STORE OUTPUT IN <textVar>` runs the command and stores its standard output in `<textVar>`. - `IN <textVar> EXECUTE AND STORE OUTPUT OF <textExpr>` is the reverse form of the previous statement. - `EXECUTE <textExpr> AND STORE EXIT CODE IN <numberVar>` runs the command and stores its exit status (usually `0` for success) in `<numberVar>`. - `IN <numberVar> EXECUTE AND STORE EXIT CODE OF <textExpr>` is the reverse form of the previous statement.
- **File operations:** - `LOAD FILE <fileExpr> IN <textVar>` reads the contents of the file named by `<fileExpr>` into `<textVar>`. Reverse form: `IN <textVar> LOAD FILE <fileExpr>`. - `WRITE <textExpr> TO FILE <fileExpr>` writes the text expression to the specified file, overwriting any existing contents. - `APPEND <textExpr> TO FILE <fileExpr>` appends the text expression to the end of the specified file.

## C++ Extensions

LDPL programs are compiled to C++, so you can extend them with native code.

- **Including C++ files:** The `EXTENSION "file.cpp"` statement includes a C++ source file in the compilation process. You may include multiple extension files. Additional compiler flags such as library links can be passed using `FLAG "-lSDL2"`. To apply a flag only on a specific platform, prefix the statement with the platform name (e.g., `FLAG windows "-lSDL2"`).
- **External variables:** To share data between LDPL and C++, declare a variable in LDPL with the keyword `EXTERNAL`, for example `counter is external number`. In the C++ extension file declare the variable using the equivalent LDPL type: `ldpl_number` for numbers, `ldpl_text` for text, `ldpl_list<T>` for lists and `ldpl_map<T>` for maps, where `T` matches the element type.
- **External functions:** In a C++ extension file, define a function with the exact signature `void NAME()`. External functions cannot accept parameters or return a value. Function names may contain only uppercase letters, digits and underscores. When you call the function from LDPL using `call external name`, the LDPL compiler converts `name` to its C++ form by capitalising letters and replacing punctuation with underscores—for example `call external window.rows` invokes `WINDOW_ROWS()`. If you need to call an LDPL sub‑procedure from C++, declare it in LDPL as `EXTERNAL SUB‑PROCEDURE Name` and forward‑declare it in C++ with `void NAME();`.
- **Naming collisions:** Because non‑alphanumeric characters are converted to underscores and letters are capitalised, different LDPL identifiers such as `one.two`, `one/two` and `One‑Two` all map to the same C++ name `ONE_TWO`. Avoid such collisions when designing external APIs.

## Error Handling

Many statements that interact with the operating system update the built‑in variables `errorcode` and `errortext`. A value of `0` in `errorcode` indicates success. After a file or command operation, check `errorcode` to see whether the operation succeeded. If it is non‑zero, `errortext` contains a human‑readable description of the problem. Do not rely on the exact wording of `errortext`, as it may change between versions.

## Naming Rules

Identifiers (variable names, sub‑procedure names and labels) are case‑insensitive and cannot:

- Contain spaces or double quotes.
- Contain colons (`:`), parentheses, plus or minus signs.
- Be valid numbers (e.g., `123` is not allowed).
- Be named `CRLF` or `LF`, as these are predefined newline constants.