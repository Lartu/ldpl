!!!Note
    While this section is up-to-date and complete, it has to be reformated
    to be easier on the eyes. All UPPERCASE statement names and code should
    be changed to lowercase.
    
## `DISPLAY`

The `DISPLAY` statement outputs the values passed to the output stream. `CRLF` means line break and is a sugar syntax for the `"\n"` escape sequence.

**Syntax:**

```coffeescript
DISPLAY <multiple NUMBER, TEXT, TEXT-VAR, NUMBER-VAR or CRLF>
```

**Example:**

```coffeescript
DISPLAY "Hello, " nameVariable "! This is a number -> " 89.1 " :)" CRLF
```

## `ACCEPT _`

The `ACCEPT` command is used to gather input from the user. If a TEXT variable is specified, anything the user enters before pressing the 'return' key will be accepted. If a NUMBER variable is specified, the user must enter a number \(if any non-numeric key is entered, the error message "Redo from start" will be output and the ACCEPT command rerun\).

**Syntax:**

```coffeescript
ACCEPT <TEXT-VAR or NUMBER-VAR>
```

## `EXECUTE _`

The `EXECUTE` statement executes the specified system command.

**Syntax:**

```coffeescript
EXECUTE <TEXT or TEXT-VAR>
```

**Example 1:**

```coffeescript
# Prepare the command to execute
IN myTextVar JOIN "echo " myVariable " >> myFile"
# Execute it
EXECUTE myTextVar
```

**Example 2:**

```coffeescript
# Execute "dir" to list the files in the current directory under Windows
EXECUTE "dir"
```

## `EXECUTE _ AND STORE OUTPUT IN _`

The `EXECUTE - AND STORE OUTPUT IN` executes the specified command and stores any resulting text in the passed variable.

**Syntax:**

```coffeescript
EXECUTE <TEXT or TEXT-VAR> AND STORE OUTPUT IN <TEXT-VAR>
```

## `EXECUTE _ AND STORE EXIT CODE IN _`

The `EXECUTE - AND STORE EXIT CODE IN` executes the specified command and stores the exit code in the passed variable.

**Syntax:**

```coffeescript
EXECUTE <TEXT or TEXT-VAR> AND STORE EXIT CODE IN <NUM-VAR>
```

## `ACCEPT _ UNTIL EOF`

The `ACCEPT UNTIL EOF` statement accepts input from standard input until an EOF state is reached and stores all data gathered in TEXT-VAR.

**Syntax:**

```coffeescript
ACCEPT <TEXT-VAR> UNTIL EOF
```

## `LOAD FILE _ IN _`


The `LOAD FILE` statement loads the contents of a file into a text variable.

**Syntax:**

```coffeescript
LOAD FILE <TEXT or TEXT-VAR> IN <TEXT-VAR>
```

**Example:**

```coffeescript
LOAD FILE "myFolder/myTextFile.txt" IN myVariable
```

**Error Codes:**

If the LOAD operation should fail, the following values will be returned into the `ERRORCODE` and `ERRORTEXT` variables:

* `ERRORCODE`: 1
* `ERRORTEXT`: "The file '&lt;filename&gt;' couldn't be opened."

!!!warning
	Always use the `ERRORCODE` variable to check if the operation was successful or not. Do **not** use `ERRORTEXT` for anything else than displaying the error found, as its contents may change in future releases of LDPL.

## `WRITE _ TO FILE _`

The `WRITE x TO FILE y` statement writes the value of `x` to the file called `y`. If the file already exists, everything in it will be overwritten by `x`.

**Syntax:**

```coffeescript
WRITE <NUMBER or NUMBER-VAR or TEXT or TEXT-VAR> TO FILE <TEXT or TEXT-VAR>
```

**Example:**

```coffeescript
WRITE "Hello there!" TO FILE "hello.txt"
```

**Error Codes:**

If the WRITE operation should fail, the `ERRORCODE` and `ERRORTEXT` variables will be set to the following values:

* `ERRORCODE`: 1
* `ERRORTEXT`: "Could not open '&lt;filename&gt;'"
* `ERRORCODE`: 2
* `ERRORTEXT`: "Could not write to '&lt;filename&gt;'"

## `APPEND _ TO FILE _`

The `APPEND x TO FILE y` statement appends the value of `x` to the file called `y`. If the file already exists, `x` will be added at the end of its contents.

**Syntax:**

```coffeescript
APPEND <NUMBER or NUMBER-VAR or TEXT or TEXT-VAR> TO FILE <TEXT or TEXT-VAR>
```

**Example:**

```coffeescript
APPEND "\nHow are you?" TO FILE "hello.txt"
```

in this case, the file `hello.txt` (created in the example of the `WRITE _ TO FILE _` function and modified as stated there) will contain the text

```text
Hello there!
How are you?
```

**Error Codes:**

If the APPEND operation should fail, the `ERRORCODE` and `ERRORTEXT` variables will be set to the following values:

* `ERRORCODE`: 1
* `ERRORTEXT`: "Could not open '&lt;filename&gt;'"
* `ERRORCODE`: 2
* `ERRORTEXT`: "Could not write to '&lt;filename&gt;'"
