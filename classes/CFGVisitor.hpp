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
#include <clang/Basic/SourceLocation.h>
#include <clang/Basic/SourceManager.h>
#include <clang/Rewrite/Core/Rewriter.h>
#include <llvm/Support/raw_ostream.h>
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

    SourceManager &SM = Context.getSourceManager();
    SourceLocation BeginLoc = S->getBeginLoc();
    StringRef lineString = "";
    if (BeginLoc.isValid()) {
        PresumedLoc PLoc = SM.getPresumedLoc(BeginLoc);
        if (PLoc.isValid()) {
            unsigned Line = PLoc.getLine();

            SourceLocation LineStart = SM.translateLineCol(SM.getFileID(BeginLoc), Line, 1);
            SourceLocation LineEnd = SM.translateLineCol(SM.getFileID(BeginLoc), Line, UINT_MAX);
            SourceRange LineRange(LineStart, LineEnd);

            Rewriter Rewriter;
            Rewriter.setSourceMgr(SM, Context.getLangOpts());
            lineString = Lexer::getSourceText(CharSourceRange::getTokenRange(LineRange), SM, Context.getLangOpts());
        }
    }

    if (isa<CallExpr>(S)) {
        Stream << "FUNCTION CALLED: ";
        S->printPretty(Stream, nullptr, PrintingPolicy(Context.getLangOpts()));
        Stream << "\\l";
    } else if (isa<DeclStmt>(S)) {
        Stream << "DECLARATION: ";
        S->printPretty(Stream, nullptr, PrintingPolicy(Context.getLangOpts()));
        Stream << "\\l";
    } else if (const ReturnStmt *RS = dyn_cast<ReturnStmt>(S)) {
        string ReturnStr;
        raw_string_ostream ReturnStream(ReturnStr);
        RS->printPretty(ReturnStream, nullptr, PrintingPolicy(Context.getLangOpts()));
        ReturnStream.flush();
        Stream << "  RETURNED.";
    } else {
        if (isa<BinaryOperator>(S)) {
            if (lineString.find("while") != llvm::StringRef::npos) {
                Stream << "WHILE LOOP CONDITION: ";
            } else if (lineString.find("for") != llvm::StringRef::npos) {
                Stream << "FOR LOOP CONDITION: ";
            } else if (lineString.find("if") != llvm::StringRef::npos) {
                Stream << "IF CONDITION: ";
            }
        }
        S->printPretty(Stream, nullptr, PrintingPolicy(Context.getLangOpts()));
        if (lineString.find("return") == llvm::StringRef::npos) {
                Stream << "\\l";
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
    } else if (Block->empty()) {
        // While loops make empty blocks
        BlockStream << "LOOP";
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