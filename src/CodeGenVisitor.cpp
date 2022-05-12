#include "CodeGenVisitor.h"
#include "Allocator.h"
#include "error.hpp"
#include "type.hpp"

CodeGenVisitor::CodeGenVisitor()
{
    tr = new TransHelper();
}

// @brief: Start visiting the syntax tree from root node Prog
// @ret: Generated asm code
antlrcpp::Any CodeGenVisitor::visitProg(MiniDecafParser::ProgContext *ctx, symTab<int>& symbol_) {
    // std::cout<<ctx->func()->Identifier()->getText()<<std::endl;
    // if (ctx->func()->Identifier()->getText() != "main") return "";
    varTab = symbol_;
    visitChildren(ctx);
    return nullptr;
    // return code_.str();
}

antlrcpp::Any CodeGenVisitor::visitFunc(MiniDecafParser::FuncContext *context)
{
    Function *fun = new Function(context->Identifier()->getText(), BaseType::Int, new Location(-1));
    // attaching function entry label
    fun->attachEntryLabel(tr->getNewEntryLabel(fun));
    tr->startFunc(fun);
    return visit(context->stmt(0));

}

antlrcpp::Any CodeGenVisitor::visitPrimary_nop(MiniDecafParser::Primary_nopContext *context)
{
    return visitChildren(context);
}


// @brief: Visit ReturnStmt node, son of Stmt node
antlrcpp::Any CodeGenVisitor::visitReturnStmt(MiniDecafParser::ReturnStmtContext *ctx) {
    Temp retVal = visit(ctx->expr());
    tr->genReturn(retVal); // Return 0 by default
    
    tr->endFunc();
    return nullptr;
}

// @brief: Visit Integer node, which loads an immediate number into register
// @ret: Expr type: Int
antlrcpp::Any CodeGenVisitor::visitInteger(MiniDecafParser::IntegerContext *ctx) {
    // code_ << "\tli a0, " << ctx->getText() << "\n"
    //       << push;
    Temp intVal = tr->genLoadImm4(std::stoi(ctx->getText()));
    return intVal;
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
    Temp rl = visit(context->add(0));
    Temp rr = visit(context->add(1));
    /* 
        ctx-><token_name>() is a ptr point to the token
        ptr == nullptr indicates that the token is undeclared
    */
    if (context->Addition()) {
        Temp result = tr -> genAdd(rl, rr);
        return result;
    }
    else if (context->Minus()) {
        Temp result = tr -> genSub(rl, rr);
        return result;
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
antlrcpp::Any CodeGenVisitor::visitUnaryOp(MiniDecafParser::UnaryOpContext *context) {
    Temp rl = visit(context->unary());
    /* 
        ctx-><token_name>() is a ptr point to the token
        ptr == nullptr indicates that the token is undeclared
    */
    if (context->Minus()) {
        // code_ << pop1
        //     << "\tsub a0, x0, t0\n"
        //     << push;
        // return retType::INT;
        Temp zeroVal = tr->genLoadImm4(0);
        Temp result = tr -> genSub(zeroVal, rl);
        return result;
    } else if(context->Exclamation()) {
        Temp result = tr -> genLNot(rl);
        return result;
    } else if(context->Tilde()) {
        Temp result = tr -> genBNot(rl);
        return result;
    }
    std::cerr<<"current not support!"<<std::endl;
    mind_assert(false);
}

antlrcpp::Any CodeGenVisitor::visitMulDiv(MiniDecafParser::MulDivContext *context)
{
    Temp rl = visit(context->mul(0));
    Temp rr = visit(context->mul(1));

    if(context->Multiplication()) {
        Temp result = tr -> genMul(rl, rr);
        return result;
    }
    else if (context->Division()) {
        Temp result = tr -> genDiv(rl, rr);
        return result;
    } else if (context->Modulo()) {
        Temp result = tr -> genMod(rl, rr);
        return result;
    } 
    std::cerr<<"current not support!"<<std::endl;
    mind_assert(false);
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
    Temp rl = visit(context->lor_op(0));
    Temp rr = visit(context->lor_op(1));

    if (context->LOR()) {
        Temp result = tr -> genLOr(rl, rr);
        return result;
    }
    std::cerr<<"current not support!"<<std::endl;
    mind_assert(false);
}

antlrcpp::Any CodeGenVisitor::visitLand(MiniDecafParser::LandContext *context)
{
    // visit(context->land_op(0));
    // visit(context->land_op(1));
    Temp rl = visit(context->land_op(0));
    Temp rr = visit(context->land_op(1));

    if (context->LAND()) {
       Temp result = tr -> genLAnd(rl, rr);
       return result;
    }
    std::cerr<<"current not support!"<<std::endl;
    mind_assert(false);
}

antlrcpp::Any CodeGenVisitor::visitEqual(MiniDecafParser::EqualContext *context)
{
    Temp rl = visit(context->equ(0));
    Temp rr = visit(context->equ(1));

    if (context->EQ()) {
        Temp result = tr -> genEqu(rl, rr);
        return result;
    } else if (context->NEQ()) {
        Temp result = tr -> genNeq(rl, rr);
        return result;
    }
    std::cerr<<"current not support!"<<std::endl;
    mind_assert(false);
}

antlrcpp::Any CodeGenVisitor::visitLegt(MiniDecafParser::LegtContext *context)
{
    Temp rl = visit(context->rel(0));
    Temp rr = visit(context->rel(1));

    code_<<pop2;
    if (context->LE()) {
        Temp result = tr -> genLeq(rl, rr);
        return result;
        // code_ << "\tsgt a0, t1, t0\n" // t1>t0
        //       << "\txori a0, a0, 1\n"; // get t1<=t0
    } else if (context->LT()) { // t1<t0
        Temp result = tr -> genLes(rl, rr);
        return result;
        // code_ << "\tslt a0, t1, t0\n"; // t0>t1
    } else if (context->GE()) { // t1>=t0
        Temp result = tr -> genGeq(rl, rr);
        return result;
        // code_ << "\tslt a0, t1, t0\n" // t1<t0
        //       << "\txori a0, a0, 1\n"; // get t1>=t0
    } else if (context->GT()) {  // t1>t0
        Temp result = tr -> genGtr(rl, rr);
        return result;
        // code_ << "\tsgt a0, t1, t0\n"; // t1<t0
    }
    code_<<push;
    return retType::INT;
}


/* Translates an entire AST into a Piece list.
 *
 * PARAMETERS:
 *   tree  - the AST
 * RETURNS:
 *   the result Piece list (represented by the first node)
 */
Piece *CodeGenVisitor::translate() 
{
    // TransHelper *helper = new TransHelper(md);
    return tr->getPiece();
}

void CodeGenVisitor::DumpIR (std::ostream &os)
{
    Tac* tacPtr = tr->getTac();
    while(tacPtr != NULL) {
        tacPtr->dump(os);
        os<<std::endl;
        tacPtr = tacPtr->next;
    }
}