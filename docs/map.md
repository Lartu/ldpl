!!!Note
    While this section is up-to-date and complete, it has to be reformated
    to be easier on the eyes. All UPPERCASE statement names and code should
    be changed to lowercase.

## `CLEAR`

The `CLEAR` statement empties a MAP, thus deleting all its contents. The MAP itself is not deleted though, and can still be used and filled with new elements after a `CLEAR` statement has been executed.

**Syntax:**

```coffeescript
CLEAR <MAP>
```

## `COPY _ TO _`

The `COPY - TO` statement copies all the elements of a MAP with their respective keys to another MAP **of the same type**. The original MAP is untouched, but the destination MAP is completely overwritten by the contents of the copied MAP and any elements that existed in it prior to the copy are deleted. In other words, the destination MAP is `CLEAR`ed before the copy.

**Syntax:**

```coffeescript
COPY <TEXT-MAP> TO <TEXT-MAP>
COPY <NUMBER-MAP> TO <NUMBER-MAP>
```

**Example:**

```coffeescript
DATA:
	foo IS TEXT MAP
	bar IS TEXT MAP
PROCEDURE:
	STORE "Hello there!" IN foo:0
	STORE "How are you?" IN foo:7
	COPY foo TO bar
	DISPLAY bar:0 " " bar:7 CRLF
	# Will display "Hello there! How are you?"
```


## `GET KEY COUNT OF _ IN _`

The `GET KEY COUNT OF - IN` statement stores the amount of elements \(or, analogously, keys\) stored in a MAP into a numeric variable.

**Syntax:**

```coffeescript
GET KEY COUNT OF <MAP> IN <NUMBER-VAR>
```

**Example:**

```coffeescript
DATA:
	foo IS TEXT MAP
	count IS NUMBER
PROCEDURE:
	STORE "Hello there!" IN foo:0
	STORE "How are you?" IN foo:7
	GET KEY COUNT OF foo IN count
	DISPLAY count CRLF
	# Will display 2
```


## `GET KEYS OF _ IN _`

The `GET KEYS OF - IN` statement stores all the keys of a MAP into a TEXT LIST. Say you have a MAP with keys `0`, `"cat"` and `"dog"`. The elements these keys point to are not important. Using the `GET KEYS OF` statement, you can copy the keys of this MAP to a LIST. Thus, the resulting LIST will \(for example\) have the value `0` at index 0, the value `"cat"` at index 1 and the value `"dog"` at index 2. This statement is thus used to find all the keys of a particular MAP.

**Syntax:**

```coffeescript
GET KEYS OF <MAP> IN <TEXT-LIST>
```

**Example:**

```coffeescript
DATA:
	foo IS TEXT MAP
	bar IS TEXT LIST
PROCEDURE:
	STORE "Hello there!" IN foo:0
	STORE "How are you?" IN foo:7
	STORE "I like cats" IN foo:"cat"
	STORE "I love dogs" IN foo:"dog"
	STORE "LDPL is nice" IN foo:3
	GET KEYS OF foo IN bar
```

At the end of the execution of the previous excerpt of code, the `TEXT LIST` called `bar` will contain the values `"0"`, `"7"`, `"cat"`, `"dog"` and `"3"` at indexes that are consecutive integers starting at zero.

