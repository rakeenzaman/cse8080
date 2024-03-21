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
        Stream << " returned.";
    }else {
        S->printPretty(Stream, nullptr, PrintingPolicy(Context.getLangOpts()));
        Stream << "\\l";
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