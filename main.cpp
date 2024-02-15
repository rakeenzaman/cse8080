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

using namespace std;
using namespace clang;
using namespace clang::tooling;
using namespace llvm;

//Getting file from the command line
static cl::opt<string> InputFile(cl::Positional, cl::desc("<input file>"), cl::Required);

class CFGVisitor {
public:
    CFGVisitor(ofstream& DotFile, ASTContext &Context, const string &FunctionName)
        : DotFile(DotFile), Context(Context), FunctionName(FunctionName) {}

    void VisitCFG(const CFG& cfg) {

        //Starting off the DOT graph
        DotFile << "digraph \"" << FunctionName << "\" {\n";
        DotFile << "  node [shape=rectangle];\n";

        // Visiting each block
        for (const CFGBlock *Block : cfg) {
            VisitBlock(Block, Context, DotFile, cfg); 
        }

        // Printing the edges
        for (const CFGBlock *Block : cfg) {
            for (CFGBlock::const_succ_iterator I = Block->succ_begin(); I != Block->succ_end(); ++I) {
                if (const CFGBlock *Succ = *I) {
                    DotFile << "  Block" << Block->getBlockID() << " -> Block" << Succ->getBlockID() << ";\n";
                }
            }
        }

        DotFile << "}\n";
    }

private:
    ofstream& DotFile;
    ASTContext &Context;
    string FunctionName;

    void PrintStatement(const Stmt *S, ASTContext &Context, raw_string_ostream &Stream) {
    if (!S) return;

    // Handle if statements
    if (const IfStmt *IfS = dyn_cast<IfStmt>(S)) {
        Stream << "If (";
        IfS->getCond()->printPretty(Stream, nullptr, PrintingPolicy(Context.getLangOpts()));
        Stream << ")\\l";

        PrintStatement(IfS->getThen(), Context, Stream);
        if (IfS->getElse()) {
            Stream << "Else\\l";
            PrintStatement(IfS->getElse(), Context, Stream);
        }
    // Handle declarations
    } else if (isa<DeclStmt>(S)) {
        Stream << "Declaration: ";
        S->printPretty(Stream, nullptr, PrintingPolicy(Context.getLangOpts()));
        Stream << "\\l";
    } else if (const ReturnStmt *RS = dyn_cast<ReturnStmt>(S)){
        string ReturnStr;
        raw_string_ostream ReturnStream(ReturnStr);
        RS->printPretty(ReturnStream, nullptr, PrintingPolicy(Context.getLangOpts()));
        ReturnStream.flush();


        size_t pos = ReturnStr.find("return");
        if (pos != string::npos) {
            ReturnStr = ReturnStr.substr(0, pos); 
        }
        Stream << ReturnStr << " Returned";
    }else {
        S->printPretty(Stream, nullptr, PrintingPolicy(Context.getLangOpts()));
        Stream << "\\l";
    }

    // handle compound statements, need to do more testing here
    if (const CompoundStmt *CS = dyn_cast<CompoundStmt>(S)) {
        for (const Stmt *Child : CS->body()) {
            PrintStatement(Child, Context, Stream);
        }
    }

}

// Visiting blocks of the CFG built by clang
void VisitBlock(const CFGBlock *Block, ASTContext &Context, ofstream& DotFile, const CFG &cfg) { 
    string BlockLabel;
    raw_string_ostream BlockStream(BlockLabel);

    if (Block == &cfg.getEntry()) {
        // Labeling entry block
        BlockStream << "entry point";
    } else if (Block == &cfg.getExit()) {
        // Labeling exit block
        BlockStream << "end";
    } else {
        for (const CFGElement &Element : *Block) {
            if (Element.getKind() == CFGElement::Statement) {
                const Stmt *S = Element.castAs<CFGStmt>().getStmt();
                PrintStatement(S, Context, BlockStream);
            }
        }
    }

    // Printing everything out in DOT format
    DotFile << "  Block" << Block->getBlockID() << " [label=\"" << BlockStream.str() << "\\l\"];\n";
}


// Helper function to determine statement label based on statement type
string GetStatementLabel(const Stmt *Statement) {
    if (!Statement) return "Basic Block";

    // Recursively check the nested statements
    if (const CompoundStmt *CompoundStatement = dyn_cast<CompoundStmt>(Statement)) {
        for (const auto &Child : CompoundStatement->children()) {
            string label = GetStatementLabel(Child);
            if (label != "Basic Block") {
                return label;
            }
        }
    } else if (isa<IfStmt>(Statement)) {
        return "If";
    } else if (isa<ForStmt>(Statement)) {
        return "For";
    } else if (isa<WhileStmt>(Statement)) {
        return "While";
    } else if (isa<DoStmt>(Statement)) {
        return "Do-While";
    }
    return "Basic Block";
}


};

// Overriding ASTConsumer
class MyASTConsumer : public ASTConsumer {
public:
    MyASTConsumer(const string& code, ofstream& DotFile) : code_(code), DotFile(DotFile) {}

    void HandleTranslationUnit(ASTContext &Context) override {

    ASTConsumer::HandleTranslationUnit(Context);

    SourceManager &sourceManager = Context.getSourceManager();

    for (const Decl *decl : Context.getTranslationUnitDecl()->decls()) {
        // Checking for function decl
        if (const FunctionDecl *funcDecl = dyn_cast<FunctionDecl>(decl)) {
            // Checking if the function is defined in main.cpp
            if (funcDecl->hasBody() && sourceManager.isInMainFile(funcDecl->getLocation())) { 
            string FunctionName = funcDecl->getNameAsString();
                CFG::BuildOptions BuildOpts;
                // Using built-in builgCFG function for clang
                unique_ptr<CFG> FuncCFG = CFG::buildCFG(funcDecl, funcDecl->getBody(), &Context, BuildOpts);
                // Visiting CFG
                if (FuncCFG) {
                    CFGVisitor Visitor(DotFile, Context, FunctionName);
                    Visitor.VisitCFG(*FuncCFG);
                }
            }
        }
    }
}



private:
    string code_;
    ofstream& DotFile;
};

// Overriding ASTFrontendAction
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

int main(int argc, const char *argv[]) {
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
