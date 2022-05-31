#pragma once

#include "MiniDecafBaseVisitor.h"
#include "string"
#include "unordered_map"
#include "tuple"
#include "tac.hpp"

template<typename T>
using symTab = std::unordered_map<std::string, std::unordered_map<std::string, T>>;

// class Type {
// public:
//     virtual std::string getType() = 0;
//     // Explicitly cast to dst type
//     virtual std::shared_ptr<Type> typeCast(int _valueType) = 0;
//     // Distinguish different pointer type by counting thier number of "*"
//     virtual int getStarNum() { return starNum; }
//     virtual int getSize() { return size; }
//     virtual std::shared_ptr<Type> getBaseType() { return baseType; }
//     void setValueType(int _valueType) { valueType = _valueType; }
//     int getValueType() { return valueType; }
//     // When we define or assign to a varible, we need to check the src & dst type
//     bool typeCheck(std::shared_ptr<Type> srcType) {
//         if (srcType.get()->getType() == typeLiteral && srcType.get()->getStarNum() == starNum) {
//             return true;
//         }
//         return false;
//     }
//     bool typeCheckLiteral(std::string srcType) { 
//         if (srcType == typeLiteral) {
//             return true;
//         }
//         return false;
//     }
//     virtual ~Type() {}
// protected:
//     int valueType;
//     int starNum;
//     int size;
//     std::string typeLiteral;
//     std::shared_ptr<Type> baseType;
// };


// class FuncSymbol {
// public:
//     FuncSymbol(std::string _literal, std::shared_ptr<Type> _retType, 
//     std::vector<std::shared_ptr<Type> > _argType, bool _init=false) {
//         literal = _literal; retType = _retType; argType = _argType; init = _init;
//     }
//     std::string getLiteral() { return literal; }
//     std::shared_ptr<Type> getRetType() { return retType; }
//     std::shared_ptr<Type> getArgType(int i) { return argType[i]; }
//     int getArgSize() { return argType.size(); }
//     bool initialized() { return init; }
//     void initialize() { init = true; }

// protected:
//     bool init;
//     std::string literal;
//     std::vector<std::shared_ptr<Type> > argType;
//     std::shared_ptr<Type> retType;
// };

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