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
    : add                                                           # add_nop
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
    :  ('!' | '~' | '-' | '*' | '&') unary                          # unaryOp
    | '(' expr ')'                                                  # autoParen
    | Interger                                                      # integer 
    ;