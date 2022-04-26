#pragma once

#include "MiniDecafBaseVisitor.h"
#include "Allocator.h"
#include "tac.hpp"
#include "trans_helper.hpp"
#include <string>
#include <iostream>

// using namespace tac;
class CodeGenVisitor : public MiniDecafBaseVisitor {
public:
    antlrcpp::Any visitProg(MiniDecafParser::ProgContext *ctx, symTab<int>& varTab);
    antlrcpp::Any visitFunc(MiniDecafParser::FuncContext *context);
    antlrcpp::Any visitReturnStmt(MiniDecafParser::ReturnStmtContext *ctx);
    antlrcpp::Any visitUnaryOp(MiniDecafParser::UnaryOpContext *ctx);
    antlrcpp::Any visitInteger(MiniDecafParser::IntegerContext *ctx);
    
    antlrcpp::Any visitLor_nop(MiniDecafParser::Lor_nopContext *context);
    antlrcpp::Any visitLand_nop(MiniDecafParser::Land_nopContext *context);
    antlrcpp::Any visitLor(MiniDecafParser::LorContext *context);
    antlrcpp::Any visitLand(MiniDecafParser::LandContext *context);
    antlrcpp::Any visitEqu_nop(MiniDecafParser::Equ_nopContext *context);
    antlrcpp::Any visitEqual(MiniDecafParser::EqualContext *context);
    antlrcpp::Any visitRel_nop(MiniDecafParser::Rel_nopContext *context);
    antlrcpp::Any visitLegt(MiniDecafParser::LegtContext *context);
    
    antlrcpp::Any visitPrimary_nop(MiniDecafParser::Primary_nopContext *context);
    antlrcpp::Any visitAdd_nop(MiniDecafParser::Add_nopContext *context);
    antlrcpp::Any visitAutoParen(MiniDecafParser::AutoParenContext *context);
    antlrcpp::Any visitMul_nop(MiniDecafParser::Mul_nopContext *context);
    antlrcpp::Any visitAddSub(MiniDecafParser::AddSubContext *context);
    antlrcpp::Any visitFactor_nop(MiniDecafParser::Factor_nopContext *context);
    antlrcpp::Any visitMulDiv(MiniDecafParser::MulDivContext *context);
    
    Piece *translate();
    void DumpIR (std::ostream &os);

    CodeGenVisitor();

private:
    /*
        Stringstream used to store generated codes
    */
    std::ostringstream code_;
    std::string curFunc;
    symTab<int> varTab;
    TransHelper *tr;
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