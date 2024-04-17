#include "classes/MyFrontendAction.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <llvm/Support/CommandLine.h>
#include <clang/AST/AST.h>
#include <clang/AST/ASTConsumer.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Frontend/FrontendAction.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Tooling/Tooling.h>
#include <clang/Analysis/CFG.h>
#include <vector>

using namespace std;
using namespace clang;
using namespace clang::tooling;
using namespace llvm;

//Getting file from the command line
static cl::opt<string> InputFile(cl::Positional, cl::desc("<input file>"), cl::Required);

int main(int argc, const char *argv[]) {
     /**
     * In this function, a dot file is created and the code from input.cpp is parsed and put into a string.
     * With these two objects, an instance of MyFrontendAction is created.
     */
    cl::ParseCommandLineOptions(argc, argv, "CFG Generator");
    // Creates DOT File
    ofstream DotFile("cfg.dot");
    // Error Checking
    if (!DotFile.is_open()) {
        errs() << "Failed to open 'cfg.dot'";
        return 1;
    }
    // Opening file specified in command line argument
    ifstream fileToBeParsed(InputFile);
    // Error Checking
    if (!fileToBeParsed.is_open()) {
        errs() << "Failed to open " << InputFile << "\n";
        return 1;
    }
    // Reading the contents of the file into a variable called 'code'
    string code((istreambuf_iterator<char>(fileToBeParsed)), istreambuf_iterator<char>());
    // Creating an instance of 'MyFrontendAction'
    clang::tooling::runToolOnCode(make_unique<MyFrontendAction>(code, DotFile), code);

    return 0;
}
