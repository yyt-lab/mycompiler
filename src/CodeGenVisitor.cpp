#include "CodeGenVisitor.h"
#include "Allocator.h"
#include "error.hpp"
#include "type.hpp"
#include "riscv_md.hpp"
#include "queue"
#include "vector"

CodeGenVisitor::CodeGenVisitor(MachineDesc *md)
{
    tr = new TransHelper(md);
}

// @brief: Start visiting the syntax tree from root node Prog
// @ret: Generated asm code
antlrcpp::Any CodeGenVisitor::visitProg(MiniDecafParser::ProgContext *ctx, symTab<Temp>& symbol_) 
{
    varTab = symbol_;
    visitChildren(ctx);
    return nullptr;
}

// three sugars for parameter offset management
class OffsetCounter;
#define RESET_OFFSET() tr->getOffsetCounter()->reset(OffsetCounter::PARAMETER)
#define NEXT_OFFSET(x) tr->getOffsetCounter()->next(OffsetCounter::PARAMETER, x)

antlrcpp::Any CodeGenVisitor::visitFunc(MiniDecafParser::FuncContext *context)
{
    Function *fun = new Function(context->Identifier()->getText(), BaseType::Int, new Location(-1));
    // attaching function entry label
    curFunc = context->Identifier()->getText();
    blockDepth = 0;
    blockOrder = 0;

    funcLabel[curFunc] = tr->getNewEntryLabel(fun);
    fun->attachEntryLabel(funcLabel[curFunc]);

    //arguments
    RESET_OFFSET();
    visit(context->parameter_list());

    // return visit(context->stmt(0));
    tr->startFunc(fun);
    visit(context->compound_statement());
    // visitChildren(context);
    tr->genReturn(tr->genLoadImm4(0)); // Return 0 by default
    tr->endFunc();
    return nullptr;
}

antlrcpp::Any CodeGenVisitor::visitPrimary_nop(MiniDecafParser::Primary_nopContext *context)
{
    return visitChildren(context);
}


// @brief: Visit ReturnStmt node, son of Stmt node
antlrcpp::Any CodeGenVisitor::visitReturnStmt(MiniDecafParser::ReturnStmtContext *ctx) {
    Temp retVal = visit(ctx->expr());
    if (retVal != nullptr)
        tr->genReturn(retVal); // Return 0 by default
    
    // tr->endFunc();
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

antlrcpp::Any CodeGenVisitor::visitDeclare(MiniDecafParser::DeclareContext *context)
{
    std::string varName = context->Identifier()->getText();
    varTab[curFunc][varName] = tr->getNewTempI4();
    if (context->expr()) {
        Temp val = visitChildren(context->expr());
        Temp dst = varTab[curFunc][varName];
        tr->genAssign(dst, val);
    }
    return nullptr;
}

antlrcpp::Any CodeGenVisitor::visitAssign(MiniDecafParser::AssignContext *context)
{
    // 变量
    std::string varName = context->Identifier()->getText();
    Temp val = nullptr;
    Temp dst = nullptr;
    std::string blockName = curFunc;   
    if (context->Lbrkt(0) == nullptr) {
        val = visit(context->expr(0));

        for (int i=blockDepth; i>=0; i--) {
            if (varTab[blockName].count(varName) == 0 || varTab[blockName][varName] == nullptr) {
                int index = blockName.find_last_of('@');
                blockName = blockName.substr(0,index);
                continue;
            }
            dst = varTab[blockName][varName];
            break;
        }
        if (dst != nullptr) {
            tr->genAssign(dst, val);
        }

        if (dst == nullptr && GlobalTab[varName] == true) {
            Temp dst = tr->genLoadSymbol(varName);
            tr->genStore(val, dst, 0);
        }
    } 
    else { // 数组
        std::string arrayName = context->Identifier()->getText();
        std::vector<int> dimVal;
        // ArraySymType arType = Allocator::getInstance().arrayTable[curFunc][arrayName];
        symTab<ArraySymType> arrayTable= Allocator::getInstance().arrayTable;
        ArraySymType arType{0};

        // 寻找数组定义
        for (int i=blockDepth; i>=0; i--) {
            if (arrayTable[blockName].count(arrayName) == 0 || arrayTable[blockName][arrayName].getSize() == 0) {
                int index = blockName.find_last_of('@');
                blockName = blockName.substr(0,index);
                continue;
            }
            arType = arrayTable[blockName][arrayName];
            break;
        }

        if (arType.getSize() != 0){ // 局部数组
            dimVal = arType.getDim();
        } else {  // 全局数组
            ArraySymType globalArType= Allocator::getInstance().arrayTable["global"][arrayName];
            dimVal = globalArType.getDim();
        }
        // 偏移参数tmp
        Temp tmp = visit(context->expr(0));
        int i;
        for (i=1; i< context->expr().size()-1; i++) {
            Temp k = tr->genLoadImm4(dimVal[i]);  // 因子
            tmp = tr->genMul(tmp, k);  // 相乘
            Temp t = visit(context->expr(i)); 
            tmp = tr->genAdd(tmp, t);  // 相加
        }
        Temp k = tr->genLoadImm4(4);
        tmp = tr->genMul(tmp, k);
        // 右值的value
        val = visit(context->expr(i));

        // 赋值
        if (arType.getSize() != 0){ // 局部数组
            Temp startNode = arType.getStartNode();
            tmp = tr->genAdd(startNode, tmp);
            tr->genStore(val, tmp, 0);
        } else {
            Temp dst = tr->genLoadSymbol(arrayName);
            tmp = tr->genAdd(dst, tmp);
            tr->genStore(val, tmp, 0);
        }


    }
   
    return val;

}

antlrcpp::Any CodeGenVisitor::visitIdentifier(MiniDecafParser::IdentifierContext *context)
{
    std::string varName = context->Identifier()->getText();
    std::string blockName = curFunc; 
    Temp dst = nullptr;

    for (int i=blockDepth; i>=0; i--) {
        if (varTab[blockName].count(varName) == 0 || varTab[blockName][varName] == nullptr) {
            int index = blockName.find_last_of('@');
            blockName = blockName.substr(0,index);
            continue;
        }
        dst = varTab[blockName][varName];
        break;
    }

    if (dst == nullptr && GlobalTab[varName] == true) {
        // dst = tr->genLoadSymbol(varName);
        Temp tmp = tr->genLoadSymbol(varName);
        dst = tr->genLoad(tmp, 0);
    }
    
    return dst;
}

antlrcpp::Any CodeGenVisitor::visitConditionalExpr(MiniDecafParser::ConditionalExprContext *context)
{
    Label L1 = tr->getNewLabel(); // entry of the false branch
    Label L2 = tr->getNewLabel(); // exit

    Temp cond_val = visit(context->lor_op()); // 条件值
    Temp retVal = tr->getNewTempI4(); // 返回值


    tr->genJumpOnZero(L1, cond_val); // 失败时，跳转到L1（else）

    // true branch begin
    tr->genAssign(retVal, visit(context->expr()));
    tr->genJump(L2); 
    // true branch end

    // false branch begin
    // retVal = visit(context->conditional());
    tr->genMarkLabel(L1);
    tr->genAssign(retVal, visit(context->conditional()));
    
    tr->genMarkLabel(L2);
    
    return retVal; 
    // false branch end

}

antlrcpp::Any CodeGenVisitor::visitIfStmt(MiniDecafParser::IfStmtContext *context)
{
    Label L1 = tr->getNewLabel(); // entry of the false branch
    Label L2 = tr->getNewLabel(); // exit

    
    Temp cond_val = visit(context->expr());

    if (context->Else()) {
        tr->genJumpOnZero(L1, cond_val); // 失败时，跳转到L1（else）

        // true branch 
        visit(context->stmt(0));
        tr->genJump(L2); // true branch执行结束

        // false branch 
        tr->genMarkLabel(L1);
        visit(context->stmt(1));

        // end if 
        tr->genMarkLabel(L2);
    } else {
        tr->genJumpOnZero(L2, cond_val); // 失败时，跳转到L1（else）

        // true branch begin
        visit(context->stmt(0));
        tr->genJump(L2); // true branch执行结束
        // true branch end

        // end branch
        tr->genMarkLabel(L2);
    }
    return nullptr;
}

antlrcpp::Any CodeGenVisitor::visitBlock(MiniDecafParser::BlockContext *context)
{
    blockDepth++;
    std::string blockName = curFunc;
    curFunc += "@" + std::to_string(blockOrder) + std::to_string(blockDepth);

    for (auto item : context->block_item()) {
        visit(item);
    }
    if (--blockDepth == 1) {
        ++blockOrder;
    }
    int pos = curFunc.find_last_of('@');
    curFunc = curFunc.substr(0, pos);
    return nullptr;

}

antlrcpp::Any CodeGenVisitor::visitForLoop(MiniDecafParser::ForLoopContext *context)
{
    blockDepth++;
    std::string blockName = curFunc;
    curFunc += "@" + std::to_string(blockOrder) + std::to_string(blockDepth);
    
    Label L1 = tr->getNewLabel(); // entry of the begin forloop
    Label L2 = tr->getNewLabel(); // entry of the body forloop
    Label L3 = tr->getNewLabel(); // entry of the end forloop
    breakLabelStack.push(L3);
    continueLabelStack.push(L2);
    int base = 0;
    if (context->declaration()) {
        visit(context->declaration());
        base = -1;
    } else if (context->expr(0)) {
        visit(context->expr(0));
    } 
    tr->genMarkLabel(L1); // start label

    if (context->expr(base+1)){
        Temp tmpVal = visit(context->expr(base+1)); // 条件跳转
        tr->genJumpOnZero(L3, tmpVal);
    }
    // tr->genJump(L3);  // 条件跳转失败，直接跳出loop
    visit(context->stmt());

    tr->genMarkLabel(L2);  // continue label
    if (context->expr(base+2)){
        visitChildren(context->expr(base+2)); // e.g., i++
    }

    tr->genJump(L1);

    tr->genMarkLabel(L3);  // break label

    breakLabelStack.pop();
    continueLabelStack.pop();
    if (--blockDepth == 0) {
        ++blockOrder;
    }
    int pos = curFunc.find_last_of('@');
    curFunc = curFunc.substr(0, pos);
    return nullptr;
}

antlrcpp::Any CodeGenVisitor::visitWhileLoop(MiniDecafParser::WhileLoopContext *context)
{
    Label L1 = tr->getNewLabel(); // entry of the begin forloop
    // Label L2 = tr->getNewLabel(); // entry of the body forloop
    Label L3 = tr->getNewLabel(); // entry of the end forloop
    breakLabelStack.push(L3);
    continueLabelStack.push(L1);

    tr->genMarkLabel(L1); // start while loop
    if (context->expr() != nullptr){
        Temp tmpVal = visit(context->expr()); // 不满足时跳转到结束
        tr->genJumpOnZero(L3, tmpVal);
    }
    // tr->genMarkLabel(L2);
    visit(context->stmt());
    tr->genJump(L1);
    tr->genMarkLabel(L3);

    breakLabelStack.pop();
    continueLabelStack.pop();
    return nullptr;
}
antlrcpp::Any CodeGenVisitor::visitDoWhileLoop(MiniDecafParser::DoWhileLoopContext *context)
{
    Label L1 = tr->getNewLabel(); // entry of the begin forloop
    Label L3 = tr->getNewLabel(); // entry of the end forloop
    breakLabelStack.push(L3);
    continueLabelStack.push(L1);

    tr->genMarkLabel(L1);
    visit(context->stmt());

    if (context->expr() != nullptr) {
        Temp tmpVal = visit(context->expr()); // 不满足时跳转到结束
        tr->genJumpOnZero(L3, tmpVal);
    }
    tr->genJump(L1);
    tr->genMarkLabel(L3);

    breakLabelStack.pop();
    continueLabelStack.pop();
    return nullptr;
}
antlrcpp::Any CodeGenVisitor::visitBreak(MiniDecafParser::BreakContext *context)
{
    tr->genJump(breakLabelStack.top());
    return nullptr;
}
antlrcpp::Any CodeGenVisitor::visitContinue(MiniDecafParser::ContinueContext *context)
{
    tr->genJump(continueLabelStack.top());
    return nullptr;
}


antlrcpp::Any CodeGenVisitor::visitFuncCall(MiniDecafParser::FuncCallContext *context)
{
    // return nullptr;
    // for (auto i = 0; i < context->expr().size(); ++i){
    //     Temp tmp = visit(context->expr(i));
    //     tr->genParam(tmp);
    // }
    std::queue<Temp> _queue;
    for (auto i = 0; i < context->expr().size(); ++i){
        Temp tmp = visit(context->expr(i));
        _queue.push(tmp);
        // tr->genParam(tmp);
    }
    while (_queue.empty() == false) {
        tr->genParam(_queue.front());
        _queue.pop();
    }

    Temp retVal = tr->genCall(funcLabel[context->Identifier()->getText()]);
    assert(funcLabel[context->Identifier()->getText()] != NULL);
    return retVal;
}

antlrcpp::Any CodeGenVisitor::visitParameter_list(MiniDecafParser::Parameter_listContext *context)
{
    for (auto i = 0; i < context->Identifier().size(); ++i) {
        std::string varName = context->Identifier(i)->getText();
        // Temp tmp = tr->genLoad();
        Temp tmp = tr->getNewTempI4();
        tmp->offset = NEXT_OFFSET(tmp->size);
        tmp->is_offset_fixed = true;
        varTab[curFunc][varName] = tmp;
    }
    return nullptr;
}

antlrcpp::Any CodeGenVisitor::visitGlobalVar(MiniDecafParser::GlobalVarContext *context)
{
    std::string varName = context->Identifier()->getText();
    Temp tmp;
    if (context->expr()) {
        Temp val = visitChildren(context->expr());
        // Temp dst = varTab[curFunc][varName];
        tr->genGlobalVarible(varName, std::stoi(context->expr()->getText()));
    } else {
        tr->genGlobalVarible(varName, 0);
    }
    GlobalTab[varName] = true;
    // varTab["global"][varName] = 
    // if (context->expr()) {
    //     Temp val = visitChildren(context->expr());
    //     Temp dst = varTab[curFunc][varName];
    //     tr->genAssign(dst, val);
    // }
    return nullptr;
}
// 右值访问数组
antlrcpp::Any CodeGenVisitor::visitArrayIndex(MiniDecafParser::ArrayIndexContext *context)
{
    std::string arrayName = context->Identifier()->getText();
    std::vector<int> dimVal;
    symTab<ArraySymType> arrayTable= Allocator::getInstance().arrayTable;
    ArraySymType arType{0};
    // ArraySymType arType= Allocator::getInstance().arrayTable[curFunc][arrayName];
    Temp ret = nullptr;
    std::string blockName = curFunc; 
    
    for (int i=blockDepth; i>=0; i--) {
        if (arrayTable[blockName].count(arrayName) == 0 || arrayTable[blockName][arrayName].getSize() == 0 ) {
            int index = blockName.find_last_of('@');
            blockName = blockName.substr(0,index);
            continue;
        }
        arType = arrayTable[blockName][arrayName];
        break;
    }

    if (arType.getSize() != 0){ // 局部数组
        dimVal = arType.getDim();
    } else {  // 全局数组
        ArraySymType globalArType= Allocator::getInstance().arrayTable["global"][arrayName];
        dimVal = globalArType.getDim();
    }
    // 偏移参数tmp
    Temp tmp = visit(context->expr(0));
    int i;
    for (i=1; i< context->expr().size(); i++) {
        Temp k = tr->genLoadImm4(dimVal[i]);  // 因子
        tmp = tr->genMul(tmp, k);  // 相乘
        Temp t = visit(context->expr(i)); 
        tmp = tr->genAdd(tmp, t);  // 相加
    }
    Temp k = tr->genLoadImm4(4);
    tmp = tr->genMul(tmp, k);

    // 赋值
    if (arType.getSize() != 0){ // 局部数组
        Temp startNode = arType.getStartNode();
        tmp = tr->genAdd(startNode, tmp);
        ret = tr->genLoad(tmp, 0);
    } else {
        Temp dst = tr->genLoadSymbol(arrayName);
        tmp = tr->genAdd(dst, tmp);
        ret = tr->genLoad(tmp, 0);
    }
    return ret;
}
// 全局数组声明
antlrcpp::Any CodeGenVisitor::visitGlobalArrayDeclare(MiniDecafParser::GlobalArrayDeclareContext *context)
{
    std::string arrayName = context->Identifier()->getText();
    ArraySymType arType= Allocator::getInstance().arrayTable["global"][arrayName];
    tr->genGlobalArray(arrayName, nullptr, 0, arType.getSize());
    return nullptr;
}

// 局部数组
antlrcpp::Any CodeGenVisitor::visitArrayDeclare(MiniDecafParser::ArrayDeclareContext *context)
{
    std::string arrayName = context->Identifier()->getText();
    ArraySymType arType= Allocator::getInstance().arrayTable[curFunc][arrayName];
    Temp tmp = tr->allocNewTempI4(arType.getSize());
    Allocator::getInstance().arrayTable[curFunc][arrayName].setStartNode(tmp);
    return nullptr;
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