grammar MiniDecaf;

import CommonLex;

prog
    : func EOF
    ;

func
    : type Identifier '(' ')' '{' stmt* '}'
    ;

stmt
    : 'return' expr ';'                                             # returnStmt
    | expr? ';'                                                     # expr_nop
    | declaration                                                   # declaration_nop
    ;

expr
    : assign_op                                                     # assign_nop
    ;

declaration
    : type Identifier ('=' expr)? ';'                               # declare
    ; 

assign_op
    : lor_op                                                        # lor_nop
    | Identifier '=' expr                                           # assign
    ;

lor_op
    : lor_op '||' lor_op                                            # lor
    | land_op                                                       # land_nop
    ;

land_op
    : land_op '&&' land_op                                          # land
    | equ                                                           # equ_nop
    ;

equ
    : equ ('==' | '!=') equ                                         # equal
    | rel                                                           # rel_nop
    ;

rel
    : rel ('<=' | '>=' | '<' | '>') rel                             # legt
    | add                                                           # add_nop
    ;

add 
    : add ('+' | '-') add                                           # addSub
    | mul                                                           # mul_nop
    ;

mul 
    : mul ('*' | '/' | '%') mul                                     # mulDiv
    | unary                                                         # factor_nop
    ;

unary   
    : ('!' | '~' | '-' | '*' | '&') unary                          # unaryOp
    | primary                                                      # primary_nop
    ;

primary
    : '(' expr ')'                                                  # autoParen
    | Interger                                                      # integer 
    ;

type
    : 'int'                                                         # intType
    ;