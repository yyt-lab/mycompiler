grammar MiniDecaf;

import CommonLex;

prog
    : func EOF
    ;

func
    : type Identifier '(' ')' '{' block_item* '}'
    ;

block_item
    : stmt                                                          # stmt_nop
    | declaration                                                   # declaration_nop
    ;

stmt
    : 'return' expr ';'                                             # returnStmt
    | expr? ';'                                                     # expr_nop
    | 'if' '(' expr ')' stmt ('else' stmt)?                         # ifStmt                                                   
    ;

declaration
    : type Identifier ('=' expr)? ';'                               # declare
    ; 

expr
    : assign_op                                                     # assign_nop
    ;


assign_op
    : conditional                                                   # conditional_nop
    | Identifier '=' expr                                           # assign
    ;

conditional
    : lor_op                                                        # lor_nop
    | lor_op '?' expr ':' conditional                               # conditionalExpr
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
    | Identifier                                                    # identifier
    ;

type
    : 'int'                                                         # intType
    ;