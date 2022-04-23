#include "CodeGenVisitor.h"
#include "Allocator.h"
// @brief: Start visiting the syntax tree from root node Prog
// @ret: Generated asm code
antlrcpp::Any CodeGenVisitor::visitProg(MiniDecafParser::ProgContext *ctx, symTab<int>& symbol_) {
    // std::cout<<ctx->func()->Identifier()->getText()<<std::endl;
    // if (ctx->func()->Identifier()->getText() != "main") return "";
    varTab = symbol_;
    code_ << ".section .text\n"
        << ".globl main\n"
        << "main:\n"; 
    visitChildren(ctx);
    return code_.str();
}

antlrcpp::Any CodeGenVisitor::visitPrimary_nop(MiniDecafParser::Primary_nopContext *context)
{
    visitChildren(context);
    return retType::INT;
}


// @brief: Visit ReturnStmt node, son of Stmt node
antlrcpp::Any CodeGenVisitor::visitReturnStmt(MiniDecafParser::ReturnStmtContext *ctx) {
    visit(ctx->expr());
    code_ << "\tret";
    return retType::UNDEF;
}

// @brief: Visit Integer node, which loads an immediate number into register
// @ret: Expr type: Int
antlrcpp::Any CodeGenVisitor::visitInteger(MiniDecafParser::IntegerContext *ctx) {
    code_ << "\tli a0, " << ctx->getText() << "\n"
          << push;
    return retType::INT;
}

// @brief: Visit addNop, which continue iterator
// @ret: 
antlrcpp::Any CodeGenVisitor::visitAdd_nop(MiniDecafParser::Add_nopContext *context){
    return visitChildren(context);
}

// @brief: Visit AutoParen, which continue iterator
// @ret: 
 antlrcpp::Any CodeGenVisitor::visitAutoParen(MiniDecafParser::AutoParenContext *context){
    return visit(context->expr());
}

// @brief: Visit Mul_nop, which continue iterator
// @ret: 
antlrcpp::Any CodeGenVisitor::visitMul_nop(MiniDecafParser::Mul_nopContext *context){
    return visitChildren(context);
}

//@brief: visit add sub
antlrcpp::Any CodeGenVisitor::visitAddSub(MiniDecafParser::AddSubContext *context){
    retType rl = visit(context->add(0));
    retType rr = visit(context->add(1));
    /* 
        ctx-><token_name>() is a ptr point to the token
        ptr == nullptr indicates that the token is undeclared
    */
    if (context->Addition()) {
        code_<<pop2
            <<"\tadd a0, t0, t1\n"
            << push;
        return retType::INT;
    }
    else if (context->Minus()) {
        code_<<pop2
            <<"\tsub a0, t1, t0\n"
            << push;
        return retType::INT;
    }
}

// @brief: Visit unaryOp_nop node. 
// @ret: Expr type: Int
antlrcpp::Any CodeGenVisitor::visitFactor_nop(MiniDecafParser::Factor_nopContext *context)
{
    return visitChildren(context);
}



// @brief: Visit unaryOp node, like '-1', '~12', '!89' and etc. 
// @ret: Expr type: Int
antlrcpp::Any CodeGenVisitor::visitUnaryOp(MiniDecafParser::UnaryOpContext *ctx) {
    visitChildren(ctx);
    /* 
        ctx-><token_name>() is a ptr point to the token
        ptr == nullptr indicates that the token is undeclared
    */
    if (ctx->Minus()) {
        code_ << pop1
            << "\tsub a0, x0, t0\n"
            << push;
        return retType::INT;
    } else if(ctx->Exclamation()) {
        code_ << pop1
            << "\tseqz a0, t0\n"
            << push; 
        return retType::INT;
    } else if(ctx->Tilde()) {
        code_ << pop1
            << "\tnot a0, t0\n"
            << push;
        return retType::INT;
    }
    return retType::UNDEF;
}

antlrcpp::Any CodeGenVisitor::visitMulDiv(MiniDecafParser::MulDivContext *context)
{
    retType rl = visit(context->mul(0));
    retType rr = visit(context->mul(1));

    if(context->Multiplication()) {
        code_ << pop2
            <<"\tmul a0, t0, t1\n"
            <<push;
        return retType::INT;
    }
    else if (context->Division()) {
        code_ << pop2
            <<"\tdiv a0, t1, t0\n"
            <<push;
        return retType::INT;
    } else if (context->Modulo()) {
        code_ << pop2
            <<"\trem a0, t1, t0\n"
            <<push;
        return retType::INT;
    } 
    return retType::UNDEF;
}


antlrcpp::Any CodeGenVisitor::visitLor_nop(MiniDecafParser::Lor_nopContext *context)
{
    return visitChildren(context);
}
antlrcpp::Any CodeGenVisitor::visitLand_nop(MiniDecafParser::Land_nopContext *context)
{
    return visitChildren(context);
}
antlrcpp::Any CodeGenVisitor::visitEqu_nop(MiniDecafParser::Equ_nopContext *context)
{
    return visitChildren(context);
}
antlrcpp::Any CodeGenVisitor::visitRel_nop(MiniDecafParser::Rel_nopContext *context)
{
    return visitChildren(context);
}
antlrcpp::Any CodeGenVisitor::visitLor(MiniDecafParser::LorContext *context)
{
    visit(context->lor_op(0));
    visit(context->lor_op(1));

    if (context->LOR()) {
        code_<<pop2
            <<"\tor a0, t0, t1\n"
            <<"\tsnez a0, a0\n"
            <<push;
        return retType::INT;
    }
    return retType::UNDEF;
}

antlrcpp::Any CodeGenVisitor::visitLand(MiniDecafParser::LandContext *context)
{
    visit(context->land_op(0));
    visit(context->land_op(1));

    if (context->LAND()) {
        code_<<pop2
            << "\tmul a0, t0, t1\n"
            << "\tsnez a0, a0\n"
            <<push;
        return retType::INT;
    }
    return retType::UNDEF;
}

antlrcpp::Any CodeGenVisitor::visitEqual(MiniDecafParser::EqualContext *context)
{
    visit(context->equ(0));
    visit(context->equ(1));

    if (context->EQ()) {
        code_<<pop2
            <<"\tsub t0, t0, t1\n"
            <<"\tseqz a0, t0\n";
    } else if (context->NEQ()) {
        code_<<pop2
            <<"\tsub t0, t0, t1\n"
            <<"\tsnez a0, t0\n";
    }
    code_<<push;
    return retType::INT;
}

antlrcpp::Any CodeGenVisitor::visitLegt(MiniDecafParser::LegtContext *context)
{
    visit(context->rel(0)); // -> TO t1
    visit(context->rel(1)); // -> TO t0

    code_<<pop2;
    if (context->LE()) {
        code_ << "\tsgt a0, t1, t0\n" // t1>t0
              << "\txori a0, a0, 1\n"; // get t1<=t0
    } else if (context->LT()) { // t1<t0
        code_ << "\tslt a0, t1, t0\n"; // t0>t1
    } else if (context->GE()) { // t1>=t0
        code_ << "\tslt a0, t1, t0\n" // t1<t0
              << "\txori a0, a0, 1\n"; // get t1>=t0
    } else if (context->GT()) {  // t1>t0
        code_ << "\tsgt a0, t1, t0\n"; // t1<t0
    }
    code_<<push;
    return retType::INT;
}