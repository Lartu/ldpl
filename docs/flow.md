# Control Flow Statements

!!!Note
    While this section is up-to-date and complete, it has to be reformated
    to be easier on the eyes. All UPPERCASE statement names and code should
    be changed to lowercase.

## `STORE _ IN _`

The `STORE` statement assigns a value to a variable.

**Syntax:**

	:::coffeescript
	STORE <NUMBER-VAR or NUMBER or TEXT-VAR or TEXT> IN <NUMBER-VAR or TEXT-VAR>


**Type Conversion Notes:**

If the value to be stored is NUMBER and it's to be stored in a TEXT variable,
the value will be converted to text, so `15` will be turned into `"15"`. If the
value to be stored is a TEXT value two things can happen. If it contains any
non-numeric characters \(for example letters, or more than one minus sign or
more than one decimal point, for example `"--1.2"` or `"15a"`\) the conversion
will fail and 0 will be stored in the NUMBER variable. If the TEXT contains a
proper number, though, for example `"-416.419"` or `"89"` it will be converted
to its number equivalent and stored in the variable. If a string literal depicting
a number is preceded by leading zeros, these will be trimmed \(turning `0005`
into `5`, `-0002.3` into `-2.3` and `00.23` into `0.23`\).

## `IF _ IS _ THEN`

The `IF` statement evaluates if the condition given is positive. If it is, the code in the positive branch is executed. If it is not, the code in the negative branch is executed \(if available\). Execution then continues normally.

**Syntax:**

```coffeescript
IF <CONDITION> THEN
	#Code goes here (positive branch)
ELSE
	#Code goes here (negative branch)
END IF
```

or

```coffeescript
IF <CONDITION> THEN
	#Code goes here (positive branch)
END IF
```

The `<CONDITION>` may be a relational operator between two values with the same type:

* `<NUMBER-VAR or NUMBER> IS <REL-OP-A> <NUMBER-VAR or NUMBER>`
* `<TEXT-VAR or TEXT> IS <REL-OP-B> <TEXT-VAR or TEXT>`
* `<NUMBER LIST> IS <REL-OP-B> <NUMBER LIST>`
* `<TEXT LIST> IS <REL-OP-B> <TEXT LIST>`
* `<NUMBER MAP> IS <REL-OP-B> <NUMBER MAP>`
* `<TEXT MAP> IS <REL-OP-B> <TEXT MAP>`

**Possible values of `REL-OP-A`:**

* `EQUAL TO`
* `NOT EQUAL TO`
* `GREATER THAN`
* `LESS THAN`
* `GREATER THAN OR EQUAL TO`
* `LESS THAN OR EQUAL TO`

**Possible values of `REL-OP-B`:**

* `EQUAL TO`
* `NOT EQUAL TO`

!!! info
    Multicontainer comparison is only available in LDPL 4.5-dev *Groovy Gualicho*.

For containers, both values must have the same type. You **cannot** compare, for example,
a `list of lists of numbers` with a `list of lists of lists of numbers` or a `number map`
with a `text map`.

!!! info
    The membership operator is only available in LDPL 4.5-dev *Groovy Gualicho*.

The `<CONDITION>` may also be a membership operator:

* `<NUMBER-VAR or NUMBER or TEXT-VAR or TEXT> IN <LIST>`
* `<NUMBER-VAR or NUMBER or TEXT-VAR or TEXT> IN <MAP>`

When using a membership operator, if the second value is a list, it checks if the first value
is contained within that list. If the second value is a map, however, it checks if the first
value is contained within the keys of that map.

The first value must always be a scalar value, you **cannot** check if, for example, a
`map of numbers` is contained within a `list of maps of numbers`.

You can also write **compound conditions** using `AND`, `OR` and parenthesis:

* `<CONDITION> AND <CONDITION>` is positive if both conditions are positive
* `<CONDITION> OR <CONDITION>` is positive if any of the conditions is positive
* `( <CONDITION> )` is positive if the condition is positive and it's used to alter the default precedence

The `AND` has higher precedence than `OR`, and the conditions inside parenthesis will be evaluated first. That means that `C1 AND C2 OR C3` is the same as `( C1 AND C2 ) OR C3`, but you can make the `OR` evaluate first if you write `C1 AND ( C2 OR C3 )`. Mind the spaces surrounding the parenthesis: `(<CONDITION>)` is **not** a
valid condition, while `( <CONDITION> )` is.

Both `AND` and `OR` perform short-circuit evaluation: If the first operand of an AND is negative the second will not be evaluated and the `AND` condition is determined as negative. If the first operand of an OR is positive the second will not be evaluated and the `OR` condition is determined as positive.

For example:

```coffeescript
DATA:
	names IS TEXT LIST
	length IS NUMBER
PROCEDURE:
	GET LENGTH OF names IN length
	IF length IS GREATER THAN 0 AND ( names:0 IS EQUAL TO "Alice" OR names:0 IS EQUAL TO "Bob" ) THEN
		#Code
	END IF
```

The `names` list is empty, so the `length is greater than 0` condition is negative, and the second one is not evaluated \(and the execution continues after the `END IF`\). Thanks to short-circuit evaluation `names:0` is not evaluated and we don\`t get an index out of range runtime error!

## `ELSE IF _ IS _ THEN`

The `ELSE IF` statement is equivalent to writing an `IF` statement inside the `ELSE` statement of another `IF` statement, but shorter. Must be used after an IF statement and before `END IF` or `ELSE`.

**Syntax:**

All the different `IF` variants of the [IF statement](./) apply, just with `ELSE` added before them.

**Example:**

```coffeescript
DATA:
	name IS TEXT
PROCEDURE:
	STORE "Mike" IN name
	IF name IS equal to "John" THEN
		DISPLAY "Hello there, John!" CRLF
	ELSE IF name IS equal to "Mike" THEN
		DISPLAY "Hello there, Mike!" CRLF
	ELSE IF name IS equal to "Robert" THEN
		DISPLAY "Hello there, Robert!" CRLF
	ELSE
		DISPLAY "I don't know you, " name CRLF
	END IF
```



## `WHILE _ IS _ DO`

The `WHILE` statement evaluates if the condition given is positive. While it is, the code between the `WHILE` and `REPEAT` statements is repeatedly ran.

**Syntax:**

```coffeescript
WHILE <CONDITION> DO
	#Code goes here
REPEAT
```

The `<CONDITION>` that you can use are the same as the [IF](if-is-then/) statement ones.

## `FOR _ FROM _ TO _ STEP _ DO`

The `FOR` statement repeatedly run the code in its body a number of times, given a `counter` variable, the `start` of the range, its `end` and a `step`.

When the loop starts, `start` is assigned to `counter` and starts an iteration, evaluating a condition. The condition is `counter < end` if `step >= 0` and `counter > end` if `step < 0`. If the condition passes, the code in the body of the `FOR` is executed, otherwise the loop will end. After the code is ran the `counter` is incremeted by `step` and a new iteration is started \(checking the condition and so on\).

**Syntax:**

```coffeescript
FOR <NUMBER-VAR> FROM <NUMBER-VAR or NUMBER> TO <NUMBER-VAR or NUMBER> STEP <NUMBER-VAR or NUMBER> DO
	#Code goes here
REPEAT
```

**Example:**

```coffeescript
DATA:
	i IS NUMBER
PROCEDURE:
	FOR i FROM 0 TO 10 STEP 2 DO
		DISPLAY i " "
	REPEAT
	# Will display "0 2 4 6 8 10"
```

## `FOR EACH _ IN _ DO`

The `FOR EACH` statement repeatedly run the code in its body for every element in a given `LIST` or `MAP`. At the start of each iteration an element of the collection is assigned to a variable matching its type. This even works with multicontainers. For example, you can iterate a `NUMBER MAP LIST` using a `NUMBER MAP` as the iteration variable.  

If the collection is a `LIST`, its elements will be iterated increasingly from index `0`, while in the case of a `MAP` all the elements will be iterated in no particular order.

**Syntax:**

```coffeescript
FOR EACH <VAR> IN <LIST or MAP> DO
	#Code goes here
REPEAT
```

**Example 1:**

```coffeescript
DATA:
	letter IS TEXT
	letters IS TEXT LIST
PROCEDURE:
	PUSH "L" TO letters
	PUSH "D" TO letters
	PUSH "P" TO letters
	PUSH "L" TO letters
	FOR EACH letter IN letters DO
		DISPLAY letter
	REPEAT
	# Will display "LDPL"
```

**Example 2:**

```coffeescript
DATA:
	number IS NUMBER
	numbers IS NUMBER MAP
PROCEDURE:
	STORE 1 IN numbers:"One"
	STORE 2 IN numbers:"Two"
	STORE 3 IN numbers:"Three"
	FOR EACH number IN numbers DO
		DISPLAY number
	REPEAT
	# Will display "312" or any permutation
```

## `BREAK`

The `BREAK` statement breaks the execution of the innermost `WHILE`, `FOR` or `FOR EACH` loop. Will throw a compiler error if used outside one.

**Syntax:**

```coffeescript
BREAK
```

## `CONTINUE`

The `CONTINUE` statement jumps to the next iteration of the innermost `WHILE` or `FOR` loop. Will throw a compiler error if used outside one.

**Syntax:**

```coffeescript
CONTINUE
```

## `CALL SUB-PROCEDURE`

The `CALL SUB-PROCEDURE` statement executes a SUB-PROCEDURE. Once the SUB-PROCEDURE returns, execution continues from the line following the `CALL SUB-PROCEDURE`.

**Syntax:**

```coffeescript
CALL SUB-PROCEDURE <sub-procedure name>
CALL SUB-PROCEDURE <sub-procedure name> WITH <multiple NUMBER, TEXT, TEXT-VAR, NUMBER-VAR, MAP or LIST>
```

Or

```coffeescript
CALL <sub-procedure name>
CALL <sub-procedure name> WITH <multiple NUMBER, TEXT, TEXT-VAR, NUMBER-VAR, MAP or LIST>
```

Of course, a SUB-PROCEDURE must be declared **somewhere** in your program for you to call it.

If the SUB-PROCEDURE you call doesn't have any declared parameters, you must call it without the `WITH` keyword, otherwise you must include it and pass all required parameters after it, in the same order declared in the `PARAMETERS` section of the SUB-PROCEDURE.

## `RETURN`

The `RETURN` statement returns from a SUB-PROCEDURE. Will throw a compiler error if used outside one.

**Syntax:**

```coffeescript
RETURN
```

## `EXIT`

The `EXIT` statement ends execution of the program.

**Syntax:**

```coffeescript
EXIT
```

## `WAIT _ MILLISECONDS`

The `WAIT` statement pauses the execution of a program for the given number of milliseconds.

**Syntax:**

```coffeescript
WAIT <NUMBER or NUMBER-VAR> MILLISECONDS
```

## `GOTO and LABEL`

> "If you want to go somewhere, goto is the best way to get there."	
> -- Ken Thompson

The `GOTO` statement performs a **one-way transfer** of control to a line of code marked by a `LABEL` statement. In lame man terms, the execution jumps to the line where the wanted `LABEL` is found and continues from there.

While maligned by [Edsger W. Dijkstra](https://en.wikipedia.org/wiki/Edsger_W._Dijkstra) and his cohorts, `GOTO` is very useful in many situations. Its reputation is undeserved and mostly perpetuated by people that don't understand the origins of the criticism or how the statement can be used.

You also can't make a COBOL-_esque_ language without `GOTO`, so \(due to popular request\) we've added it to the language.

**Syntax:**

```text
LABEL <labelName>
```

```text
GOTO <labelName>
```

!!!hint
	Label names follow the naming rules stated in the **Identifier Naming Schemes** section of this documentation.


**Example:**

```coffeescript
PROCEDURE:
	GOTO start

	LABEL start
	display "> starting..." crlf

	GOTO ending

	LABEL middle
	display "> entering the middle section..." crlf

	sub cool-code
		GOTO cool
		display "hmm... is this cool?" crlf
		LABEL cool
		display "wow, yeah! cool code!" crlf
	end sub

	LABEL ending
	CALL cool-code
	display "> that's the end" crlf
```

In the output of this program you can see the `middle` LABEL and the start of the `cool-code` SUB-PROCEDURE are skipped:

```text
> starting...
wow, yeah! cool code!
> that's the end
```

In order to keep `GOTO` from turning your source into _unmaintainable spaghetti code_, both your `GOTO` statements and the `LABEL`s they jump to have to be declared together in the same sub-procedure or in the main code body of an LDPL program. You can't `goto` across sub-procedures or into them, or anything like that.

## `CREATE STATEMENT _ EXECUTING _`

The `CREATE STATEMENT` statement lets you add custom statements to LDPL that execute SUB-PROCEDUREs.

**Syntax:**

```coffeescript
CREATE STATEMENT <TEXT> EXECUTING <sub-procedure name>
```

The `TEXT` describes the new statement syntax and must contain tokens separated by whitespace. Each token can be a keyword, which is a word with `A-Z` characters \(preferably in English\), or `"$"`, a character that marks where parameters are passed. At least one keyword token is required and the number of `"$"` tokens must be the same as the number of parameters of the SUB-PROCEDURE you pass after `EXECUTING`. For example, a valid `TEXT` is `"DISPLAY $ $ TIMES"` if the SUB-PROCEDURE has exactly two parameters. The SUB-PROCEDURE must be declared before creating the statement.

After a statement is created you can use it like any other LDPL statement in `PROCEDURE` sections, just write a line with all the tokens of the `TEXT` in the same order but placing values instead of `"$"`. The types of the values must be the same as the parameter types of the SUB-PROCEDURE the statement executes following the same order. Using the new statement will produce the same effect as `CALL`ing the SUB-PROCEDURE \(parameters are passed by reference too\).

You can create two different statements with same `TEXT` and use both if at least one of the parameter types are different in each SUB-PROCEDURE, because the resulting syntaxes will differ from each other. Using this you can create two versions of the same statements dealing with different parameter types, like the first example shows.

Bear in mind that a line in your program could match more than one statement: If all of them were created with `CREATE STATEMENT`, the one that was created first will be executed. If one of the them is a LDPL built-in statement, this will be executed. For example, if you create `"DISPLAY $ $ TIMES"`, declare a variable `TIMES` and use the line `DISPLAY "Hi!" 3 TIMES`, the LDPL `DISPLAY` statement will be executed, because the line matches its syntax. This is illustrated in the second example.

**Example 1:**

```coffeescript
PROCEDURE:
	SUB-PROCEDURE displayNValueTimes
	PARAMETERS:
		value is number
		times is number
	LOCAL DATA:
		i is number
	PROCEDURE:
		FOR i FROM 0 TO times STEP 1 DO
		DISPLAY value " "
		REPEAT
	END SUB-PROCEDURE
	CREATE STATEMENT "DISPLAY $ $ TIMES" EXECUTING displayNValueTimes
	# Syntax for this new statement: DISPLAY <NUMBER or NUMBER-VAR> <NUMBER or NUMBER-VAR> TIMES

	SUB-PROCEDURE displayTValueTimes
	PARAMETERS:
		value is text
		times is number
	LOCAL DATA:
		i is number
	PROCEDURE:
		FOR i FROM 0 to times STEP 1 DO
		DISPLAY value " "
		REPEAT
	END SUB-PROCEDURE
	CREATE STATEMENT "DISPLAY $ $ TIMES" EXECUTING displayTValueTimes
	# Syntax for this new statement: DISPLAY <TEXT or TEXT-VAR> <NUMBER or NUMBER-VAR> TIMES

	# We can imagine that we have only one new statement:
	# DISPLAY <NUMBER or NUMBER-VAR or TEXT or TEXT-VAR> <NUMBER or NUMBER-VAR> TIMES

	DISPLAY 100 2 TIMES # This executes: CALL displayNValueTimes with 100 2
	DISPLAY "Hi!" 3 TIMES # This executes: CALL displayTValueTimes with "Hi!" 3

	# This program displays "100 100 Hi! Hi! "
```

**Example 2:**

```coffeescript
DATA:
	times is number
PROCEDURE:
	SUB-PROCEDURE displayTValueTimes
	PARAMETERS:
		value is text
		times is number
	LOCAL DATA:
		i is number
	PROCEDURE:
		FOR i FROM 0 to times STEP 1 DO
			DISPLAY value " "
		REPEAT
	END SUB-PROCEDURE
	CREATE STATEMENT "DISPLAY $ $ TIMES" EXECUTING displayTValueTimes
	# Syntax for this new statement: DISPLAY <TEXT or TEXT-VAR> <NUMBER or NUMBER-VAR> TIMES

	DISPLAY "Hi!" 3 TIMES # This executes the LDPL DISPLAY statement!
	# This program displays "Hi!30" because times is equal to 0
```

## `CALL EXTERNAL _`

!!!hint
    This section talks about external sub-procedure calling for **C++ Extensions**. If
    you have not read the section on C++ Extensions yet, ignore this and then come
    back later.


The `CALL EXTERNAL` statement executes a SUB-PROCEDURE defined in an extension to LDPL, typically in C++. It otherwise operates the same as `CALL SUB-PROCEDURE`, except that external SUB-PROCEDURES do not receive parameters.

**Syntax:**

```text
CALL EXTERNAL <external sub-procedure name>
```

**Example:**

```text
CALL EXTERNAL http-get
```

## `IN _ CALL PARALLEL _`

!!! info
    This feature is currently only available in LDPL 4.5-dev *Groovy Gualicho*.

The `CALL PARALLEL` statement executes a PARALLEL SUB-PROCEDURE on a parallel thread to the main thread of your program and stores a reference number to it in the passed NUMBER variable. Once the parallel thread is running, execution continues from the line following the `CALL PARALLEL`.

The value stored in the NUMBER variable will be the same as the value stored in the
`parallel_id` variable within the parallel sub-procedure.

**Syntax:**

```coffeescript
IN <NUMBER-VAR> CALL PARALLEL <parallel sub-procedure name>
```

Of course, a PARALLEL SUB-PROCEDURE must be declared **somewhere** in your program for you to call it.

## `WAIT FOR PARALLEL _`

!!! info
    This feature is currently only available in LDPL 4.5-dev *Groovy Gualicho*.

The `WAIT FOR PARALLEL` statement halts execution of the current thread until the parallel sub-procedure referenced by the passed NUMBER variable has finished running.

**Syntax:**

```coffeescript
WAIT FOR PARALLEL <NUMBER-VAR>
```

## `STOP PARALLEL _`

!!! info
    This feature is currently only available in LDPL 4.5-dev *Groovy Gualicho*.

The `STOP PARALLEL` statement halts execution of the parallel sub-procedure referenced by the passed NUMBER variable.

**Syntax:**

```coffeescript
STOP PARALLEL <NUMBER-VAR>
```
