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

/**
 * This class creates an instance of MyASTConsumer.
 */


class MyFrontendAction : public ASTFrontendAction {
public:
    MyFrontendAction(const string& code, ofstream& DotFile) : code_(code), DotFile(DotFile) {}

    unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI, StringRef file) override {
        /**
         * Creates an instance of MyASTConsumer.
         */
        return make_unique<MyASTConsumer>(code_, DotFile);
    }

private:
    string code_;
    ofstream& DotFile;
};