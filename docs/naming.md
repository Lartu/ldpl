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

## Label Naming Schemes

Labels are identifiers used alongside the `label` statement.
Labels in LDPL may not be empty strings and may contain any character except spaces and
`"` (double quotes). Labels cannot be named `CRLF` nor `LF` for the same reasons explained in
the section above.

## External Identifier Naming Schemes

!!!hint
    This section talks about identifier naming schemes for **C++ Extensions**. If
    you have not read the section on C++ Extensions yet, ignore this and then come
    back later.

All C++ variables and functions accessible to LDPL programs may contain only
`A-Z`, `0-9`, and the `_` character in their names. All other characters used
on the LDPL side to reference a variable or function will get converted to an
underscore (`_`) or, if it's a letter, capitalized.

For example:

| LDPL Identifier | Result When Converted to a C++ Identifier |
| :--- | :--- |
| window.rows | WINDOW\_ROWS |
| HTTP/get | HTTP\_GET |
| SDL/Font.new | SDL\_FONT\_NEW |
| sdl.font-new | SDL\_FONT\_NEW |
| NAME | NAME |
| version\_number | VERSION\_NUMBER |

!!!warning
    Note that this conversion scheme may cause collisions.
    All of the following LDPL variables will be converted to `ONE_TWO:` 

    * `One-Two` 
    * `one.two` 
    * `one/two` 
    * `OnE-TWO`
