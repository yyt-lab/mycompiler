#include <iostream>
#include <fstream>
#include "/root/minidecaf/generated/MiniDecafLexer.h"
#include "/root/minidecaf/generated/MiniDecafParser.h"
#include "CodeGenVisitor.h"
#include "/root/minidecaf/third_party/antlr4-runtime/antlr4-runtime.h"
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
    MiniDecafParser::ProgContext* treeNode = parser.prog();
    CodeGenVisitor codeGenVisitor;
    string asmCode = codeGenVisitor.visitProg(treeNode);

    // We get the asm code!
    outFile << asmCode << endl;
    return 0;
}

