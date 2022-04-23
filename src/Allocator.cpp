#include "Allocator.h"

antlrcpp::Any Allocator::visitProg(MiniDecafParser::ProgContext *context)
{
    offest = 0;
    visitChildren(context);
    return varTab;

}
antlrcpp::Any Allocator::visitFunc(MiniDecafParser::FuncContext *context)
{
    curFunc = context->Identifier()->getText();
    visitChildren(context);
    return retType::INT;
}

antlrcpp::Any Allocator::visitDeclare(MiniDecafParser::DeclareContext *context)
{
    std::string varName = context->Identifier()->getText();
    varTab[curFunc][varName] = offest;
    offest++;
    return retType::INT;
}

antlrcpp::Any Allocator::visitAssign(MiniDecafParser::AssignContext *context)
{
    std::string varName = context->Identifier()->getText();
    if (varTab[curFunc].find(varName) == varTab[curFunc].end())
    {
        std::cerr<<"undefined variable name :"<<varName<<std::endl;
    }
    return retType::INT;
}
    