## The Procedure Section

The **procedure** section is where all the code of a LDPL program that is not a
variable declaration is written. An LDPL program **must** contain a procedure
section, even if it's empty. Execution should and will fail otherwise.

Within the procedure section, every line can contain either a comment,
a statement, a statement and a comment or be empty. No two statements can be
written on the same line.

An example procedure section may end up looking like this:

    :::coffeescript
    procedure:
        store 5 in foo
        store "hi there" in bar:"hi"
        # Note that these are the variables
        # declared in the data section above.

Code within the procedure section is executed from top to bottom,
skipping sub-procedure declarations, unless they are explicitly called.

Available statements and sub-procedure declarations will be explained further
in the following sections of this document.

## Sub-procedures
A **sub-procedure** is a piece of code that can be called and executed from other
parts of the script. Sub-procedure subsections must be declared within the
**procedure** section of the code using a `sub-procedure <name>` statement and
end with an `end sub-procedure` statement. Alternatively, the shorter versions
`sub <name>` and `end sub` may be used. Bear in mind that you can't define a
sub-procedure within a sub-procedure. 

Sub-procedures may be invoked at any point in your code by **calling** them with
the `call` statement. When you do that, the sub-procedure is executed and once
it finishes executing, the line after the `call` that called the sub-procedure
is executed and execution continues normally. More on the call statement later.

Also bear in mind that you can `call` a
sub-procedure before it has been declared, but the compilation process will
fail if the compiler doesn't find the sub-procedure once it has parsed all the
files in your program.

The full syntax for declaring sub-procedures is this one:

    :::coffeescript
    sub-procedure procedureName
        parameters:
            # parameters go here
        local data:
            # local variable declarations go here
        procedure:
            # code goes here
    end sub-procedure
    
Or you may, as stated, use the shorter version:

    :::coffeescript
    sub procedureName
        parameters:
            # parameters go here
        local data:
            # local variable declarations go here
        procedure:
            # code goes here
    end sub
    
In context, the full declaration of a sub-procedure looks like this:

    :::coffeescript
    data:
        # ...
    procedure:
        # ...
        sub mySubprocedure
            parameters:
                # ...
            local data:
                # ...
            procedure:
                # ...
        end sub

The **parameters** and local **data** sub-sections are optional (more on these
later). If you decide to not include any of those sub-sections, you can skip
the **procedure** tag altogether and just go ahead writing your code like this:

    :::coffeescript
    sub someOtherSub
        display "Hello there!" lf
    end sub

You cannot have more than one sub-procedure with the same name. Also,
sub-procedure names must follow the guidelines stated in the **Naming Schemes**
section of this document.

### The Procedure Subsection:

In the **procedure** sub-section of the sub-procedure you may write the code
of your sub-procedure using statements like in the main **procedure** section.
In this procedure sub-section, however, you may also use the `RETURN` statement
to halt execution of the sub-procedure and return to the point where it was
called from.

### The Parameters and Local Data Sub-Sections

Within the **parameters** and **data** sub-sections of a function you may only
declare variables just like in the global **data** section. The variables
defined here, however, can only be used inside the procedure sub-section of the
same sub-procedure they where declared in.

Variables between the parameters and local data sub-sections may not share
names.

If a variable declared within the parameters or data sub-sections of
a sub-procedure shares its name with a global variable, when using that name 
within the procedure section of the sub-procedure, it will always refer to the
variable declared in said sub-procedure and not the global one.

At the start of the sub-procedure execution, all the variables declared in its
**local data** section will be initialized with their type default values, and
each invocation of the function will have its own copy of the local variables.
This is important if you want to implement recursive sub-procedures:

    :::coffeescript
    data:
        execution is number
    procedure:
        sub myRecursiveSub
            local data:
                myLocalVar is number
            procedure:
                in executions solve executions + 1
                if executions is equal to 3 then
                    return # We don't want this to run forever!
                end if
                display "[myLocalVar starts at " myLocalVar "!"
                store executions in myLocalVar
                call myRecursiveSub
                display "I'm execution n°" myLocalVar "!]"
        end sub
        
        call myRecursiveSub
        
That weird recursive function displays the following output:

    :::text
    [myLocalVar starts at 0![myLocalVar starts at 0!I'm execution Nº2!]I'm execution Nº1!]
    
!!!hint
    Most of the statements used in this example will be explained later.
    Don't feel bad if you don't yet understand completely what that sub-procedure
    does.

Variables declared within the **parameters** sub-section are quite different.
When calling sub-procedures using the `call` statement, you may also use the
optional `with` keyword to specify values to be passed to the sub-procedure.
The variables declared in the **parameters** sub-section will take these values,
following the order they were declared in.

For example, if you declare a sub-procedure like this:

    :::coffeescript
    sub addTwoNumbers
        parameters:
            a is number
            b is number
            c is number
        procedure:
            # ...
    end sub
    
You may then call it like this:

    :::coffeescript
    call addTwoNumbers with 5 6 7
    
And `a` will take the value 5, `b` the value 6 and `c` the value 7.

While this is fine, it gets more powerful when using variables instead of
fixed values. LDPL is a pass-by-reference language. This means that if you
pass a variable to a sub-procedure with the `with` keyword and its value is
assigned to a parameter variable, if you modify that parameter variable the
original variable will be modified as well. Let's see an example:

    :::coffeescript
    data:
        result is number
    
    procedure:
        sub addTwoNumbers
            parameters:
                a is number
                b is number
                c is number
            procedure:
                in c solve a + b
        end sub

        # the variable result is initialized to 0 by default
        call addTwoNumbers with 4 5 result
        display "The result is: " result "." lf
        
!!!hint
    The `in _ solve _` statement is used to solve mathematical expressions.
    If you execute, for example, `in foo solve 5 + 6 - 8`, the result of solving
    5 + 6 - 8 will be stored in the number variable `foo`.
        
        
That code displays the following text:

    :::text
    The result is 9.
    
This is because, as we called the variable with a variable as a parameter
(`call addTwoNumbers with 4 5 result`, **result** is the variable here) it was
*somewhat* aliased to the local parameter variable `c` and, thus, when we
solved `a + b` in `c`, we stored the result of `a + b` in `result`.

Passing variables by reference lets you return results from your sub-procedure,
as shown in the example above.

## Parallel Sub-procedures

!!! info
    This feature is currently only available in LDPL 4.5-dev *Groovy Gualicho*.

Starting from release 4.5 *Groovy Gualicho*, LDPL supports **Parallel Sub-procedures**.
Parallel sub-procedures are procedures that you can -as the name suggests- run in
parallel to your main `procedure:` section code. Technically speaking, parallel
sub-procedures are sub-procedures that run on a different thread to the main one of your
program.

    :::coffeescript
    procedure:
    parallel sub myParallelSub
        #...
    end sub

Parallel sub-procedures differ from traditional sub-procedures in that they cannot
have a `parameters:` section (and, thus, they cannot receive parameters). Instead,
the **number** variable `parameter_id` is declared by default inside each parallel
sub-procedure. The `parameter_id` variable holds a numberic value that identifies
the running parallel sub-procedure. Every time a parallel sub-procedure is called,
a different `parameter_id` is assigned to each call. If you call the same parallel
sub-procedure three times, you will end up with three instances of the parallel
sub-procedure running at the same time, each with a different `parameter_id` value.

Parallel sub-procedures can only be called using the `in _ call parallel _` statement.
Check the **Control Flow Statements** section of this document for more information.

If you want your current procedure to stop and wait until a parallel sub-procedure has
finished executing, use the `wait for parallel _` statement. Check the **Control Flow
Statements** section of this document for more information.

If you want to stop a running parallel sub-procedure, use the `stop parallel _` statement.
Check the **Control Flow Statements** section of this document for more information.

    :::coffeescript
    data:
        printNumbers_id is number

    procedure:
        parallel sub printNumbers
            local data:
                n is number
            procedure:
                while 1 is equal to 1 do
                    in n solve n + 1
                    display n crlf
                repeat
        end sub

        in printNumbers_id call parallel printNumbers
        wait 5000 milliseconds
        stop parallel printNumbers_id