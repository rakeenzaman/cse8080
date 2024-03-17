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

class CFGVisitor {
public:
    CFGVisitor(ofstream& DotFile, ASTContext &Context, const string &FunctionName, const string &FunctionType, const string &FunctionParameters)
        : DotFile(DotFile), Context(Context), FunctionName(FunctionName), FunctionType(FunctionType), FunctionParameters(FunctionParameters) {}

    void VisitCFG(const CFG& cfg) {

        //Starting off the DOT graph
        DotFile << "digraph \"" << (FunctionName + ";" + FunctionType + ";" + FunctionParameters) << "\" {\n";
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
    string FunctionType;
    string FunctionParameters;

    void PrintStatement(const Stmt *S, ASTContext &Context, raw_string_ostream &Stream) {
    if (!S) return;

    cout << S->getStmtClassName() << endl;

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
    } 
    // Handle declarations
    /*
    else if (isa<DeclStmt>(S)) {
        Stream << "Declaration: ";
        S->printPretty(Stream, nullptr, PrintingPolicy(Context.getLangOpts()));
        Stream << "\\l";
    } 
    */
    else if (isa<WhileStmt>(S)) {
        Stream << "While: ";
        S->printPretty(Stream, nullptr, PrintingPolicy(Context.getLangOpts()));
        Stream << "\\l";
    } 
    else if (isa<CallExpr>(S)) {
        Stream << "Function Called: ";
        S->printPretty(Stream, nullptr, PrintingPolicy(Context.getLangOpts()));
        Stream << "\\l";
    }
    else if (const ReturnStmt *RS = dyn_cast<ReturnStmt>(S)){
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

string escapeDoubleQuotes(const string& input) {
    string output;

    for (char ch : input) {
        if (ch == '\"') {
            output += "\\\"";
        } else {
            output += ch;
        }
    }
    return output;
}

// Visiting blocks of the CFG built by clang
void VisitBlock(const CFGBlock *Block, ASTContext &Context, ofstream& DotFile, const CFG &cfg) { 
    string BlockLabel;
    raw_string_ostream BlockStream(BlockLabel);

    if (Block == &cfg.getEntry()) {
        // Labeling entry block
        BlockStream << "ENTRY POINT";
    } else if (Block == &cfg.getExit()) {
        // Labeling exit block
        BlockStream << "END";
    } else {
        for (const CFGElement &Element : *Block) {
            if (Element.getKind() == CFGElement::Statement) {
                const Stmt *S = Element.castAs<CFGStmt>().getStmt();
                PrintStatement(S, Context, BlockStream);
            }
        }
    }

    // Printing everything out in DOT format
    DotFile << "  Block" << Block->getBlockID() << " [label=\"" << escapeDoubleQuotes(BlockStream.str()) << "\\l\"];\n";
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
            string FunctionType = funcDecl->getReturnType().getAsString();
            string FunctionParameters = funcParamVectorToString(getFunctionParamsAsVector(funcDecl));
                CFG::BuildOptions BuildOpts;
                // Using built-in builgCFG function for clang
                unique_ptr<CFG> FuncCFG = CFG::buildCFG(funcDecl, funcDecl->getBody(), &Context, BuildOpts);
                // Visiting CFG
                if (FuncCFG) {
                    CFGVisitor Visitor(DotFile, Context, FunctionName, FunctionType, FunctionParameters);
                    Visitor.VisitCFG(*FuncCFG);
                }
            }
        }
    }
    cout << "finished";
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
