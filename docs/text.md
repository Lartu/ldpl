!!!Note
    While this section is up-to-date and complete, it has to be reformated
    to be easier on the eyes. All UPPERCASE statement names and code should
    be changed to lowercase.

## `IN _ JOIN _`

The `IN JOIN` statement concatenates two or more values and stores them in a TEXT variable. If any of those values is a number, it is converted to a string before concatenation.

**Syntax:**

```coffeescript
IN <TEXT-VAR> JOIN <multiple NUMBER, TEXT, TEXT-VAR, NUMBER-VAR or CRLF>
```

**Example:**

```coffeescript
IN myTextVariable JOIN "Hello World!" " " "Welcome to LDPL!" crlf
```

will store

```coffeescript
"Hello World! Welcome to LDPL!\n"
```

in `myTextVariable`.


## `REPLACE _ WITH _ FROM _ IN _`


The `REPLACE` statement finds and replaces every occurrence of some TEXT in a TEXT variable or value some other TEXT. The result is then stored in a TEXT variable.

**Syntax:**

```coffeescript
REPLACE <TEXT-VAR or TEXT> WITH <TEXT-VAR or TEXT> FROM <TEXT-VAR or TEXT> IN <TEXT-VAR>
```

**Example:**

```coffeescript
REPLACE "COBOL" FROM "COBOL is great!" WITH "LDPL" IN sentiment
DISPLAY sentiment crlf
```

Outputs:

```text
LDPL is great!
```

## `SPLIT _ BY _ IN _`


The `SPLIT` statement breaks up a single TEXT variable into multiple parts based on another TEXT variable and puts those parts into sub-indexes of a `TEXT LIST`, starting at the NUMBER `0` and incrementing by whole numbers. This allows you to break up a text sentence into multiple parts by splitting on spaces, for example. Or to split a file into lines by splitting on `"\n"`

To break TEXT into individual characters, split by the empty string of `""`.

**Syntax:**

```coffeescript
SPLIT <TEXT-VAR or TEXT> BY <TEXT-VAR or TEXT> IN <TEXT-LIST>
```

**Example:**

```coffeescript
DATA:
	parts IS TEXT LIST
PROCEDURE:
	SPLIT "Hello there!" BY " " IN parts
	display parts:0 crlf parts:1 crlf
```

will output:

```text
Hello
there!
```

**Split into characters:**

```coffeescript
DATA:
	parts IS TEXT LIST
PROCEDURE:
	SPLIT "onomatopoeia" BY "" IN parts
	DISPLAY parts:3 " is M " crlf
```

will output:

```text
m is M
```

## `GET CHARACTER AT _ FROM _ IN _`

The `GET CHARACTER AT` statement gets the character at the position indicated by the NUMBER value from the TEXT value and stores it in a TEXT variable.

**Syntax:**

```coffeescript
GET CHARACTER AT <NUMBER-VAR or NUMBER> FROM <TEXT-VAR or TEXT> IN <TEXT-VAR>
```

## `GET LENGTH OF _ IN _`

The `GET LENGTH OF` statement counts the number of characters in the passed TEXT and stores that number in the NUMBER variable.

**Syntax:**

```coffeescript
GET LENGTH OF <TEXT-VAR or TEXT> IN <NUMBER-VAR>
```

## `GET ASCII CHARACTER _ IN _`

The `GET ASCII CHARACTER` statement stores the character with the ASCII code passed in NUMBER or NUMBER-VAR in TEXT-VAR.

**Syntax:**

```coffeescript
GET ASCII CHARACTER <NUMBER or NUMBER-VAR> IN <TEXT-VAR>
```

## `GET CHARACTER CODE OF _ IN _`

The `GET CHARACTER CODE OF` statement stores the ASCII code of the character passed in TEXT or TEXT-VAR in NUMBER-VAR. Will fail if the length of the string passed in TEXT or TEXT-VAR is not 1.

**Syntax:**

```c
GET CHARACTER CODE OF <TEXT or TEXT-VAR> IN <NUMBER-VAR>
```

**Error Codes:**

Multi-byte characters \(like emojis and non-ASCII characters\) cannot be parsed by this statement. When trying to do so, the operation will fail and the following values will be returned into the `ERRORCODE` and `ERRORTEXT` variables:

* `ERRORCODE`: 1
* `ERRORTEXT`: "Multibyte character received \(probably UTF-8\). Can't be parsed into a single number."

!!!warning
	Always use the `ERRORCODE` variable to check if the operation was successful or not. Do **not** use `ERRORTEXT` for anything else than displaying the error found, as its contents may change in future releases of LDPL.


## `STORE QUOTE _ IN _`

The `STORE QUOTE IN` statement allows you to store multiple lines in a single TEXT variable. Between the `STORE QUOTE IN` and `END QUOTE` statements whitespace is preserved literally, escape codes like `\t` and `\e` work the same as they do in regular text variables \(and can themselves be escaped using `\\`\), and double quotes \(`"`\) don't need to be escaped. 

**Syntax:**

```python
STORE QUOTE IN <TEXT-VAR>
	#Text goes here
END QUOTE
```

**Example:**

```coffeescript
DATA:
template IS TEXT

PROCEDURE:
STORE QUOTE IN template
<html>
	<head><title>{{title}}</title></head>
	<body>{{body}}</body>
</html>
END QUOTE

# ...code to use the template...
```

## `GET INDEX OF _ FROM _ IN _`


The `GET INDEX OF - FROM - IN` statement stores in a NUMBER variable the position of the first occurrence of a specified value in a string or TEXT variable. The first position of a string \(the first letter\) is considered to be the position number `0`. The value `-1` is stored if there are no occurrences.

**Syntax:**

```coffeescript
GET INDEX OF <TEXT or TEXT-VAR> FROM <TEXT or TEXT-VAR> IN <NUMBER-VAR>
```

**Example:**

```coffeescript
DATA:
	position IS NUMBER
PROCEDURE:
	GET INDEX OF "is" FROM "LDPL is nice!" IN position
	DISPLAY position CRLF
	# Will display 5.
```

## `COUNT _ FROM _ IN _`

The `COUNT - FROM - IN` statement counts all the appearances of a string in another string and stores that value in a NUMBER variable.

**Syntax:**

```coffeescript
COUNT <TEXT or TEXT-VAR> FROM <TEXT or TEXT-VAR> IN <NUMBER-VAR>
```

**Example:**

```coffeescript
DATA:
	count IS NUMBER
PROCEDURE:
	COUNT "the" FROM "the cat is called theodore" IN count
	DISPLAY count CRLF
	# Will display 2, as the can be found two times in that sentence.
```

## `SUBSTRING _ FROM _ LENGTH _ IN _`

The `SUBSTRING - FROM - LENGTH - IN` statement extracts parts of a string, beginning at the character at the specified position and storing in the destination TEXT variable the specified number of characters.

**Syntax:**

```coffeescript
SUBSTRING <TEXT or TEXT-VAR> FROM <NUMBER or NUMBER-VAR> LENGTH <NUMBER or NUMBER-VAR> IN <TEXT-VAR>
```

**Example:**

```coffeescript
DATA:
	foo IS TEXT
PROCEDURE:
	SUBSTRING "Hello there!" FROM 1 LENGTH 4 IN foo
	# This will extract 4 characters from position 1
	DISPLAY foo CRLF
	# Will display "ello"
```

## `TRIM _ IN`


The `TRIM - IN` statement removes whitespace from both sides of a string and stores the resulting string in a TEXT variable.

**Syntax:**

```coffeescript
TRIM <TEXT or TEXT-VAR> IN <TEXT-VAR>
```

**Example:**

```coffeescript
DATA:
	foo IS TEXT
PROCEDURE:
	TRIM "    hello there!         " IN foo
	DISPLAY foo CRLF
	# Will display "hello there!"
```

## `CONVERT _ TO UPPERCASE IN _`

The `CONVERT - TO UPPERCASE IN` statement converts all the characters in a string to uppercase and stores the resulting string in a TEXT variable.

**Syntax:**

```coffeescript
CONVERT <TEXT or TEXT-VAR> TO UPPERCASE IN <TEXT-VAR>
```

**Example:**

```coffeescript
DATA:
	greeting IS TEXT
PROCEDURE:
	CONVERT "hello there!" TO UPPERCASE IN greeting
	DISPLAY greeting CRLF
	# Will display "HELLO THERE!"
```

## `CONVERT _ TO LOWERCASE IN _`

The `CONVERT - TO LOWERCASE IN` statement converts all the characters in a string to lowercase and stores the resulting string in a TEXT variable.

**Syntax:**

```coffeescript
CONVERT <TEXT or TEXT-VAR> TO LOWERCASE IN <TEXT-VAR>
```

**Example:**

```coffeescript
DATA:
	greeting IS TEXT
PROCEDURE:
	CONVERT "HELLO THERE!" TO LOWERCASE IN greeting
	DISPLAY greeting CRLF
	# Will display "hello there!"
```
