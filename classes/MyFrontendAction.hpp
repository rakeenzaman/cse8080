#include "MyASTConsumer.hpp"
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

class MyFrontendAction : public ASTFrontendAction {
public:
    MyFrontendAction(const string& code, ofstream& DotFile) : code_(code), DotFile(DotFile) {}

    // Creating an instance of MyASTConsumer
    unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI, StringRef file) override {
        return make_unique<MyASTConsumer>(code_, DotFile);
    }

private:
    string code_;
    ofstream& DotFile;
};