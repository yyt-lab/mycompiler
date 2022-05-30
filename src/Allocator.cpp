#include "Allocator.h"

antlrcpp::Any Allocator::visitProg(MiniDecafParser::ProgContext *context)
{
    offest = 0;
    blockDepth=-1;
    blockOrder=0;
    visitChildren(context);
    return varTab;

}
antlrcpp::Any Allocator::visitFunc(MiniDecafParser::FuncContext *context)
{
    curFunc = context->Identifier()->getText();
    // curFunc += "@" + std::to_string(blockOrder) + "@" + std::to_string(blockDepth);
    visitChildren(context);
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
