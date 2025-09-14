!!!Note
    Starting on LDPL 5.2.1, all arithmetic statements also accept an IN-first synonym. This
    means that in addition to writing `ADD 1 AND 2 IN myVar`, you can now also write
    `IN myVar ADD 1 AND 2`.

## `IN _ SOLVE _`

The `IN - SOLVE` statement will solve a simple arithmetic expression and place the result in a NUMBER variable. Only `+`, `-`, `/`, `*` operators, NUMBER values, and TEXT values can be used in a MATH-EXPRESSION. Other LDPL arithmetic functions, like `floor` and `modulo`, are not supported by this statement and should be used as standalone statements. TEXT values will be implicitly converted to NUMBERs using the same algorithm as the one used in `store _ in _`.

Spaces **must** be used to separate numbers, variables and operators.

As in actual arithmetic, `*` and `/` have higher precedence than `+` and `-` , while parens `()` can be used to group expressions.

**Syntax:**

```coffeescript
IN <NUMBER-VAR> SOLVE <MATH-EXPRESSION>
```

**Example:**

```coffeescript
IN myNumVariable SOLVE 1 + 1
```

Will set the value of `myNumVariable` to `2`.

**Area of Circle:**

```coffeescript
DATA:
Radius is NUMBER
Area is NUMBER

PROCEDURE:
DISPLAY "Enter Radius: " 
ACCEPT Radius

IN Area SOLVE 3.14159 * (Radius * Radius)
DISPLAY "Area is: " Area CRLF
```

Outputs:

```text
Enter Radius: 0.5
Area is: 0.7853975
```

## `FLOOR`

The `FLOOR` statement rounds down the value of NUMBER-VAR to the nearest lower integer.

**Syntax:**

```coffeescript
FLOOR <NUMBER-VAR>
```

## `CEIL`

The `CEIL` statement rounds up the value of NUMBER-VAR to the nearest higher integer.

**Syntax:**

```coffeescript
CEIL <NUMBER-VAR>
```

## `FLOOR _ IN _`

The `FLOOR _ IN _` statement rounds down the value of NUMBER-VAR to the nearest lower integer
and stores the result in a NUMBER variable.

**Syntax:**

```coffeescript
FLOOR <NUMBER-VAR> IN <NUMBER-VAR>
```

## `CEIL _ IN _`

The `CEIL _ IN _` statement rounds up the value of NUMBER-VAR to the nearest higher integer
and stores the result in a NUMBER variable.

**Syntax:**

```coffeescript
CEIL <NUMBER-VAR> IN <NUMBER-VAR>
```

## `ADD _ AND _ IN _`

The `ADD` statement adds two NUMBER values and stores the result in a NUMBER variable.

**Syntax:**

```coffeescript
 ADD <NUMBER-VAR or NUMBER> AND <NUMBER-VAR or NUMBER> IN <NUMBER-VAR>
```

## `SUBTRACT _ FROM _ IN _`

The `SUBTRACT` statement subtracts two NUMBER values and stores the result in a NUMBER variable.

**Syntax:**

```coffeescript
 SUBTRACT <NUMBER-VAR or NUMBER> FROM <NUMBER-VAR or NUMBER> IN <NUMBER-VAR>
```

## `MULTIPLY _ BY _ IN _`

The `MULTIPLY` statement multiplies two NUMBER values and stores the result in a NUMBER variable.

**Syntax:**

```coffeescript
 MULTIPLY <NUMBER-VAR or NUMBER> BY <NUMBER-VAR or NUMBER> IN <NUMBER-VAR>
```


## `DIVIDE _ BY _ IN _`

The `DIVIDE` statement divides two NUMBER values and stores the result in a NUMBER variable.

**Syntax:**

```coffeescript
 DIVIDE <NUMBER-VAR or NUMBER> BY <NUMBER-VAR or NUMBER> IN <NUMBER-VAR>
```

## `MODULO _ BY _ IN _`

The `MODULO` statement calculates the remainder of the modulo operation between two NUMBER values and stores the result in a NUMBER variable.

**Syntax:**

```coffeescript
 MODULO <NUMBER-VAR or NUMBER> BY <NUMBER-VAR or NUMBER> IN <NUMBER-VAR>
```


## `GET RANDOM IN _`

The `GET RANDOM` statement stores a random value between 0 \(inclusive\) and 1 \(noninclusive\) in a NUMBER variable.

**Syntax:**

```coffeescript
GET RANDOM IN <NUMBER-VAR>
```

## `RAISE _ TO _ IN _`

The `RAISE <a> TO <b> IN <c>` statement calculates `a^b` and stores the result in `c`.

**Syntax:**

```coffeescript
RAISE <NUMBER-VAR or NUMBER> TO <NUMBER-VAR or NUMBER> IN <NUMBER-VAR>
```

## `LOG _ IN _`

The `LOG _ IN _` statement calculates the natural logarithm of a NUMBER and stores the result in a NUMBER variable.

**Syntax:**

```coffeescript
LOG <NUMBER-VAR or NUMBER> IN <NUMBER-VAR>
```

## `SIN _ IN _`

The `SIN _ IN _` statement calculates the sine of a NUMBER and stores the result in a NUMBER variable.

**Syntax:**

```coffeescript
SIN <NUMBER-VAR or NUMBER> IN <NUMBER-VAR>
```

## `COS _ IN _`

The `COS _ IN _` statement calculates the cosine of a NUMBER and stores the result in a NUMBER variable.

**Syntax:**

```coffeescript
COS <NUMBER-VAR or NUMBER> IN <NUMBER-VAR>
```

## `TAN _ IN _`

The `TAN _ IN _` statement calculates the tangent of a NUMBER and stores the result in a NUMBER variable.

**Syntax:**

```coffeescript
TAN <NUMBER-VAR or NUMBER> IN <NUMBER-VAR>
```
```

## `INCREMENT _`

The `INCREMENT _` statement adds 1 to the value of a NUMBER variable.

**Syntax:**

```coffeescript
INCREMENT <NUMBER-VAR>
```

## `DECREMENT _`

The `DECREMENT _` statement subtracts 1 from the value of a NUMBER variable.

**Syntax:**

```coffeescript
DECREMENT <NUMBER-VAR>
```