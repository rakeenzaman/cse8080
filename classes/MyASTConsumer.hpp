#include "CFGVisitor.hpp"
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

class MyASTConsumer : public ASTConsumer {
public:
    vector<vector<string>> getFunctionParamsAsVector(const FunctionDecl *funcDecl) {
        unsigned numOfParams = funcDecl->getNumParams();
        vector<vector<string>> params;
        for (unsigned i = 0; i < numOfParams; i++) {
            vector<string> param;
            // setting first value in vector as the param type
            param.push_back(funcDecl->getParamDecl(i)->getType().getNonReferenceType().getUnqualifiedType().getAsString());
            // setting second value as name of param
            param.push_back(funcDecl->getParamDecl(i)->getNameAsString());
            // putting it in vector that will be returned
            params.push_back(param);
        }
        return params;
    }

    string funcParamVectorToString(vector<vector<string>> params) {
        string convertedToString = "";
        for (int i = 0; i < params.size(); i++) {
            convertedToString += (params[i][0] + " " +  params[i][1]);
            if (i != (params.size() - 1)) {
                convertedToString += ", ";
            }
        }
        return convertedToString;
    }
    
    MyASTConsumer(const string& code, ofstream& DotFile) : code_(code), DotFile(DotFile) {}

    void HandleTranslationUnit(ASTContext &Context) override {

    ASTConsumer::HandleTranslationUnit(Context);

    SourceManager &sourceManager = Context.getSourceManager();

    for (const Decl *decl : Context.getTranslationUnitDecl()->decls()) {
        if (const FunctionDecl *funcDecl = dyn_cast<FunctionDecl>(decl)) {
            if (funcDecl->hasBody() && sourceManager.isInMainFile(funcDecl->getLocation())) {
                ProcessFunction(funcDecl, Context);
            }
        }
        else if (const CXXRecordDecl *classDecl = dyn_cast<CXXRecordDecl>(decl)) {
            if (classDecl->isThisDeclarationADefinition() && sourceManager.isInMainFile(classDecl->getLocation())) {
                for (const Decl *memberDecl : classDecl->decls()) {
                    if (const CXXMethodDecl *methodDecl = dyn_cast<CXXMethodDecl>(memberDecl)) {
                        if (methodDecl->hasBody()) {
                            ProcessFunction(methodDecl, Context);
                        }
                    }
                }
            }
        }
    }
}

void ProcessFunction(const FunctionDecl *funcDecl, ASTContext &Context) {
    string FunctionName = funcDecl->getNameAsString();
    string FunctionType = funcDecl->getReturnType().getAsString();
    string FunctionParameters = funcParamVectorToString(getFunctionParamsAsVector(funcDecl));
    CFG::BuildOptions BuildOpts;
    unique_ptr<CFG> FuncCFG = CFG::buildCFG(funcDecl, funcDecl->getBody(), &Context, BuildOpts);
    if (FuncCFG) {
        CFGVisitor Visitor(DotFile, Context, FunctionName, FunctionType, FunctionParameters);
        Visitor.VisitCFG(*FuncCFG);
    }
}

private:
    string code_;
    ofstream& DotFile;
};