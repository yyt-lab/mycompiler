#pragma once

#include "MiniDecafBaseVisitor.h"
#include "string"
#include "unordered_map"
#include "tuple"
#include "tac.hpp"

template<typename T>
using symTab = std::unordered_map<std::string, std::unordered_map<std::string, T>>;

class Allocator: public MiniDecafBaseVisitor {

    public:
    antlrcpp::Any visitProg(MiniDecafParser::ProgContext *context);
    antlrcpp::Any visitFunc(MiniDecafParser::FuncContext *context);
    antlrcpp::Any visitDeclare(MiniDecafParser::DeclareContext *context);
    antlrcpp::Any visitAssign(MiniDecafParser::AssignContext *context);
    antlrcpp::Any visitBlock(MiniDecafParser::BlockContext *context);
    antlrcpp::Any visitIdentifier(MiniDecafParser::IdentifierContext *context);
    antlrcpp::Any visitForLoop(MiniDecafParser::ForLoopContext *context);
    antlrcpp::Any visitFuncCall(MiniDecafParser::FuncCallContext *context);
    antlrcpp::Any visitParameter_list(MiniDecafParser::Parameter_listContext *context);
    antlrcpp::Any visitGlobalVar(MiniDecafParser::GlobalVarContext *context);
    
    private:
    /*
        Symbol table, which stores function, scope & varible information.
        The table is organized as follows:
        ------------------------------------------------------
        | Function | Variable1 | Offset in stack | Data type |
        |          | Variable2 | ...             | ...       |
        |          | ...       | ...             | ...       |
        ------------------------------------------------------
    */
    enum retType {INT, UNDEF}; 
    symTab<Temp> varTab;
    std::unordered_map<std::string, bool> funcTable;
    std::string curFunc;
    int offest;
    int blockDepth;
    int blockOrder;
};