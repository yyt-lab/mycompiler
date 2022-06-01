#include "Allocator.h"

antlrcpp::Any Allocator::visitProg(MiniDecafParser::ProgContext *context)
{
    offest = 0;
    // blockDepth=-1;
    // blockOrder=0;
    visitChildren(context);
    return varTab;

}

antlrcpp::Any Allocator::visitFunc(MiniDecafParser::FuncContext *context)
{
    blockDepth=0;
    // blockDepth=-1;
    blockOrder=0;
    curFunc = context->Identifier()->getText();
    // 函数定义的情况
    if (!context->Semicolon()) { 
        if (funcTable.count(curFunc) > 0 && funcTable[curFunc] == true) { // 重复声明
            std::cerr << "[ERROR] Redefinition of function " << curFunc << std::endl;
            exit(1);
        } else { // 第一次声明
            funcTable[curFunc] = true;
            if (context->parameter_list())
                visit(context->parameter_list());
            visit(context->compound_statement());
        }
    } else {
        // 函数声明的情况
        funcTable[curFunc] = false;
    }
    // visitChildren(context);
    return retType::INT;
}

antlrcpp::Any Allocator::visitParameter_list(MiniDecafParser::Parameter_listContext *context)
{
    for (auto i = 0; i < context->Identifier().size(); ++i) {
        std::string varName = context->Identifier(i)->getText();
        varTab[curFunc][varName] = nullptr;
    }
    return retType::INT;
}

antlrcpp::Any Allocator::visitDeclare(MiniDecafParser::DeclareContext *context)
{
    std::string varName = context->Identifier()->getText();
    if (varTab[curFunc].count(varName) != 0) {
        std::cerr<<"multi_define :"<<varName<<std::endl;
    }
    varTab[curFunc][varName] = nullptr;
    offest++;
    return retType::INT;
}

antlrcpp::Any Allocator::visitAssign(MiniDecafParser::AssignContext *context)
{
    std::string varName = context->Identifier()->getText();
    bool find = false;
    std::string blockName = curFunc; 
    for (int i=blockDepth; i>=0; i--) {
        // if (varTab[blockName].count(varName) == 0) {
        if (varTab[blockName].find(varName) == varTab[blockName].end() ) {
            int index = blockName.find_last_of('@');
            blockName = blockName.substr(0,index);
            continue;
        }
        find = true;
    }

    if (find == false && varTab["global"].count(varName) > 0 ) {
        find = true;
    }

    if (find == false) {
        std::cerr<<"undefined variable name :"<<varName<<std::endl;
        assert(0);
    }
    return retType::INT;
}

antlrcpp::Any Allocator::visitBlock(MiniDecafParser::BlockContext *context)
{
    /*
    <function>@<blockOrder>@<blockDep>
        symbol table:
        main@0@0: 0
        main@1@1: 1
    */
    blockDepth++;
    curFunc += "@" + std::to_string(blockOrder) + std::to_string(blockDepth);
    for (auto item : context->block_item()) {
        visitChildren(item);
    }
    if (--blockDepth == 0) {
        blockOrder++;
    }

    int pos = curFunc.find_last_of('@');
    curFunc = curFunc.substr(0, pos);
    return retType::INT;
}

antlrcpp::Any Allocator::visitIdentifier(MiniDecafParser::IdentifierContext *context)
{
    std::string varName = context->Identifier()->getText();
    bool find = false;
    std::string blockName = curFunc; 

    for (int i=blockDepth; i>=0; i--) {
        // if (varTab[blockName].count(varName) == 0) {
        if (varTab[blockName].find(varName) == varTab[blockName].end() ) {
            int index = blockName.find_last_of('@');
            blockName = blockName.substr(0,index);
            continue;
        }
        find = true;
    }

    if (find == false && varTab["global"].count(varName) > 0 ) {
        find = true;
    }
    
    if (find == false) {
        std::cerr<<"undefined variable name :"<<varName<<std::endl;
        assert(0);
    }

    return retType::INT;
}

antlrcpp::Any Allocator::visitForLoop(MiniDecafParser::ForLoopContext *context)
{
    blockDepth++;
    curFunc += "@" + std::to_string(blockOrder) + std::to_string(blockDepth);
    visitChildren(context);
    if (--blockDepth == 0) {
        blockOrder++;
    }

    int pos = curFunc.find_last_of('@');
    curFunc = curFunc.substr(0, pos);
    return retType::INT;
}



antlrcpp::Any Allocator::visitFuncCall(MiniDecafParser::FuncCallContext *context)
{
    std::string funcName = context->Identifier()->getText();
    if (funcTable[funcName] == false) {
        std::cerr << "[ERROR] Use of undefined function " << curFunc << "\n";
        exit(1);
    }
    return retType::UNDEF;
}

antlrcpp::Any Allocator::visitGlobalVar(MiniDecafParser::GlobalVarContext *context)
{
    std::string varName = context->Identifier()->getText();
    if (varTab["global"].count(varName) > 0) {
        std::cerr << "[ERROR] Redefinition of global variable " << varName << "\n";
        exit(1);
    }
    varTab["global"][varName] = nullptr;
    return retType::INT;
}

