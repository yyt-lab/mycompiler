grammar MiniDecaf;

import CommonLex;

prog
    : func EOF
    ;

func
    : 'int' Identifier '(' ')' '{' stmt '}'
    ;

stmt
    : 'return' expr ';'                                             # returnStmt
    ;

expr
    : lor_op                                                        # lor_nop
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