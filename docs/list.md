!!!Note
    While this section is up-to-date and complete, it has to be reformated
    to be easier on the eyes. All UPPERCASE statement names and code should
    be changed to lowercase.

## `PUSH _ TO _`

The `PUSH - TO` statement is used to add elements to a LIST. When you push an element to a LIST it is appended at the end of the list.

**Syntax:**

```coffeescript
PUSH <NUMBER-VAR or NUMBER> TO <NUMBER-LIST>
PUSH <TEXT-VAR or TEXT> TO <TEXT-LIST>
```

**Example:**

```coffeescript
DATA:
	foo IS TEXT LIST
PROCEDURE:
	PUSH "First index" TO foo
	PUSH "Second index" TO foo
```

In the above example, `foo` now contains the value `"First index"` at index `0` and the value `"Second index"` at index `1`.

## `CLEAR`

The `CLEAR` statement empties a LIST, thus deleting all its contents. The LIST itself is not deleted though, and can still be used and filled with new elements after a `CLEAR` statement has been executed.

**Syntax:**

```coffeescript
CLEAR <LIST>
```

## `COPY _ TO _`

The `COPY - TO` statement copies all the elements of a LIST with their respective indices to another LIST **of the same type**. The original LIST is untouched, but the destination LIST is completely overwritten by the contents of the copied LIST and any elements that existed in it prior to the copy are deleted. In other words, the destination LIST is `CLEAR`ed before the copy.

**Syntax:**

```coffeescript
COPY <TEXT-LIST> TO <TEXT-LIST>
COPY <NUMBER-LIST> TO <NUMBER-LIST>
```

**Example:**

```coffeescript
DATA:
	foo IS TEXT LIST
	bar IS TEXT LIST
PROCEDURE:
	PUSH "Hello there!" TO foo
	PUSH "How are you?" TO foo
	COPY foo TO bar
	DISPLAY bar:0 " " bar:1 CRLF
	# Will display "Hello there! How are you?"
```

## `GET LENGTH OF _ IN _`

The `GET LENGTH OF - IN` statement stores the amount of elements stored in a LIST \(or, analogously, the length of the LIST\) into a numeric variable.

**Syntax:**

```coffeescript
GET LENGTH OF <LIST> IN <NUMBER-VAR>
```

**Example:**

```coffeescript
DATA:
	foo IS TEXT LIST
	count IS NUMBER
PROCEDURE:
	PUSH "Hello there!" TO foo
	PUSH "How are you?" TO foo
	STORE LENGTH OF foo IN count
	DISPLAY count CRLF
	# Will display 2
```

## `DELETE LAST ELEMENT OF _`


The `DELETE LAST ELEMENT OF` deletes the last element pushed to a LIST. If the LIST was empty, this statement does nothing.

**Syntax:**

```coffeescript
DELETE LAST ELEMENT OF <LIST>
```

## `REMOVE ELEMENT AT _ FROM _`

The `REMOVE ELEMENT AT - FROM` statement deletes the element at the specified index from a LIST. If the index is out of bounds, this statement does nothing.

**Syntax:**

```coffeescript
REMOVE ELEMENT AT <NUMBER-VAR or NUMBER> FROM <LIST>
```

**Example:**

```coffeescript
DATA:
	foo IS TEXT LIST
PROCEDURE:
	PUSH "Hello there!" TO foo
	PUSH "How are you?" TO foo
	REMOVE ELEMENT AT 0 FROM foo
	DISPLAY foo:0 CRLF
	# Will display "How are you?"
```
