!!!Note
    While this section is up-to-date and complete, it has to be reformated
    to be easier on the eyes. All UPPERCASE statement names and code should
    be changed to lowercase.

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

!!! info
    This feature is currently only available in LDPL 4.5-dev *Groovy Gualicho*.

The `CEIL` statement rounds up the value of NUMBER-VAR to the nearest higher integer.

**Syntax:**

```coffeescript
CEIL <NUMBER-VAR>
```

## `FLOOR _ IN _`

!!! info
    This feature is currently only available in LDPL 4.5-dev *Groovy Gualicho*.

The `FLOOR _ IN _` statement rounds down the value of NUMBER-VAR to the nearest lower integer
and stores the result in a NUMBER variable.

**Syntax:**

```coffeescript
FLOOR <NUMBER-VAR> IN <NUMBER-VAR>
```

## `CEIL _ IN _`

!!! info
    This feature is currently only available in LDPL 4.5-dev *Groovy Gualicho*.

The `CEIL _ IN _` statement rounds up the value of NUMBER-VAR to the nearest higher integer
and stores the result in a NUMBER variable.

**Syntax:**

```coffeescript
CEIL <NUMBER-VAR> IN <NUMBER-VAR>
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

## `RAISE _ TO THE _ IN _`

!!! info
    This feature is currently only available in LDPL 4.5-dev *Groovy Gualicho*.

The `RAISE <a> TO THE <b> IN <c>` statement calculates `a^b` and stores the result in `c`.

**Syntax:**

```coffeescript
RAISE <NUMBER-VAR or NUMBER> TO THE <NUMBER-VAR or NUMBER> IN <NUMBER-VAR>
```

## `LOG _ IN _`

!!! info
    This feature is currently only available in LDPL 4.5-dev *Groovy Gualicho*.

The `LOG _ IN _` statement calculates the natural logarithm of a NUMBER and stores the result in a NUMBER variable.

**Syntax:**

```coffeescript
LOG <NUMBER-VAR or NUMBER> IN <NUMBER-VAR>
```

## `SIN _ IN _`

!!! info
    This feature is currently only available in LDPL 4.5-dev *Groovy Gualicho*.

The `SIN _ IN _` statement calculates the sine of a NUMBER and stores the result in a NUMBER variable.

**Syntax:**

```coffeescript
SIN <NUMBER-VAR or NUMBER> IN <NUMBER-VAR>
```

## `COS _ IN _`

!!! info
    This feature is currently only available in LDPL 4.5-dev *Groovy Gualicho*.

The `COS _ IN _` statement calculates the cosine of a NUMBER and stores the result in a NUMBER variable.

**Syntax:**

```coffeescript
COS <NUMBER-VAR or NUMBER> IN <NUMBER-VAR>
```

## `TAN _ IN _`

!!! info
    This feature is currently only available in LDPL 4.5-dev *Groovy Gualicho*.

The `TAN _ IN _` statement calculates the tangent of a NUMBER and stores the result in a NUMBER variable.

**Syntax:**

```coffeescript
TAN <NUMBER-VAR or NUMBER> IN <NUMBER-VAR>
```