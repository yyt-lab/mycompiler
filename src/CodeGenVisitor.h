#pragma once

#include "/root/minidecaf/generated/MiniDecafBaseVisitor.h"
#include <string>
#include <iostream>
#include "/root/minidecaf/third_party/antlr4-runtime/antlr4-runtime.h"

class CodeGenVisitor : public MiniDecafBaseVisitor {
public:
    antlrcpp::Any visitProg(MiniDecafParser::ProgContext *ctx);
    // antlrcpp::Any visitFunc(MiniDecafParser::FuncContext *context);
    antlrcpp::Any visitReturnStmt(MiniDecafParser::ReturnStmtContext *ctx);
    antlrcpp::Any visitUnaryOp(MiniDecafParser::UnaryOpContext *ctx);
    antlrcpp::Any visitInteger(MiniDecafParser::IntegerContext *ctx);
    
    antlrcpp::Any visitAdd_nop(MiniDecafParser::Add_nopContext *context);
    antlrcpp::Any visitAutoParen(MiniDecafParser::AutoParenContext *context);
    antlrcpp::Any visitMul_nop(MiniDecafParser::Mul_nopContext *context);
    antlrcpp::Any visitAddSub(MiniDecafParser::AddSubContext *context);
    antlrcpp::Any visitFactor_nop(MiniDecafParser::Factor_nopContext *context);
    antlrcpp::Any visitMulDiv(MiniDecafParser::MulDivContext *context);
    
private:
    /*
        Stringstream used to store generated codes
    */
    std::ostringstream code_;
    /* 
        A simple stack machine model 
        Support basic push, pop1 & pop2 operations
    */
    const char* push = "\taddi sp, sp, -4\n"
                       "\tsw a0, (sp)\n";
    const char* pop2 = "\tlw t0, (sp)\n"
                       "\tlw t1, 4(sp)\n"
                       "\taddi sp, sp, 8\n";
    const char* pop1 = "\tlw t0, (sp)\n"
                       "\taddi sp, sp, 4\n";
    /*
        Specify return type of each operation
    */
    enum retType {UNDEF, INT};
};