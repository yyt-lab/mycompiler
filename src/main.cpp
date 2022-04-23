#include <iostream>
#include <fstream>
#include "MiniDecafLexer.h"
#include "MiniDecafParser.h"
#include "CodeGenVisitor.h"
#include "Allocator.h"
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
    symTab<int> varTab = allocator.visitProg(treeNode);
    string asmCode = codeGenVisitor.visitProg(treeNode, varTab);

    // We get the asm code!
    outFile << asmCode << endl;
    return 0;
}

