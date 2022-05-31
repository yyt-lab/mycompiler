grammar MiniDecaf;

import CommonLex;

prog
    : func* EOF
    ;

func
    : type Identifier '(' parameter_list ')' (compound_statement | ';')
    ;

parameter_list
    : (type Identifier (',' type Identifier)*)?
    ;

compound_statement
    : '{' block_item* '}'                                           # block
    ;

block_item
    : stmt                                                          # stmt_nop
    | declaration ';'                                               # declaration_nop
    ;

stmt
    : 'return' expr ';'                                             # returnStmt
    | expr? ';'                                                     # expr_nop
    | 'if' '(' expr ')' stmt ('else' stmt)?                         # ifStmt                                                   
    | compound_statement                                            # block_nop
    // | 'for' '(' expr? ';' expr? ';' expr? ')' stmt                  # forLoop
    | 'for' '(' (declaration | expr)? ';' (expr)? ';' (expr)?  ')' stmt            # forLoop
    | 'while' '(' expr ')' stmt                                     # whileLoop
    | 'do' stmt 'while' '(' expr ')' ';'                            # doWhileLoop
    | 'break' ';'                                                   # break
    | 'continue' ';'                                                # continue
    ;

declaration
    : type Identifier ('=' expr)?                                   # declare
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
    | postfix                                                      # postfix_nop
    ;

postfix
    : Identifier '(' (expr ',')* (expr)? ')'                       # funcCall
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