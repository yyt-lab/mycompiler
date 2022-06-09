#pragma once

#include "MiniDecafBaseVisitor.h"
#include "string"
#include "unordered_map"
#include "tuple"
#include "tac.hpp"
#include "iostream"
#include "vector"

template<typename T>
using symTab = std::unordered_map<std::string, std::unordered_map<std::string, T>>;

class ArraySymType {
public:
    ArraySymType(){}
    ArraySymType(int _arrLength) {
        size =4 * _arrLength; 
        typeLiteral = "Array"; 
        valueType = 0;
    }

    std::string getType() { return typeLiteral; }

    int getSize() {return this->size;}
    std::vector<int> getDim() {return dimVal;}
    Temp getStartNode() {return startNode;}


    void setDim(int depthNum, int value) {
        depthNum = depthNum; 
        dimVal.push_back(value);
        // dimVal = new int(value);
        // dimVal++;
    }
    void setSize (int val) { size = val; }
    void setStartNode(Temp tmp) {startNode = tmp;}

protected:
    int valueType;
    int size;
    std::string typeLiteral;
    int depthNum;
    std::vector<int> dimVal;
    // int *dimVal;
    Temp startNode;
    // std::shared_ptr<Type> baseType;
};

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
    
    // antlrcpp::Any visitArrayIndex(MiniDecafParser::ArrayIndexContext *context);
    antlrcpp::Any visitGlobalArrayDeclare(MiniDecafParser::GlobalArrayDeclareContext *context);
    antlrcpp::Any visitArrayDeclare(MiniDecafParser::ArrayDeclareContext *context);
    
    static Allocator& getInstance() {
        static Allocator pInstance;
        return pInstance;
    }

    symTab<Temp> varTab;  // 变量表
    std::unordered_map<std::string, bool> funcTable; // 函数表
    symTab<ArraySymType> arrayTable; // 数组表
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
    std::string curFunc;
    int offest;
    int blockDepth;
    int blockOrder;
};

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
//     // // When we define or assign to a varible, we need to check the src & dst type
//     // bool typeCheck(std::shared_ptr<Type> srcType) {
//     //     if (srcType.get()->getType() == typeLiteral && srcType.get()->getStarNum() == starNum) {
//     //         return true;
//     //     }
//     //     return false;
//     // }
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


