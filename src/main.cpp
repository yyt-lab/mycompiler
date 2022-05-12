#include <iostream>
#include <fstream>
#include "MiniDecafLexer.h"
#include "MiniDecafParser.h"
#include "CodeGenVisitor.h"
#include "Allocator.h"
#include "riscv_md.hpp"
using namespace antlr4;
using namespace std;

int main(int argc, const char* argv[]) {
    if (argc != 3) {
        std::cerr << "[error] Input sourcefile required: ./MiniDecaf <sourcefile>\n";
        std::cerr << "[error] Ouput sourcefile required: ./MiniDecaf <sourcefile>\n";
        return 1;
    }

    ifstream sourceFile;
    ofstream outFile;
    MachineDesc* md = new RiscvDesc();
    sourceFile.open(argv[1]);
    outFile.open(argv[2]);

    // lexer & parser part of antlr4
    ANTLRInputStream input(sourceFile);
    MiniDecafLexer lexer(&input);
    CommonTokenStream tokens(&lexer);
    MiniDecafParser parser(&tokens);

    // customized pass: allocator, typer, codegen and etc.
    // cout<<(parser.func()->toStringTree(true));
    
    MiniDecafParser::ProgContext* treeNode = parser.prog();
    CodeGenVisitor codeGenVisitor;
    Allocator allocator;
    symTab<Temp> varTab = allocator.visitProg(treeNode);

    codeGenVisitor.visitProg(treeNode, varTab);
    Piece *ir = codeGenVisitor.translate();
    ir->dump(std::cout);
    // std::cout<<std::endl;
    codeGenVisitor.DumpIR(std::cout);
    
    // result << std::endl;
    // result.flush();
    md->emitPieces(ir, outFile);
    return 0;

    // We get the asm code!
    // outFile << asmCode << endl;
    // return 0;
}

