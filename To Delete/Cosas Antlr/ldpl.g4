grammar ldpl;

// +-----------------------------------------+ //
// |               Lexer Rules               | //
// +-----------------------------------------+ //

MULTICOMMENT    : '/*' .*? '*/' -> skip;
COMMENT         : '#' .*? '\n' -> skip;
WHITESPACE      : (' ' | '\t' | '\r') -> skip;
NEWLINE         : '\n';
// <Reserved words>
LINEFEED        : 'lf';
CRLF            : 'crlf';
OS_NAME         : 'linux' | 'macos' | 'android' | 'bsd' | 'emscripten';
NUMBERTYPE      : 'number' | 'numbers';
TEXTTYPE        : 'text' | 'texts';
LISTTYPE        : 'list' | 'lists';
MAPTYPE         : 'map' | 'maps';
RWOF            : 'of';
RWIS            : 'is';
RWFLAG          : 'flag';
RWINCLUDE       : 'include';
RWEXTENSION     : 'extension';
RWSUBPR         : 'sub' | 'sub-procedure';
RWDATA          : 'data:';
RWLOCAL         : 'local';
RWPROCEDURE     : 'procedure:';
RWPARAMETERS    : 'parameters:';
RWEND           : 'end';
RWCALL          : 'call';
RWWITH          : 'with';
// </Reserved words>
NUMBER          : ('+'?'-'?)* [0-9]+ ('.' [0-9]+);
PLUS_OP         : '+';
MINUS_OP        : '-';
TIMES_OP        : '*';
LPAR            : '(';
RPAR            : ')';
DIV_OP          : '/';
DQUOTE          : '"';
DCHAR           : ~["] | ESCAPEDDQUOTE ;
ESCAPEDDQUOTE   : '\\"';
STRING          : (DQUOTE DCHAR* DQUOTE);
IDENTIFIER      : [^:"()+\-*/]+;


// +------------------------------------------+ //
// |               Parser Rules               | //
// +------------------------------------------+ //

lapl_source             
        : init_block? data_block? procedure_block EOF
        ;

// Init section  
init_block
        : (init_statement NEWLINE)*
        ;
init_statement
        : include_statement
        | flag_statement
        ;
include_statement
        : RWINCLUDE STRING
        ;
flag_statement
        : RWFLAG OS_NAME? STRING
        ;
extension_statement
        : RWEXTENSION STRING
        ;

// Data section
data_block
        :
        | RWDATA NEWLINE (data_statement NEWLINE)*
        ;
data_statement
        : IDENTIFIER RWIS ((LISTTYPE|MAPTYPE) RWOF)+ (NUMBERTYPE|TEXTTYPE)
        | IDENTIFIER RWIS (NUMBERTYPE|TEXTTYPE) (LISTTYPE|MAPTYPE)+
        | IDENTIFIER RWIS (NUMBERTYPE|TEXTTYPE)
        ;

// Procedure section
procedure_block
        :
        | RWPROCEDURE NEWLINE (procedure_statement_parent)*
        ;
procedure_statement_parent
        : sub_procedure_block
        | procedure_statement NEWLINE
        ;
sub_procedure_block
        : RWSUBPR IDENTIFIER NEWLINE parameters_block? local_data_block? local_procedure_block? RWEND RWSUBPR NEWLINE
        | RWSUBPR IDENTIFIER NEWLINE (procedure_statement NEWLINE)* RWEND RWSUBPR NEWLINE
        ;
parameters_block
        : RWPARAMETERS NEWLINE (data_statement NEWLINE)*
        ;
local_data_block
        : RWLOCAL RWDATA NEWLINE (data_statement NEWLINE)*
        ;
local_procedure_block
        : RWPROCEDURE NEWLINE (procedure_statement NEWLINE)*
        ;
procedure_statement
        : call_statement
        ;
call_statement
        : RWCALL IDENTIFIER
        | RWCALL IDENTIFIER RWWITH ((NUMBER|STRING|IDENTIFIER))+
        ;


/*
statement               
        : block
        | line_statement
        | empty_line
        ;
empty_line
        : SEMICOLON
        ;
block                   
        : BLOCK_OPEN statement* BLOCK_CLOSE
        | while_block
        | if_block
        | function_declaration
        ;
line_statement
        : 
        ( assignment
        | function_call
        | continue_statement
        | break_statement
        | exit_statement
        | display_statement
        | value
        | return_statement
        ) SEMICOLON
        ;
string
        : STRING
        | STR_OP number_expression
        | LINEFEED
        | CRLF
        ;
number
        : PLUS_OP* MINUS_OP* NUMBER 
        | NUM_OP string_expression
        | TYPE value
        ;
number_expression
        : number
        | builtin_number_function
        | function_call
        | VARIABLE
        | CEIL_OP number_expression
        | FLOOR_OP number_expression
        | number_expression POW_OP number_expression
        | number_expression MOD_OP number_expression
        | number_expression DIV_OP number_expression
        | number_expression TIMES_OP number_expression
        | number_expression MINUS_OP number_expression
        | number_expression PLUS_OP number_expression
        | LPAR number_expression RPAR
        ; //Precedence goes from bottom (higher) to top (lower)
string_expression
        : string
        | builtin_string_function
        | function_call
        | VARIABLE
        | string_expression INDEX_ACCESS_O number_expression INDEX_ACCESS_C
        | string_expression INDEX_ACCESS_O number_expression COMMA number_expression INDEX_ACCESS_C
        | string_expression CONCAT_OP string_expression
        | LPAR string_expression RPAR
        ; 
builtin_number_function
        : BIF_LEN LPAR string_expression RPAR
        | RANDOM LPAR RPAR
        ;
builtin_string_function
        : ACCEPT LPAR RPAR
        | BIF_REPLACE LPAR string_expression COMMA string_expression COMMA string_expression RPAR
        ;
builtin_boolean_function
        : BIF_ISNUM LPAR string_expression RPAR
        ;
/*array //TODO check
        : ARRAY
        | VARIABLE INDEX_ACCESS_O value INDEX_ACCESS_C
        | function_call INDEX_ACCESS_O value INDEX_ACCESS_C
        ;
map //TODO check
        : MAP
        | VARIABLE BLOCK_OPEN value BLOCK_CLOSE
        | function_call BLOCK_OPEN value BLOCK_CLOSE
        ;
array_expression
        : array
        | VARIABLE
        | function_call
        ;
map_expression
        : map
        | VARIABLE
        | function_call
        ;
value
        : VARIABLE
        | function_call
        | string_expression
        | number_expression
        | boolean_expr
        //| array_expression
        //| map_expression
        ;
argument
        : value
        //| REF_OP VARIABLE
        ;
assignment
        : VARIABLE ASSIGN_OP value
        | VAR VARIABLE ASSIGN_OP value
        | VAR VARIABLE
        //| VARIABLE INDEX_ACCESS_O value INDEX_ACCESS_C ASSIGN_OP value
        ;
function_call
        : IDENTIFIER LPAR argument (COMMA argument)* RPAR
        ;
while_block
        : WHILE boolean_expr statement
        ;
boolean_value
        : TRUE
        | FALSE
        ;
boolean_expr
        : boolean_value
        | builtin_boolean_function
        | function_call
        | VARIABLE
        | NOT_OP boolean_expr
        | string_expression EQ_OP string_expression
        | string_expression NEQ_OP string_expression
        | string_expression LT_OP string_expression
        | string_expression GT_OP string_expression
        | string_expression LE_OP string_expression
        | string_expression GE_OP string_expression
        | number_expression EQ_OP number_expression
        | number_expression NEQ_OP number_expression
        | number_expression LT_OP number_expression
        | number_expression GT_OP number_expression
        | number_expression LE_OP number_expression
        | number_expression GE_OP number_expression
        | boolean_expr EQ_OP boolean_expr
        | boolean_expr NEQ_OP boolean_expr
        | boolean_expr OR_OP boolean_expr
        | boolean_expr AND_OP boolean_expr
        | LPAR boolean_expr RPAR
        ;
if_block
        : IF boolean_expr statement else_block?
        ;
else_block
        : ELSE statement
        ;
continue_statement
        : CONTINUE
        ;
break_statement
        : BREAK
        ;
return_statement
        : RETURN
        ;
exit_statement
        : EXIT
        | EXIT number_expression
        ;
function_declaration
        : FUNCTION IDENTIFIER LPAR (VARIABLE (COMMA VARIABLE)*)? RPAR statement
        ;
display_statement
        : DISPLAY display_values
        ;
display_values
        : value+
        ;

FEATURES TO BE ADDED (in no particular order):
        BOOLEAN EXPRESSIONS WITH VARIABLES OR FUNCTIONS ON BOTH SIDES
        arrays
        maps
        pass by reference for maps and arrays
        copy maps and arrays
        include
        SCRIPTDIR
        try - except
        try without except
        sleep
        execute
        file saving, loading and appending (and reading line by line)
        trigonometric functions
        split

features to be *maybe* added:
        ternary operator
        for each
        for
        namespaces
        fork
        mutex
        semaphores

already added:
        == and != for booleans
        string length
        random number
        isNumeric(string)
        accept()
        What happens if num "aslkdj" is executed?
        utf8 support
        replace
        Negative values in string slices
        String slices to infinity
        real return statement

functions to be added to the stdlib:
        string uppercase
        string lowercase
*/